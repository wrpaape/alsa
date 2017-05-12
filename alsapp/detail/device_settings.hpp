#ifndef ALSAPP_DETAIL_DEVICE_SETTINGS_HPP
#define ALSAPP_DETAIL_DEVICE_SETTINGS_HPP

// EXTERNAL DEPENDENCIES
// =============================================================================
#include "alsapp/detail/alsa_interface.h" // snd_pcm_*
#include "alsapp/detail/check_action.hpp" // alsapp::detail::check_action



// EXTERNAL API
// =============================================================================
namespace alsapp {
namespace detail {

class DeviceSettings
{
public:
    DeviceSettings(snd_pcm_t *const device_handle)
        : device_handle(device_handle)
    {
        detail::check_action("allocate hardware parameters structure",
                             snd_pcm_hw_params_malloc(&hw_params_handle));

        detail::check_action("initialize hardware parameters structure",
                             snd_pcm_hw_params_any(device_handle,
                                                   hw_params_handle));
    }

    ~DeviceSettings()
    {
        snd_pcm_hw_params_free(hw_params_handle);
    }

    void
    set_access_mode(const snd_pcm_access_t access_mode)
    {
        detail::check_action("set access mode",
                             snd_pcm_hw_params_set_access(device_handle,
                                                          hw_params_handle,
                                                          access_mode));
    }

    void
    set_sample_format(const snd_pcm_format_t sample_format)
    {
        detail::check_action("set sample format",
                             snd_pcm_hw_params_set_format(device_handle,
                                                          hw_params_handle,
                                                          sample_format));
    }

    void
    set_channel_count(const unsigned int count_channels)
    {
        detail::check_action("set channel count",
                             snd_pcm_hw_params_set_channels(device_handle,
                                                            hw_params_handle,
                                                            count_channels));
    }

    void
    set_sample_rate(const unsigned int sample_rate)
    {
        detail::check_action("set sample rate",
                             snd_pcm_hw_params_set_rate(device_handle,
                                                        hw_params_handle,
                                                        sample_rate,
                                                        0));
    }

    void
    set_period_frame_size(const snd_pcm_uframes_t period_frame_size)
    {
        detail::check_action(
            "set period (frame) size",
            snd_pcm_hw_params_set_period_size(device_handle,
                                              hw_params_handle,
                                              period_frame_size,
                                              0)
        );
    }

    void
    finalize()
    {
        detail::check_action("finalize device settings",
                             snd_pcm_hw_params(device_handle,
                                               hw_params_handle));
    }


private:
    snd_pcm_t *const device_handle;
    snd_pcm_hw_params_t *hw_params_handle;
}; // class DeviceSettings

} // namespace detail
} // namespace alsapp

#endif  // ifndef ALSAPP_DETAIL_DEVICE_SETTINGS_HPP
