#ifndef ALSA_HPP
#define ALSA_HPP

// EXTERNAL_DEPENDENCIES
// =============================================================================
#define ALSA_PCM_NEW_HW_PARAMS_API // use the newer ALSA API
#include <alsa/asoundlib.h>        // ALSA API
#include <stdexcept>               // std::runtime_error
#include <string>                  // std::string

namespace alsa {

class MicrophoneFailure : public std::runtime_error {
public:
    explicit MicrophoneFailure(const std::string &what_arg)
        : std::runtime_error(what_arg)
    {}

    explicit MicrophoneFailure(const char *what_arg)
        : std::runtime_error(what_arg)
    {}

    MicrophoneFailure(const std::string &prefix,
                      const int error_code)
        : std::runtime_error(prefix + ": " + snd_strerror(error_code))
    {}
}; // class MicrophoneFailure




class Microphone
{
public:
    Microphone()
    {

        snd_pcm_hw_params_t hw_params;

        open_handle();

        init_hw_params(hw_params);
        set_access_type(hw_params);
        set_sample_format(hw_params);
        set_channel_count(hw_params);
        set_sample_rate(hw_params);
        set_period_size(hw_params);

    }

    ~Microphone()
    {
        /* free_hw_params(); */
        close_handle();
    }



private:
    class MicrophoneSettings
    {
        MicrophoneSettings()
        {
        }


    } // class MicrophoneSettings

    // PCM Settings
    // -------------------------------------------------------------------------
    static const char *name                     = "default";
    static const snd_pcm_stream_t stream        = SND_PCM_STREAM_CAPTURE;
    static const int open_mode                  = 0; // blocking
    static const snd_pcm_access_t access_type   = SND_PCM_ACCESS_RW_INTERLEAVED;
    static const snd_pcm_format_t sample_format = SND_PCM_FORMAT_S16_LE;
    static const unsigned int count_channels    = 2; // stereo
    static const unsigned int sample_rate       = 16000; // bits/second
    static const snd_pcm_uframes_t period_size  = 32;

    // instance data
    // -------------------------------------------------------------------------
    snd_pcm_t *device;

    // instance methods
    // -------------------------------------------------------------------------
    void open_handle()
    {
        const int status = snd_pcm_open(&device,
                                        name,
                                        stream,
                                        open_mode);
        if (status < 0)
            throw MicrophoneFailure("cannot open audio device", status);
    }

    void init_hw_params(snd_pcm_hw_params_t &hw_params)
    {
        const int status = snd_pcm_hw_params_any(device,
                                                 &hw_params);
        if (status < 0)
            throw MicrophoneFailure("cannot initialize hardware parameter "
                                    "structure", status);
    }

    void set_access_type(snd_pcm_hw_params_t &hw_params)
    {
        const int status = snd_pcm_hw_params_set_access(device,
                                                        &hw_params,
                                                        access_type);
        if (status < 0)
            throw MicrophoneFailure("cannot set access type", status);
    }

    void set_sample_format(snd_pcm_hw_params_t &hw_params)
    {
        const int status = snd_pcm_hw_params_set_format(device,
                                                        &hw_params,
                                                        sample_format);
        if (status < 0)
            throw MicrophoneFailure("cannot set sample format", status);
    }

    void set_channel_count(snd_pcm_hw_params_t &hw_params)
    {
        const int status = snd_pcm_hw_params_set_channels(device,
                                                          &hw_params,
                                                          count_channels);
        if (status < 0)
            throw MicrophoneFailure("cannot set channel count", status);
    }


    void set_sample_rate(snd_pcm_hw_params_t &hw_params)
    {
        const int status = snd_pcm_hw_params_set_rate(device,
                                                      &hw_params,
                                                      sample_rate,
                                                      0);
        if (status < 0)
            throw MicrophoneFailure("cannot set sample rate", status);
    }

    void set_period_size(snd_pcm_hw_params_t &hw_params)
    {
        const int status = snd_pcm_hw_params_set_period_size(device,
                                                             &hw_params,
                                                             period_size,
                                                      0);
        if (status < 0)
            throw MicrophoneFailure("cannot set sample rate", status);
    }


    void close_handle()
    {
        (void) snd_pcm_close(device);
    }




}; // class Microphone

} // namespace alsa

#endif // ifndef ALSA_HPP
