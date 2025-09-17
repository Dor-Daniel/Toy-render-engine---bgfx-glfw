#pragma once
#include "../config.hpp"

namespace Components {

    class transform {
    private:
        math::Vec3 scale;
        math::Vec3 position;
        math::Vec3 rotation;
    public:
        transform();  // pos=(0,0,0), rot=(0,0,0), scale=(1,1,1)
        transform(const math::Vec3& pos, const math::Vec3& rot, const math::Vec3& scl);
        ~transform() = default;

        math::Vec3 get_position() const;
        math::Vec3 get_rotation() const;
        math::Vec3 get_scale() const;

        void set_position(math::Vec3 val);
        void set_rotation(math::Vec3 val);
        void set_scale   (math::Vec3 val);
    };

} // namespace Components
