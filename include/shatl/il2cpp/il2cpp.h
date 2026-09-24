#pragma once

#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>

#include "shatl/utils/log.h"

namespace tl {
class il2cpp {
    inline static void *domain = nullptr;
    inline static void *handle = nullptr;

    static void *get_handle_sym(std::string_view str) noexcept;

  public:
    static bool init() noexcept;

    template <typename R, typename... Args>
    static R il2cpp_invoke(std::string_view fn, Args... args) {
        static std::unordered_map<std::string, void *> functions;

        auto it = functions.find(std::string(fn));
        if (it == functions.end())
            it = functions.emplace(std::string(fn), get_handle_sym(fn)).first;

        return static_invoke<R>(it->second, args...);
    }

    template <typename R, typename... Args>
    static R static_invoke(void *fn, Args... args) {
        if (fn == nullptr) {
            LOGI("il2cpp invoke called with nullptr");
            if constexpr (std::is_same<R, void>::value)
                return;
            else
                return R{};
        }
        return reinterpret_cast<R (*)(Args...)>(fn)(args...);
    }

    class assembly;
    class _class;

    class field {
      public:
        static field *create(const _class *cls, std::string_view name) {
            void *iter = nullptr;
            while (field *m = il2cpp_invoke<field *>("il2cpp_class_get_fields",
                                                     cls, &iter)) {
                if (name == m->name())
                    return m;
            }

            return nullptr;
        }

        std::string_view name() const noexcept {
            const char *str =
                il2cpp_invoke<const char *>("il2cpp_field_get_name", this);
            return str ? str : "";
        }

        size_t offset() const noexcept {
            return il2cpp_invoke<size_t>("il2cpp_field_get_offset", this);
        }

        template <typename T> T value(const void *obj) const noexcept {
            if (!obj) [[unlikely]]
                return T{};
            return *reinterpret_cast<T *>(
                reinterpret_cast<std::uintptr_t>(obj) + offset());
        }

        template <typename T> void value(void *obj, const T &v) noexcept {
            if (!obj) [[unlikely]] {
                LOGI("obj=nullptr");
                return;
            }
            *reinterpret_cast<T *>(reinterpret_cast<std::uintptr_t>(obj) +
                                   offset()) = v;
        }

        template <typename T> T static_value() const noexcept {
            T value{};
            il2cpp_invoke<void>("il2cpp_field_static_get_value", this, &value);
            return value;
        }

        template <typename T> void static_value(const T &value) const noexcept {
            il2cpp_invoke<void>("il2cpp_field_static_set_value", this, &value);
        }
    };

    class method {
      public:
        static method *create(const _class *cls, std::string_view name,
                              int argc = -1) noexcept {
            void *iter = nullptr;
            while (method *m = il2cpp_invoke<method *>(
                       "il2cpp_class_get_methods", cls, &iter)) {
                if (name == m->name()) {
                    if (argc == -1 || argc == m->param_count())
                        return m;
                }
            }
            return nullptr;
        }

        static method *create(const _class *cls, std::string_view name,
                              std::initializer_list<std::string_view> ps) {
            void *iter = nullptr;

            while (method *m = il2cpp_invoke<method *>(
                       "il2cpp_class_get_methods", cls, &iter)) {
                int count =
                    il2cpp_invoke<int>("il2cpp_method_get_param_count", m);

                if (name != m->name() || ps.size() != count)
                    continue;

                bool match = true;
                int index = 0;

                for (auto p : ps) {
                    const char *real = il2cpp_invoke<const char *>(
                        "il2cpp_type_get_name",
                        il2cpp_invoke<void *>("il2cpp_method_get_param", m,
                                              index));

                    if (!(p == "*" || p == real)) {
                        match = false;
                        break;
                    }
                    index++;
                }

                if (match)
                    return m;
            }

            return nullptr;
        }

        std::string_view name() const noexcept {
            const char *str =
                il2cpp_invoke<const char *>("il2cpp_method_get_name", this);
            return str ? str : "";
        }

        void *get() noexcept { return *reinterpret_cast<void **>(this); }

        template <typename R, typename... Args> R invoke(Args... args) {
            return static_invoke<R>(get(), args..., this);
        }

        template <typename R, typename... Args>
        R runtime_invoke(void *obj, Args... args) {
            void *params[sizeof...(Args)] = {
                reinterpret_cast<void *>(&args)...};
            void *exc = nullptr;

            void *ret =
                il2cpp_invoke<void *>("il2cpp_runtime_invoke", this, obj,
                                      sizeof...(Args) ? params : nullptr, &exc);
            LOGI("il2cpp_runtime_invoke call = %p", ret);
            if (exc) {
                LOGI("il2cpp exception!");
                if constexpr (!std::is_same_v<R, void>)
                    return R{};
                else
                    return;
            }

            if constexpr (std::is_same_v<R, void>)
                return;
            else
                return *static_cast<R *>(ret);
        }

