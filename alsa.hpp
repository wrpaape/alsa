#ifndef ALSA_HPP
#define ALSA_HPP

// EXTERNAL_DEPENDENCIES
// =============================================================================
#define ALSA_PCM_NEW_HW_PARAMS_API // use the newer ALSA API
#include <alsa/asoundlib.h>        // ALSA API
#include <stdint.h>                // int16_t
#include <stdexcept>               // std::runtime_error
#include <string>                  // std::string
#include <array>                   // std::array
#include <vector>                  // std::vector




namespace alsa {
// GLOBAL TYPEDEFS
// =============================================================================
typedef snd_pcm_uframes_t   frame_size_type;




// PRIVATE IMPLEMENTATION
// =============================================================================
namespace detail {
// typedefs
// -----------------------------------------------------------------------------
typedef snd_pcm_t           device_type;
typedef snd_pcm_stream_t    stream_type;
typedef snd_pcm_format_t    format_type;
typedef snd_pcm_access_t    access_type;
typedef snd_pcm_sframes_t   signed_frame_size_type;
typedef snd_pcm_hw_params_t hw_params_type;



// helper functions
// -----------------------------------------------------------------------------
// throw runtime error if action failed
inline void
check_status(const int status,
             const char *action)
{
    if (status < 0) {
        std::string what_arg("failed to ");
        what_arg += action;
        what_arg += ": ";
        what_arg += snd_strerror(status);
        throw std::runtime_error(what_arg);
    }
}



// base classes
// -----------------------------------------------------------------------------
class Device
{
public:
    Device(const char *name,
           const stream_type stream,
           const int open_mode)
    {
        const int status = snd_pcm_open(&handle,
                                        name,
                                        stream,
                                        open_mode);
        check_status(status,
                     "open audio device");
    }

    ~Device()
    {
        (void) snd_pcm_close(handle);
    }


protected:
    operator device_type *() const
    {
        return handle;
    }


private:
    device_type *handle;
}; // class Device



class DeviceSettings
{
public:
    DeviceSettings(const Device &device)
        : device_handle(device)
    {
        const int status = snd_pcm_hw_params_any(device_handle,
                                                 &hw_params);
        detail::check_status(status,
                             "initialize hardware parameter structure");
    }

    void
    set_access(const access_type access)
    {
        const int status = snd_pcm_hw_params_set_access(device_handle,
                                                        &hw_params,
                                                        access);
        detail::check_status(status,
                             "set access");
    }

    void
    set_sample_format(const format_type sample_format)
    {
        const int status = snd_pcm_hw_params_set_format(device_handle,
                                                        &hw_params,
                                                        sample_format);
        detail::check_status(status,
                             "set sample format");
    }

    void
    set_channel_count(const unsigned int count_channels)
    {
        const int status = snd_pcm_hw_params_set_channels(device_handle,
                                                          &hw_params,
                                                          count_channels);
        detail::check_status(status,
                             "set channel count");
    }

    void
    set_sample_rate(const unsigned int sample_rate)
    {
        const int status = snd_pcm_hw_params_set_rate(device_handle,
                                                      &hw_params,
                                                      sample_rate,
                                                      0);
        detail::check_status(status,
                             "set sample rate");
    }

    void
    set_period_size(const frame_size_type period_size)
    {
        const int status = snd_pcm_hw_params_set_period_size(device_handle,
                                                             &hw_params,
                                                             period_size,
                                                             0);
        detail::check_status(status,
                             "set period size");
    }

    void
    finalize()
    {
        const int status = snd_pcm_hw_params(device_handle,
                                             &hw_params);
        detail::check_status(status,
                             "finalize device settings");
    }


private:
    device_type    *device_handle;
    hw_params_type  hw_params;
}; // class DeviceSettings

} // namespace detail




// EXTERNAL API
// =============================================================================
class Microphone : private detail::Device
{
public:
    typedef int16_t frame_type;

    Microphone()
        : detail::Device("default",              // device name
                         SND_PCM_STREAM_CAPTURE, // capture stream
                         0)                      // blocking mode
    {
        detail::DeviceSettings settings(*this);

        // grant access to interleaved channel read (and write)
        settings.set_access(SND_PCM_ACCESS_RW_INTERLEAVED);

        // signed, 16-bit, little-endian
        settings.set_sample_format(SND_PCM_FORMAT_S16_LE);

        // two channels (stereo)
        settings.set_channel_count(2);

        // sample at 16000 bits/second
        settings.set_sample_rate(16000);

        // set period size to 32 frames
        settings.set_period_size(32);

        settings.finalize();
    }

    frame_size_type
    read(frame_type *buffer,
         frame_size_type frame_count)
    {
        detail::signed_frame_size_type frames_read;

        frames_read = snd_pcm_readi(*this,
                                    buffer,
                                    frame_count);

        // blocking, no need to check for EAGAIN
        detail::check_status(static_cast<int>(frames_read),
                             "read from microphone");

        return static_cast<frame_size_type>(frames_read);
    }

    template<frame_size_type capacity>
    frame_size_type
    read(frame_type buffer[capacity])
    {
        return read(&buffer[0],
                    capacity);
    }

    frame_size_type
    read(std::array<frame_type> &buffer)
    {
        return read(buffer.data(),
                    static_cast<frame_size_type>(buffer.max_size()));
    }

    frame_size_type
    read(std::vector<frame_type> &buffer)
    {
        return read(buffer.data(),
                    static_cast<frame_size_type>(buffer.capacity()));
    }
}; // class Microphone

} // namespace alsa

#endif // ifndef ALSA_HPP
