#pragma once

#include <cstddef>
#include <tuple>
#include <utility>
namespace tl {
namespace detail {
inline constexpr int max_field_count = 16;

struct any_field {
    template <typename T> operator T() const { return T{}; }
};

template <typename T, std::size_t... I>
consteval std::size_t field_count_impl(std::index_sequence<I...>) {
    if constexpr (requires { T{((void)I, any_field{})...}; })
        return sizeof...(I);
    else if constexpr (sizeof...(I) > 0)
        return field_count_impl<T>(
            std::make_index_sequence<sizeof...(I) - 1>{});
    else
        return 0;
}

template <typename T, typename Tuple, std::size_t... I>
consteval std::size_t find_field_index(std::index_sequence<I...>) {
    constexpr bool matches[] = {
        std::is_same_v<std::remove_cvref_t<T>,
                       std::remove_cvref_t<std::tuple_element_t<I, Tuple>>>...};

    for (std::size_t i = 0; i < sizeof...(I); ++i) {
        if (matches[i])
            return i;
    }

    return static_cast<std::size_t>(-1);
}

template <std::size_t I> struct as_tuple {};

#define AS_TUPLE_SPEC(N, ...)                                                  \
    template <> struct as_tuple<N> {                                           \
        template <typename T> auto operator()(T &obj) noexcept {               \
            auto &[__VA_ARGS__] = obj;                                         \
            return std::tie(__VA_ARGS__);                                      \
        }                                                                      \
    };

AS_TUPLE_SPEC(1, a1)
AS_TUPLE_SPEC(2, a1, a2)
AS_TUPLE_SPEC(3, a1, a2, a3)
AS_TUPLE_SPEC(4, a1, a2, a3, a4)
AS_TUPLE_SPEC(5, a1, a2, a3, a4, a5)
AS_TUPLE_SPEC(6, a1, a2, a3, a4, a5, a6)
AS_TUPLE_SPEC(7, a1, a2, a3, a4, a5, a6, a7)
AS_TUPLE_SPEC(8, a1, a2, a3, a4, a5, a6, a7, a8)
AS_TUPLE_SPEC(9, a1, a2, a3, a4, a5, a6, a7, a8, a9)
AS_TUPLE_SPEC(10, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
AS_TUPLE_SPEC(11, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)
AS_TUPLE_SPEC(12, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12)
AS_TUPLE_SPEC(13, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13)
AS_TUPLE_SPEC(14, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14)
AS_TUPLE_SPEC(15, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
              a15)
AS_TUPLE_SPEC(16, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
              a15, a16)

#undef AS_TUPLE_SPEC

} // namespace detail

template <typename T> consteval std::size_t field_count() {
    return detail::field_count_impl<T>(
        std::make_index_sequence<detail::max_field_count>{});
}

// template <typename T, typename F> void for_each_field(const T &obj, F &&f) {
//     constexpr auto N = field_count<T>();
//     auto t = detail::as_tuple<N>{}(obj);
// }

template <typename T, typename Obj> decltype(auto) get(Obj &obj) noexcept {
    static_assert(std::is_aggregate_v<Obj>, "get requires aggregate type");

    constexpr auto N = field_count<Obj>();
    auto tuple = detail::as_tuple<N>{}(obj);

    using tuple_type = decltype(tuple);
    constexpr auto index =
        detail::find_field_index<T, tuple_type>(std::make_index_sequence<N>{});

    static_assert(index != static_cast<std::size_t>(-1),
                  "Field type not found");

    return std::get<index>(tuple);
}

} // namespace tl