        int param_count() const noexcept {
            return il2cpp_invoke<int>("il2cpp_method_get_param_count", this);
        }
    };

    class _class {
      public:
        static _class *create(const assembly *assembly,
                              std::string_view _namespace,
                              std::string_view name) noexcept {
            void *image = assembly->image();
            return il2cpp_invoke<_class *>("il2cpp_class_from_name", image,
                                           _namespace.data(), name.data());
        }

        std::string_view name() const noexcept {
            const char *str =
                il2cpp_invoke<const char *>("il2cpp_class_get_name", this);
            return str ? str : "";
        }

        _class *parent() const noexcept {
            return il2cpp_invoke<_class *>("il2cpp_class_get_parent", this);
        }

        method *mmethod(std::string_view name, int argc = -1) const noexcept {
            auto m = method::create(this, name, argc);
            if (m == nullptr) {
                LOGI("il2cpp_class_get_methods failed: name = %s", name.data());
            }
            return m;
        }

        method *
        mmethod(std::string_view name,
                std::initializer_list<std::string_view> ps) const noexcept {
            auto m = method::create(this, name, ps);
            if (m == nullptr) {
                LOGI("il2cpp_class_get_methods failed: name = %s", name.data());
            }
            return m;
        }

        field *mfield(std::string_view name) const noexcept {
            auto f = field::create(this, name);
            if (f == nullptr) {
                LOGI("il2cpp_class_get_fields failed: name = %s", name.data());
            }
            return f;
        }

        template <typename... Args>
        void *object_new(Args... args) const noexcept {
            void *obj = il2cpp_invoke<void *>("il2cpp_object_new", this);
            if (!obj) {
                LOGI("il2cpp_object_new failed");
                return nullptr;
            }
            auto m = mmethod(".ctor", sizeof...(Args));
            if (m == nullptr) {
                LOGI("call .ctor failed");
                return obj;
            }
            m->invoke<void>(obj, args...);
            return obj;
        }
    };

    class assembly {
      public:
        static assembly *create(std::string_view name) noexcept {
            size_t size = 0;
            auto assemblies = il2cpp_invoke<assembly **>(
                "il2cpp_domain_get_assemblies", domain, &size);

            for (auto i = 0; i < size; i++) {
                auto _assembly = assemblies[i];
                if (_assembly == nullptr)
                    continue;
                if (_assembly->name() == name)
                    return _assembly;
            }

            return nullptr;
        }

        std::string_view name() const noexcept {
            const char *str =
                il2cpp_invoke<const char *>("il2cpp_image_get_name", image());
            return str ? str : "";
        }

        void *image() const noexcept {
            return il2cpp_invoke<void *>("il2cpp_assembly_get_image", this);
        }

        _class *mclass(std::string_view _namespace,
                       std::string_view name) const noexcept {
            auto c = _class::create(this, _namespace, name);
            if (c == nullptr) {
                LOGI("il2cpp_class_from_name failed: name = %s", name.data());
                return nullptr;
            }
            return c;
        }
    };
    template <std::size_t I> struct tag {};
    template <typename Tag = tag<0>> struct object {
        _class *klass;
        void *monitor;

        template <typename T> T get(std::string_view name) const {
            auto f = klass->mfield(name);
            if (f)
                return f->value<T>(this);
            LOGW("not found %s", name.data());
            return {};
        }

        template <typename T> void set(std::string_view name, const T &value) {
            auto f = klass->mfield(name);
            if (f)
                return f->value<T>(this, value);
            LOGW("not found %s", name.data());
        }

        template <typename T> T static_get(std::string_view name) const {
            return klass->mfield(name)->static_value<T>();
        }

        template <typename T>
        void static_set(std::string_view name, const T &value) {
            klass->mfield(name)->static_value<T>(value);
        }

        template <typename Ret, typename... Args>
        Ret call(std::string_view name, Args... args) {
            if (!methods.contains(name))
                methods.emplace(name, klass->mmethod(name, sizeof...(args)));

            return methods[name]->invoke<Ret>(this, args...);
        }

        template <typename Ret, typename... Args>
        Ret static_call(std::string_view name, Args... args) {
            return get_method(name, sizeof...(args))
                ->template invoke<Ret>(args...);
        }

        template <typename Ret, typename... Args>
        inline static Ret static_call2(std::string_view dll,
                                       std::string_view _namespace,
                                       std::string_view cls,
                                       std::string_view name, Args... args) {
            if (!methods.contains(name))
                methods.emplace(
                    name, _class::create(assembly::create(dll), _namespace, cls)
                              ->mmethod(name, sizeof...(args)));

            return methods[name]->invoke<Ret>(args...);
        }

        method *get_method(std::string_view name, int args_num) noexcept {
            if (!methods.contains(name))
                methods.emplace(name, klass->mmethod(name, args_num));

            return methods[name];
        }

      private:
        inline static std::unordered_map<std::string_view, method *> methods{};
    };

