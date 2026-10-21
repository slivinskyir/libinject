// clang-format off
#pragma once

#ifdef INJECT_AVAILABLE
	#if defined(__clang__) || defined(__GNUC__)
		#define inject__ __attribute__((weak))
	#else
		#define inject__
	#endif
#else 
	#define inject__
#endif

#ifdef INJECT_AVAILABLE
#include <functional>
#include <mutex>
#include <iostream>

namespace _inject {

template <typename T>
struct resetter
{
    resetter(T& target)
        : target_(target), reset_value_(target){}
    ~resetter(){ target_ = std::move(reset_value_); }

    T& target_;
    T reset_value_;
};

#define INJECT_DECLARE(ReturnType, Name, Params)                  \
    extern std::mutex InjectMutex_##Name;                         \
    extern std::function<ReturnType Params> InjectLamdba_##Name;

#define INJECT_DEFINE(ReturnType, Name, Params, Args)             \
    namespace _inject {                                           \
        ReturnType Name Params;                                   \
    }                                                             \
                                                                  \
    std::mutex InjectMutex_##Name;                                \
    std::function<ReturnType Params> InjectLamdba_##Name =        \
       [] Params { return _inject::Name Args; };                  \
                                                                  \
    ReturnType Name Params                                        \
    {                                                             \
        return InjectLamdba_##Name  Args;                         \
    }

#define INJECT_DEFINE_NS(Namespace, ReturnType, Name, Params, Args) \
    namespace _inject {                                             \
        namespace Namespace {                                       \
            ReturnType Name Params;                                 \
        }                                                           \
    }                                                               \
                                                                    \
    std::mutex InjectMutex_##Name;                                  \
    std::function<ReturnType Params> InjectLamdba_##Name =          \
       [] Params { return _inject::Namespace::Name Args; };         \
                                                                    \
    namespace Namespace {                                           \
        ReturnType Name Params                                      \
        {                                                           \
            return InjectLamdba_##Name  Args;                       \
        }                                                           \
    }

#define INJECT_SET_LOCAL(Name, Body)                              \
    std::lock_guard InjectMutexGuard_##Name(InjectMutex_##Name);  \
    _inject::resetter InjectResetter_##Name(InjectLamdba_##Name); \
    InjectLamdba_##Name = Body;

} // namespace _inject

#endif
// clang-format on