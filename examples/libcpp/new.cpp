//===--------------------------- new.cpp ----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#if 0 //PJ: Need to remove
#include <stdlib.h>
#else
//PJ: from libc
#include <memory.h>
#endif

#include "new"

static std::new_handler __new_handler;

// Implement all new and delete operators as weak definitions
// in this shared library, so that they can be overriden by programs
// that define non-weak copies of the functions.

__attribute__((__weak__, __visibility__("default")))
void *
operator new(std::size_t size)
#if !__has_feature(cxx_noexcept)
    throw(std::bad_alloc)
#endif
{
    if (size == 0)
        size = 1;
    void* p;
    while ((p = ::malloc(size)) == 0)
    {
        // If malloc fails and there is a new_handler,
        // call it to try free up memory.
        std::new_handler nh = std::get_new_handler();
        if (nh)
            nh();
        else
#ifndef _LIBCPP_NO_EXCEPTIONS
            throw std::bad_alloc();
#else
            break;
#endif
    }
    return p;
}

__attribute__((__weak__, __visibility__("default")))
void*
operator new(size_t size, const std::nothrow_t&) _NOEXCEPT
{
    void* p = 0;
#ifndef _LIBCPP_NO_EXCEPTIONS
    try
    {
#endif  // _LIBCPP_NO_EXCEPTIONS
        p = ::operator new(size);
#ifndef _LIBCPP_NO_EXCEPTIONS
    }
    catch (...)
    {
    }
#endif  // _LIBCPP_NO_EXCEPTIONS
    return p;
}

__attribute__((__weak__, __visibility__("default")))
void*
operator new[](size_t size)
#if !__has_feature(cxx_noexcept)
    throw(std::bad_alloc)
#endif
{
    return ::operator new(size);
}

__attribute__((__weak__, __visibility__("default")))
void*
operator new[](size_t size, const std::nothrow_t& nothrow) _NOEXCEPT
{
    void* p = 0;
#ifndef _LIBCPP_NO_EXCEPTIONS
    try
    {
#endif  // _LIBCPP_NO_EXCEPTIONS
        p = ::operator new[](size);
#ifndef _LIBCPP_NO_EXCEPTIONS
    }
    catch (...)
    {
    }
#endif  // _LIBCPP_NO_EXCEPTIONS
    return p;
}

__attribute__((__weak__, __visibility__("default")))
void
operator delete(void* ptr) _NOEXCEPT
{
    if (ptr)
        ::free(ptr);
}

__attribute__((__weak__, __visibility__("default")))
void
operator delete(void* ptr, const std::nothrow_t&) _NOEXCEPT
{
    ::operator delete(ptr);
}

__attribute__((__weak__, __visibility__("default")))
void
operator delete[] (void* ptr) _NOEXCEPT
{
    ::operator delete (ptr);
}

__attribute__((__weak__, __visibility__("default")))
void
operator delete[] (void* ptr, const std::nothrow_t&) _NOEXCEPT
{
    ::operator delete[](ptr);
}

namespace std
{

const nothrow_t nothrow = {};

#if 0 //TODO
new_handler
set_new_handler(new_handler handler) _NOEXCEPT
{
    return __sync_lock_test_and_set(&__new_handler, handler);
}

new_handler
get_new_handler() _NOEXCEPT
{
    return __sync_fetch_and_add(&__new_handler, (new_handler)0);
}
#endif

bad_alloc::bad_alloc() _NOEXCEPT
{
}

bad_alloc::~bad_alloc() _NOEXCEPT
{
}

const char*
bad_alloc::what() const _NOEXCEPT
{
    return "std::bad_alloc";
}

bad_array_new_length::bad_array_new_length() _NOEXCEPT
{
}

bad_array_new_length::~bad_array_new_length() _NOEXCEPT
{
}

const char*
bad_array_new_length::what() const _NOEXCEPT
{
    return "bad_array_new_length";
}

void
__throw_bad_alloc()
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    throw bad_alloc();
#endif
}

}  // std