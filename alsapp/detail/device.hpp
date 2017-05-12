#ifndef ALSAPP_DETAIL_ALSA_DEVICE_HPP
#define ALSAPP_DETAIL_ALSA_DEVICE_HPP

// EXTERNAL DEPENDENCIES
// =============================================================================
#include "alsapp/detail/alsa_interface.h" // snd_pcm_[[stream]_t|open|close]
#include "alsapp/detail/check_action.hpp" // alsapp::detail::check_action



// EXTERNAL API
// =============================================================================
namespace alsapp {
namespace detail {

class Device
{
public:
    Device(const char *name,
           const snd_pcm_stream_t stream,
           const int open_mode)
    {
        const int status = snd_pcm_open(&handle,
                                        name,
                                        stream,
                                        open_mode);
        check_action("open audio device",
                     status);
    }

    ~Device()
    {
        (void) snd_pcm_close(handle);
    }


protected:
    operator snd_pcm_t *() const
    {
        return handle;
    }


private:
    snd_pcm_t *handle;
}; // class Device

} // namespace detail
} // namespace alsapp

#endif  // ifndef ALSAPP_DETAIL_ALSA_DEVICE_HPP
