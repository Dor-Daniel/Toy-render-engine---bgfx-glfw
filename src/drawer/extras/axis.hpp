#pragma once
#include "../mesh.hpp"
#include "../../components/transform.hpp"

enum AxisType {
    X_AXIS, Y_AXIS, Z_AXIS,
    XY_AXIS, XZ_AXIS, YZ_AXIS,
    XYZ_AXIS
};

class axis : public Mesh::mesh
{
private:
    Components::transform axis_transform;

    // config
    AxisType axes_{ XYZ_AXIS };
    float    length_{ 1.0f };                 // half-extent; axis goes [-L, +L]
    uint32_t col_x_{ 0xff0000ff };            // ABGR: X=red
    uint32_t col_y_{ 0xff00ff00 };            // Y=green
    uint32_t col_z_{ 0xffff0000 };            // Z=blue

    // floor grid
    bool     floor_enabled_{ false };
    AxisType floor_plane_{ XY_AXIS };         // XY / XZ / YZ
    uint32_t floor_lines_count_{ 10 };        // lines in each direction (n)
    uint32_t floor_color_{ 0xff7f7f7f };      // gray

    // fog placeholder (not used in geometry yet)
    bool     fog_enabled_{ false };
    float    fog_density_{ 0.02f };

    void update_mesh();                       // rebuild vertices + line indices

    // helpers
    static inline bool has_x(AxisType t) { return t==X_AXIS || t==XY_AXIS || t==XZ_AXIS || t==XYZ_AXIS; }
    static inline bool has_y(AxisType t) { return t==Y_AXIS || t==XY_AXIS || t==YZ_AXIS || t==XYZ_AXIS; }
    static inline bool has_z(AxisType t) { return t==Z_AXIS || t==XZ_AXIS || t==YZ_AXIS || t==XYZ_AXIS; }

public:
    explicit axis(AxisType type = XYZ_AXIS) : axes_{type} { update_mesh(); }
    ~axis() = default;

    // transform
    void set_transform(const Components::transform& _transform) { axis_transform = _transform; update_mesh(); }

    // length (half-extent)
    void set_length(float L) { length_ = (L < 0.0f ? -L : L); update_mesh(); }

    // per-axis colors
    void set_x_color(uint32_t abgr) { col_x_ = abgr; update_mesh(); }
    void set_y_color(uint32_t abgr) { col_y_ = abgr; update_mesh(); }
    void set_z_color(uint32_t abgr) { col_z_ = abgr; update_mesh(); }
    void set_axes_colors(uint32_t x, uint32_t y, uint32_t z) { col_x_ = x; col_y_ = y; col_z_ = z; update_mesh(); }

    // which axes to draw
    void set_axes(AxisType t) { axes_ = t; update_mesh(); }

    // floor grid
    void enable_floor(bool on) { floor_enabled_ = on; update_mesh(); }
    void set_floor_plane(AxisType plane) { floor_plane_ = plane; update_mesh(); } // use XY_AXIS/XZ_AXIS/YZ_AXIS
    void set_floor_lines_count(uint32_t n) { floor_lines_count_ = (n == 0 ? 1u : n); update_mesh(); }
    void set_floor_color(uint32_t abgr) { floor_color_ = abgr; update_mesh(); }

    // fog placeholder
    void set_fog_enabled(bool on) { fog_enabled_ = on; }
    void set_fog_density(float d) { fog_density_ = d; }

    // convenience: create with line topology
    void create_renderer_lines(const std::string& vs, const std::string& fs, uint64_t state = Shader::default_state) {
        mesh::create_renderer(vs, fs, state | BGFX_STATE_PT_LINES);
    }
};
