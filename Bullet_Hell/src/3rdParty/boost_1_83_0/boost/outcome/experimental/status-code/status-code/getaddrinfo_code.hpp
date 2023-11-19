/* Proposed SG14 status_code
(C) 2020-2023 Niall Douglas <http://www.nedproductions.biz/> (5 commits)
File Created: Jan 2020


Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#ifndef BOOST_OUTCOME_SYSTEM_ERROR2_GETADDRINFO_CODE_HPP
#define BOOST_OUTCOME_SYSTEM_ERROR2_GETADDRINFO_CODE_HPP

#include "quick_status_code_from_enum.hpp"

#ifdef _WIN32
#error Not available for Microsoft Windows
#else
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif

BOOST_OUTCOME_SYSTEM_ERROR2_NAMESPACE_BEGIN

class _getaddrinfo_code_domain;
//! A getaddrinfo error code, those returned by `getaddrinfo()`.
using getaddrinfo_code = status_code<_getaddrinfo_code_domain>;
//! A specialisation of `status_error` for the `getaddrinfo()` error code domain.
using getaddrinfo_error = status_error<_getaddrinfo_code_domain>;

/*! The implementation of the domain for `getaddrinfo()` error codes, those returned by `getaddrinfo()`.
 */
class _getaddrinfo_code_domain : public status_code_domain
{
  template <class DomainType> friend class status_code;
  template <class StatusCode, class Allocator> friend class detail::indirecting_domain;
  using _base = status_code_domain;

public:
  //! The value type of the `getaddrinfo()` code, which is an `int`
  using value_type = int;
  using _base::string_ref;

  //! Default constructor
  constexpr explicit _getaddrinfo_code_domain(typename _base::unique_id_type id = 0x5b24b2de470ff7b6) noexcept
      : _base(id)
  {
  }
  _getaddrinfo_code_domain(const _getaddrinfo_code_domain &) = default;
  _getaddrinfo_code_domain(_getaddrinfo_code_domain &&) = default;
  _getaddrinfo_code_domain &operator=(const _getaddrinfo_code_domain &) = default;
  _getaddrinfo_code_domain &operator=(_getaddrinfo_code_domain &&) = default;
  ~_getaddrinfo_code_domain() = default;

  //! Constexpr singleton getter. Returns constexpr getaddrinfo_code_domain variable.
  static inline constexpr const _getaddrinfo_code_domain &get();

  virtual string_ref name() const noexcept override { return string_ref("getaddrinfo() domain"); }  // NOLINT

  virtual payload_info_t payload_info() const noexcept override
  {
    return {sizeof(value_type), sizeof(status_code_domain *) + sizeof(value_type),
            (alignof(value_type) > alignof(status_code_domain *)) ? alignof(value_type) : alignof(status_code_domain *)};
  }

protected:
  virtual bool _do_failure(const status_code<void> &code) const noexcept override  // NOLINT
  {
    assert(code.domain() == *this);                                   // NOLINT
    return static_cast<const getaddrinfo_code &>(code).value() != 0;  // NOLINT
  }
  virtual bool _do_equivalent(const status_code<void> &code1, const status_code<void> &code2) const noexcept override  // NOLINT
  {
    assert(code1.domain() == *this);                                // NOLINT
    const auto &c1 = static_cast<const getaddrinfo_code &>(code1);  // NOLINT
    if(code2.domain() == *this)
    {
      const auto &c2 = static_cast<const getaddrinfo_code &>(code2);  // NOLINT
      return c1.value() == c2.value();
    }
    return false;
  }
  virtual generic_code _generic_code(const status_code<void> &code) const noexcept override  // NOLINT
  {
    assert(code.domain() == *this);                               // NOLINT
    const auto &c = static_cast<const getaddrinfo_code &>(code);  // NOLINT
    switch(c.value())
    {
#ifdef EAI_ADDRFAMILY
    case EAI_ADDRFAMILY:
      return errc::no_such_device_or_address;
#endif
    case EAI_FAIL:
      return errc::io_error;
    case EAI_MEMORY:
      return errc::not_enough_memory;
#ifdef EAI_NODATA
    case EAI_NODATA:
      return errc::no_such_device_or_address;
#endif
    case EAI_NONAME:
      return errc::no_such_device_or_address;
#ifdef EAI_OVERFLOW
    case EAI_OVERFLOW:
      return errc::argument_list_too_long;
#endif
    case EAI_BADFLAGS:  // fallthrough
    case EAI_SERVICE:
      return errc::invalid_argument;
    case EAI_FAMILY:  // fallthrough
    case EAI_SOCKTYPE:
      return errc::operation_not_supported;
    case EAI_AGAIN:  // fallthrough
    case EAI_SYSTEM:
      return errc::resource_unavailable_try_again;
    default:
      return errc::unknown;
    }
  }
  virtual string_ref _do_message(const status_code<void> &code) const noexcept override  // NOLINT
  {
    assert(code.domain() == *this);                               // NOLINT
    const auto &c = static_cast<const getaddrinfo_code &>(code);  // NOLINT
    return string_ref(gai_strerror(c.value()));
  }
#if defined(_CPPUNWIND) || defined(__EXCEPTIONS) || defined(BOOST_OUTCOME_STANDARDESE_IS_IN_THE_HOUSE)
  BOOST_OUTCOME_SYSTEM_ERROR2_NORETURN virtual void _do_throw_exception(const status_code<void> &code) const override  // NOLINT
  {
    assert(code.domain() == *this);                               // NOLINT
    const auto &c = static_cast<const getaddrinfo_code &>(code);  // NOLINT
    throw status_error<_getaddrinfo_code_domain>(c);
  }
#endif
};
//! A constexpr source variable for the `getaddrinfo()` code domain, which is that of `getaddrinfo()`. Returned by `_getaddrinfo_code_domain::get()`.
constexpr _getaddrinfo_code_domain getaddrinfo_code_domain;
inline constexpr const _getaddrinfo_code_domain &_getaddrinfo_code_domain::get()
{
  return getaddrinfo_code_domain;
}

BOOST_OUTCOME_SYSTEM_ERROR2_NAMESPACE_END

#endif
