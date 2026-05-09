//
// Created by voltas on 5/9/2026.
//

#ifndef RBXSDK_VECTOR_H
#pragma once
#include <cmath>

namespace rbx {

    struct vector2 { float x, y; };
    struct vector3 { float x, y, z; };
    struct vector4 { float x, y, z, w; };

    struct matrix4x4 {
        float m[4][4];
        float* operator[](int i) { return m[i]; }
        const float* operator[](int i) const { return m[i]; }
    };

    inline float dot(const vector3& a, const vector3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    inline vector3 cross(const vector3& a, const vector3& b) {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }

    inline vector3 operator-(const vector3& a, const vector3& b) {
        return { a.x - b.x, a.y - b.y, a.z - b.z };
    }

    inline float length(const vector3& v) {
        return std::sqrt(dot(v, v));
    }

    inline vector3 normalize(const vector3& v) {
        float l = length(v);
        return l > 0.001f ? vector3{ v.x / l, v.y / l, v.z / l } : vector3{0, 0, 0};
    }

} // rbx
#define RBXSDK_VECTOR_H

#endif //RBXSDK_VECTOR_H