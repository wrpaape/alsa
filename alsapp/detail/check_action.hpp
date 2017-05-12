#ifndef ALSAPP_DETAIL_CHECK_ACTION_HPP
#define ALSAPP_DETAIL_CHECK_ACTION_HPP

// EXTERNAL DEPENDENCIES
// =============================================================================
#include "alsapp/detail/alsa_interface.h" // snd_strerror
#include <stdexcept>                      // std::runtime_error
#include <string>                         // std::string



// EXTERNAL API
// =============================================================================
namespace alsapp {
namespace detail {

// throw runtime error if action failed
inline void
check_action(const char *const action,
             const int status)
{
    if (status < 0) {
        std::string what_arg("failed to ");
        what_arg += action;
        what_arg += ": ";
        what_arg += snd_strerror(status);
        throw std::runtime_error(what_arg);
    }
}

} // namespace detail
} // namespace alsapp

#endif  // ifndef ALSAPP_DETAIL_CHECK_ACTION_HPP
