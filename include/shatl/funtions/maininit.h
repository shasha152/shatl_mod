#pragma once

namespace tl {
namespace func {

using initializer_type = void (*)();

bool push_init_func(int prio, initializer_type func) noexcept;
void initialize() noexcept;

namespace detail {
class _register_init_func {
  public:
    explicit _register_init_func(int prio, initializer_type i) noexcept {
        push_init_func(prio, i);
    }
};
} // namespace detail

#define PRIORITY_HIGHEST 0
#define PRIORITY_HIGH 1
#define PRIORITY_MEDIUM 2
#define PRIORITY_LOW 3
#define PRIORITY_LOWEST 4

#define TL_Register_Init(prio, _func_)                                         \
    static ::tl::func::detail::_register_init_func __reg_##_func_ {            \
        prio, _func_                                                           \
    }
} // namespace func
} // namespace tl