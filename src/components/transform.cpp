#include "transform.hpp"

namespace Components {

    transform::transform()
        : scale{1.0f, 1.0f, 1.0f}
        , position{0.0f, 0.0f, 0.0f}
        , rotation{0.0f, 0.0f, 0.0f}
    {}

    transform::transform(const math::Vec3& pos, const math::Vec3& rot, const math::Vec3& scl)
        : scale{scl}
        , position{pos}
        , rotation{rot}
    {}

    math::Vec3 transform::get_position() const { return position; }
    math::Vec3 transform::get_rotation() const { return rotation; }
    math::Vec3 transform::get_scale()    const { return scale; }

} // namespace Components