    template <typename T> class array : public object<> {
        void *bounds;
        size_t length;
        T data[0];

      public:
        using value_type = T;
        using iterator = T *;
        using const_iterator = const T *;

        using reference = T &;
        using const_reference = const T &;

        size_t size() const noexcept { return length; }

        iterator begin() noexcept { return data; }
        iterator end() noexcept { return data + length; }

        const_iterator begin() const noexcept { return data; }
        const_iterator end() const noexcept { return data + length; }

        reference operator[](size_t i) noexcept { return data[i]; }
        const_reference operator[](size_t i) const noexcept { return data[i]; }
    };

    template <typename T> struct any_vector2 {
        T x;
        T y;

        // any_vector2() = default;

        explicit any_vector2(const T &_x = T{}, const T &_y = T{}) noexcept
            : x(_x), y(_y) {}

        template <typename OtherType>
        explicit operator any_vector2<OtherType>() const noexcept {
            return any_vector2<OtherType>{static_cast<OtherType>(this->x),
                                          static_cast<OtherType>(this->y)};
        }

        any_vector2 operator/(const T &r) const noexcept {
            return any_vector2{this->x / r, this->y / r};
        }

        any_vector2 operator*(const T &r) const noexcept {
            return any_vector2{this->x * r, this->y * r};
        }

        any_vector2 operator/(const any_vector2 &r) const noexcept {
            return any_vector2{this->x / r.x, this->y / r.y};
        }
        any_vector2 operator+(const any_vector2 &r) const noexcept {
            return any_vector2{this->x + r.x, this->y + r.y};
        }

        T distance(const any_vector2 &r) const noexcept {
            T _x = this->x - r.x;
            T _y = this->y - r.y;
            return static_cast<T>(std::sqrt(_x * _x + _y * _y));
        }
#ifdef IMGUI_VERSION
        operator ImVec2() const noexcept { return ImVec2{this->x, this->y}; }
#endif
    };

    using vector2 = any_vector2<float>;

    struct vector3 {
        float x;
        float y;
        float z;
    };

    struct rectangle {
        int x;      // 0x10
        int y;      // 0x14
        int width;  // 0x18
        int height; // 0x1c

        rectangle() = default;

        explicit rectangle(int _x, int _y, int _width, int _height) noexcept
            : x(_x), y(_y), width(_width), height(_height) {}
        explicit rectangle(const vector2 &xy, const vector2 &wh) noexcept
            : x(xy.x), y(xy.y), width(wh.x), height(wh.y) {}

        vector2 leftTop() const noexcept { return vector2(x, y); }

        vector2 rightBottem() const noexcept { return vector2(width, height); }
    };

    struct color {
        uint8_t r, g, b, a;
        color() = default;
        color(uint8_t i, uint8_t i1, uint8_t i2, uint8_t i3)
            : r{i}, g{i1}, b{i2}, a{i3} {}
    };

    struct point {
        int x, y;
    };

    struct string : object<> {
        int32_t length;
        char16_t wchar[0];

        std::string to_string() const noexcept {
            return utf16_to_utf8(wchar, length);
        }

        static string *create(const char *name) noexcept {
            return il2cpp_invoke<string *>("il2cpp_string_new", name);
        }

      private:
        static std::string utf16_to_utf8(const char16_t *w, int len) {
            std::string out;
            out.reserve(len);

            for (int i = 0; i < len; i++) {
                uint32_t c = w[i];

                if (c <= 0x7F)
                    out.push_back((char)c);
                else if (c <= 0x7FF) {
                    out.push_back(0xC0 | (c >> 6));
                    out.push_back(0x80 | (c & 0x3F));
                } else {
                    out.push_back(0xE0 | (c >> 12));
                    out.push_back(0x80 | ((c >> 6) & 0x3F));
                    out.push_back(0x80 | (c & 0x3F));
                }
            }
            return out;
        }
    };

    static void *resolve_icall(std::string_view name) noexcept {
        return il2cpp_invoke<void *>("il2cpp_resolve_icall", name.data());
    }
};
} // namespace tl