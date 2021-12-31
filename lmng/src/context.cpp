#include <lmng/archetype.h>
#include <lmng/camera.h>
#include <lmng/hierarchy.h>
#include <lmng/meta/reflect_component.h>
#include <lmng/physics.h>
#include <lmng/shapes.h>
#include <lmng/transform.h>

namespace lmng
{
void set_meta_context(entt::meta_ctx const &ctx) { entt::meta_ctx::bind(ctx); }

void reflect_types()
{
    REFLECT_TYPE(box_render, "Box Render")
      .REFLECT_MEMBER(box_render, extents, "Extents")
      .REFLECT_MEMBER(box_render, colour, "Colour");

    REFLECT_TYPE(transform, "Transform")
      .REFLECT_MEMBER(transform, position, "Position")
      .REFLECT_MEMBER(transform, rotation, "Rotation");

    REFLECT_TYPE(parent, "Transform Parent")
      .REFLECT_MEMBER(parent, entity, "Entity");

    REFLECT_TYPE(rigid_body, "Rigid Body")
      .REFLECT_MEMBER(rigid_body, mass, "Mass")
      .REFLECT_MEMBER(rigid_body, restitution, "Restitution")
      .REFLECT_MEMBER(rigid_body, friction, "Friction");

    REFLECT_TYPE(character_controller, "Character Controller");

    REFLECT_TYPE(box_collider, "Box Collider")
      .REFLECT_MEMBER(box_collider, extents, "Extents");

    REFLECT_TYPE(camera, "Camera")
      .REFLECT_MEMBER(camera, field_of_view, "Field of view")
      .REFLECT_MEMBER(camera, near_clip, "Near clip")
      .REFLECT_MEMBER(camera, far_clip, "Far clip")
      .REFLECT_MEMBER(camera, active, "Active");

    REFLECT_TYPE(archetype, "Archetype")
      .REFLECT_MEMBER(archetype, asset_path, "Asset path");

    REFLECT_TYPE(name, "Name");

    REFLECT_TYPE(rigid_constraint, "Rigid Constraint")
      .REFLECT_MEMBER(rigid_constraint, first, "First")
      .REFLECT_MEMBER(rigid_constraint, second, "Second")
      .REFLECT_MEMBER(rigid_constraint, break_impulse, "Break impulse");
}
} // namespace lmng
