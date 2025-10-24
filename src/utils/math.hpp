#include <cmath>
#include <limits>
#include <cmath>

namespace math{
    struct Vec2 {
        float x{0.0f}, y{0.0f};

        constexpr Vec2() noexcept = default;
        constexpr Vec2(float _x, float _y) noexcept : x{_x}, y{_y} {}
        constexpr Vec2(float val) noexcept : x{val}, y{val}{}

        // element-wise 
        constexpr Vec2 operator+(const Vec2& o) const noexcept { return {x + o.x, y + o.y}; }
        constexpr Vec2 operator-(const Vec2& o) const noexcept { return {x - o.x, y - o.y}; }
        constexpr Vec2 operator*(const Vec2& o) const noexcept { return {x * o.x, y * o.y}; }
        constexpr Vec2 operator/(const Vec2& o) const noexcept { return {x / o.x, y / o.y}; }

        constexpr Vec2& operator+=(const Vec2& o) noexcept { x += o.x; y += o.y; return *this; }
        constexpr Vec2& operator-=(const Vec2& o) noexcept { x -= o.x; y -= o.y; return *this; }
        constexpr Vec2& operator*=(const Vec2& o) noexcept { x *= o.x; y *= o.y; return *this; }
        constexpr Vec2& operator/=(const Vec2& o) noexcept { x /= o.x; y /= o.y; return *this; }

        // scalar versions (like GLSL)
        constexpr Vec2 operator*(float s) const noexcept { return {x * s, y * s}; }
        constexpr Vec2 operator/(float s) const noexcept { return {x / s, y / s}; }
        constexpr Vec2& operator*=(float s) noexcept { x *= s; y *= s; return *this; }
        constexpr Vec2& operator/=(float s) noexcept { x /= s; y /= s; return *this; }

        // allow scalar on the left (GLSL style)
        friend constexpr Vec2 operator*(float s, const Vec2& v) noexcept { return {v.x * s, v.y * s}; }
        friend constexpr Vec2 operator/(float s, const Vec2& v) noexcept { return {s / v.x, s / v.y}; }

        // comparisons
        constexpr bool operator==(const Vec2& o) const noexcept { return x == o.x && y == o.y; }

        static const Vec2 unit; 
        static const Vec2 zero;
        static const Vec2 e1;
        static const Vec2 e2;
    };

    struct Vec3 {
        float x{0.0f}, y{0.0f}, z{0.0f};

        constexpr Vec3() noexcept = default;
        constexpr Vec3(float _x, float _y, float _z) noexcept : x{_x}, y{_y}, z{_z} {}
        constexpr Vec3(float val) noexcept : x{val}, y{val}, z{val}{}

        // element-wise (Hadamard)
        constexpr Vec3 operator+(const Vec3& o) const noexcept { return {x + o.x, y + o.y, z + o.z}; }
        constexpr Vec3 operator-(const Vec3& o) const noexcept { return {x - o.x, y - o.y, z - o.z}; }
        constexpr Vec3 operator*(const Vec3& o) const noexcept { return {x * o.x, y * o.y, z * o.z}; }
        constexpr Vec3 operator/(const Vec3& o) const noexcept { return {x / o.x, y / o.y, z / o.z}; }

        constexpr Vec3& operator+=(const Vec3& o) noexcept { x += o.x; y += o.y; z += o.z; return *this; }
        constexpr Vec3& operator-=(const Vec3& o) noexcept { x -= o.x; y -= o.y; z -= o.z; return *this; }
        constexpr Vec3& operator*=(const Vec3& o) noexcept { x *= o.x; y *= o.y; z *= o.z; return *this; }
        constexpr Vec3& operator/=(const Vec3& o) noexcept { x /= o.x; y /= o.y; z /= o.z; return *this; }

        // scalar versions
        constexpr Vec3 operator*(float s) const noexcept { return {x * s, y * s, z * s}; }
        constexpr Vec3 operator/(float s) const noexcept { return {x / s, y / s, z / s}; }
        constexpr Vec3& operator*=(float s) noexcept { x *= s; y *= s; z *= s; return *this; }
        constexpr Vec3& operator/=(float s) noexcept { x /= s; y /= s; z /= s; return *this; }

        // scalar on the left (GLSL style)
        friend constexpr Vec3 operator*(float s, const Vec3& v) noexcept { return {v.x * s, v.y * s, v.z * s}; }
        friend constexpr Vec3 operator/(float s, const Vec3& v) noexcept { return {s / v.x, s / v.y, s / v.z}; }

        constexpr bool operator==(const Vec3& o) const noexcept { return x == o.x && y == o.y && z == o.z; }
        static const Vec3 unit;
        static const Vec3 zero;
        static const Vec3 e1;
        static const Vec3 e2;
        static const Vec3 e3;
    };
    
    
    
    inline const Vec2 Vec2::unit{1}; 
    inline const Vec2 Vec2::zero{0};
    inline const Vec2 Vec2::e1{1, 0};
    inline const Vec2 Vec2::e2{0, 1};

    inline const Vec3 Vec3::unit{1}; 
    inline const Vec3 Vec3::zero{0};
    inline const Vec3 Vec3::e1{1, 0, 0};
    inline const Vec3 Vec3::e2{0, 1, 0};
    inline const Vec3 Vec3::e3{0, 0, 1};
    
    constexpr float dot(const Vec2 &v, const Vec2 &u){ Vec2 o = v * u; return o.x + o.y; }
    constexpr float dot(const Vec3 &v, const Vec3 &u){ Vec3 o = v * u; return o.x + o.y + o.z; }
    constexpr float length(const Vec2 &v){ return std::sqrt(dot(v, v)); }
    constexpr float length(const Vec3 &v){ return std::sqrt(dot(v, v)); }
    constexpr float length2(const Vec2 &v){ return dot(v, v); }
    constexpr float length2(const Vec3 &v){ return dot(v, v); }
    constexpr Vec2  normalize(Vec2 v){ return v == Vec2::zero? v : v / length(v); }
    constexpr Vec3  normalize(Vec3 v){ return v == Vec3::zero? v : v / length(v); }
    constexpr Vec3  cross(Vec3 v, Vec3 u){ 
            return  (v.y * u.z - v.z * u.y) * Vec3::e1 +
                    (v.z * u.x - v.x * u.z) * Vec3::e2 +
                    (v.x * u.y - u.x * v.y) * Vec3::e3 ; 
            }

    // struct Ray{
    //     Vec3 begin;
    //     Vec3 direction; 

    //     Ray(Vec3 _begin, Vec3 _direction) : begin{_begin}, direction{_direction}{}

    // };

    // struct Plane{
    //     Vec3 point;
    //     Vec3 normal;

    //     Plane(Vec3 _point, Vec3 _normal) : point{_point}, normal{_normal}{}
    // };

    // struct RayHitInfo{
    //     Vec3 pointOfImpact;
    //     float rayDistance;
    //     float angleOfImpact;
    //     bool hit;
    // };

    // RayHitInfo GetRayHitPlane(Ray ray, Plane plane){
    //     RayHitInfo hitInfo = {};
    //     float rayDotN = dot(ray.direction, plane.normal);
    //     float angleOfImpact = acosf(rayDotN / (length(ray.direction) * length(plane.normal)));
    //     if(angleOfImpact > bx::kPiHalf || angleOfImpact < )

    //     return hitInfo;
    // }


}