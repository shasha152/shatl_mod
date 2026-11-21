#pragma once

namespace tl {
namespace func {

using initializer_type = void (*)();

bool push_init_func(initializer_type func) noexcept;
void initialize() noexcept;

namespace detail {
class _register_init_func {
  public:
    _register_init_func(initializer_type i) noexcept { push_init_func(i); }
};
} // namespace detail

#define TL_Register_Init(_func_)                                                 \
    static ::tl::func::detail::_register_init_func __reg_##_func_ { _func_ }
} // namespace func
} // namespace tl