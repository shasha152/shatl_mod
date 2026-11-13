#pragma once

#include <deque>
#include <functional>
#include <future>
namespace tl {
namespace func {
namespace detail {
inline std::vector<std::function<void()>> static_register_function;
inline std::deque<std::function<void()>> once_function;
inline std::mutex once_function_mtx;
} // namespace detail

template <typename Func>
std::future<std::invoke_result_t<Func>> register_once_function(Func &&func);
inline void invoke_once_function() noexcept;

struct __register_function {
    template <typename Func> explicit __register_function(Func &&func) {
        detail::static_register_function.emplace_back(std::forward<Func>(func));
    }
};

#define TL_Register_World_Call(fun)                                            \
    static ::tl::func::__register_function reg__##fun { fun }

template <typename Func>
inline std::future<std::invoke_result_t<Func>>
register_once_function(Func &&func) {
    using ReturnType = std::invoke_result_t<Func>;

    auto promise = std::make_shared<std::promise<ReturnType>>();
    auto future = promise->get_future();

    auto task = [func = std::forward<Func>(func), promise]() mutable {
        try {
            if constexpr (std::is_void_v<ReturnType>) {
                std::invoke(std::move(func));
                promise->set_value();
            } else {
                promise->set_value(std::invoke(std::move(func)));
            }
        } catch (...) {
            promise->set_exception(std::current_exception());
        }
    };
    {
        std::lock_guard lock(detail::once_function_mtx);
        detail::once_function.emplace_back(std::move(task));
    }

    return future;
}

inline void invoke_once_function() noexcept {
    std::function<void()> func;

    {
        std::lock_guard lock(detail::once_function_mtx);
        if (detail::once_function.empty())
            return;

        func = std::move(detail::once_function.front());
        detail::once_function.pop_front();
    }

    func();
}
} // namespace func
} // namespace tl