#ifndef ALSAPP_MICROPHONE_HPP
#define ALSAPP_MICROPHONE_HPP
// EXTERNAL DEPENDENCIES
// =============================================================================
#include "alsapp/detail/alsa_interface.h"    // snd_pcm_*, SND_PCM_*
#include "alsapp/detail/check_action.hpp"    // alsapp::detail::check_action
#include "alsapp/detail/device.hpp"          // alsapp::detail::Device
#include "alsapp/detail/device_settings.hpp" // alsapp::detail::DeviceSettings
#include <cstddef>                           // std::size_t
#include <cstdint>                           // std::int16_t



// EXTERNAL API
// =============================================================================
namespace alsapp {

class Microphone : private detail::Device
{
private:
    // Microphone Settings
    // -------------------------------------------------------------------------
    // request a capture stream
    static const snd_pcm_stream_t stream_mode = SND_PCM_STREAM_CAPTURE;

    // open stream in synchronous, blocking mode
    static const int open_mode = 0;
    
    // grant access to interleaved channel read (and write)
    static const snd_pcm_access_t access_mode = SND_PCM_ACCESS_RW_INTERLEAVED;

    // signed, 16-bit, little-endian samples
    static const snd_pcm_format_t sample_format = SND_PCM_FORMAT_S16_LE;
    typedef std::int16_t sample_type;

    // two channels (stereo), two samples per frame
    /* static const unsigned int channel_count = 2; */
    static const unsigned int channel_count = 1;
    typedef sample_type frame_type[channel_count];

    // sample at 16000 bits/second
    static const unsigned int sample_rate = 16000;

    // 32 frames per period
    /* static const snd_pcm_uframes_t period_frame_size = 32; */
    static const snd_pcm_uframes_t period_frame_size = 128;

    // sizeof(period_type)
    static const std::size_t period_size = sizeof(frame_type)
                                         * period_frame_size;


public:
    typedef char period_type[period_size]; // audio units

    /* template<std::size_t size> */
    /* class Buffer { */

    /* }; // class Period */


    Microphone(const char *const device_name = "default")
        : detail::Device(device_name,
                         stream_mode,
                         open_mode) // open device
    {
        detail::DeviceSettings settings(*this);

        // apply settings
        settings.set_access_mode(access_mode);
        settings.set_sample_format(sample_format);
        settings.set_channel_count(channel_count);
        settings.set_sample_rate(sample_rate);
        settings.set_period_frame_size(period_frame_size);
        settings.finalize();
    }


    // read into a period buffer
    std::size_t
    read(period_type *const buffer,
         std::size_t capacity)
    {

        const snd_pcm_uframes_t frame_capacity = capacity * period_frame_size;

        const snd_pcm_sframes_t frames_read = snd_pcm_readi(*this,
                                                            buffer,
                                                            frame_capacity);
        // blocking, no need to check for EAGAIN
        detail::check_action("read from microphone",
                             static_cast<int>(frames_read));

        return static_cast<std::size_t>(frames_read) * sizeof(frame_type);
    }

    // read into a single period
    std::size_t
    read(period_type &period)
    {
        return read(&period, 1);
    }

    // read into a C-style array of periods
    template<std::size_t capacity>
    std::size_t
    read(period_type (&buffer)[capacity])
    {
        return read(&buffer[0], capacity);
    }

    // number of periods required to record 'seconds' seconds of sound
    static constexpr std::size_t
    size_buffer(const std::size_t seconds)
    {
        return ((seconds * sample_rate) / period_frame_size)
             + (((seconds * sample_rate) % period_frame_size) > 0);
    }


    /* // read into a std::array of periods */
    /* template<std::size_t capacity> */
    /* std::size_t */
    /* read(std::array<period_type, capacity> &buffer) */
    /* { */
    /*     return read(buffer.data(), */
    /*                 capacity); */
    /* } */

    /* // read into a std::vector of periods */
    /* std::size_t */
    /* read(std::vector<period_type> &buffer) */
    /* { */
    /*     return read(buffer.data(), */
    /*                 static_cast<frame_size_type>(buffer.capacity())); */
    /* } */
}; // class Microphone

} // namespace alsapp

#endif  // ifndef ALSAPP_MICROPHONE_HPP
