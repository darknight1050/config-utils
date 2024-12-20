#pragma once

namespace UnityEngine {
    struct Color {
        float r, g, b, a;
        Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
    };
    struct Vector2 {
        float x, y;
        Vector2(float x, float y) : x(x), y(y) {}
    };
    struct Vector3 {
        float x, y, z;
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    };
    struct Vector4 {
        float x, y, z, w;
        Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    };
}

namespace ConfigUtils {

#define CONVERSION(clazz, construct, convert) \
    clazz(const UnityEngine::clazz& other) { construct; } \
    clazz& operator=(const UnityEngine::clazz& other) { construct; return *this; } \
    operator UnityEngine::clazz() const { return UnityEngine::clazz convert; }

    DECLARE_JSON_STRUCT(Vector2) {
        VALUE(float, x);
        VALUE(float, y);
        CONVERSION(Vector2, x = other.x; y = other.y;, (x, y));
        Vector2() = default;
        Vector2(float x, float y) : x(x), y(y) {}
    };

    DECLARE_JSON_STRUCT(Vector3) {
        VALUE(float, x);
        VALUE(float, y);
        VALUE(float, z);
        CONVERSION(Vector3, x = other.x; y = other.y; z = other.z;, (x, y, z));
        Vector3() = default;
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    };

    DECLARE_JSON_STRUCT(Vector4) {
        VALUE(float, x);
        VALUE(float, y);
        VALUE(float, z);
        VALUE(float, w);
        CONVERSION(Vector4, x = other.x; y = other.y; z = other.z; w = other.w;, (x, y, z, w));
        Vector4() = default;
        Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    };

    DECLARE_JSON_STRUCT(Color) {
        VALUE(float, r);
        VALUE(float, g);
        VALUE(float, b);
        VALUE(float, a);
        CONVERSION(Color, r = other.r; g = other.g; b = other.b; a = other.a;, (r, g, b, a));
        Color() = default;
        Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
    };

#define SPECIALIZATION(type) \
    template <> \
    struct Specialization<UnityEngine::type> { \
        using JSONType = type; \
    }; \
    template <> \
    struct Specialization<std::vector<UnityEngine::type>> { \
        using JSONType = std::vector<type>; \
    };

    SPECIALIZATION(Vector2)
    SPECIALIZATION(Vector3)
    SPECIALIZATION(Vector4)
    SPECIALIZATION(Color)

#undef CONVERSION
#undef SPECIALIZATION

}
