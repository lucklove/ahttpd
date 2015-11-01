#include "exception.hh"

namespace ahttpd {

const char *Exception::what() const noexcept
{
    return xm_pszMessage;
}

}
