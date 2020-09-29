/** Copyright 2020 Blood Eagle Studio
 *
 * You may not use, not distribute and not modify this code 
 * under any manifestable possibility and if such a scenario
 * occurs, any changes to the code must be reviewed by the 
 * original author of this project.
 *
 *  Author : Siddharth J Singh(dante)
 */

#pragma once

/** \brief Derived from a VisibleEntity
 * Designed to behave like a RigidBody based on inputs from a Physics Engine and
 * interact with other rigid bodies and soft materials which can work with that
 * engine.
 */
#include <G3D-base/G3D-base.h>
#include <G3D-gfx/G3D-gfx.h>
#include <G3D-app/G3D-app.h>

namespace G3D {
class RigidEntity : public VisibleEntity {
public:
    Shape::Type m_collisionShape;
    shared_ptr<Shape> m_shape;
    float m_rollingFriction = 0.f;

    RigidEntity();

    /** update the pose of object here by querying the physics engine */
    virtual void onSimulation(SimTime absoluteTime, SimTime deltaTime) override;

    /** For deserialization from Any / loading from file */
    static shared_ptr<Entity> create(const String &name, Scene *scene,
                                     AnyTableReader &propertyTable,
                                     const ModelTable &modelTable,
                                     const Scene::LoadOptions &loadOptions);

    /** For programmatic construction at runtime */
    static shared_ptr<Entity> create(const String &name, Scene *scene,
                                     const CFrame &position,
                                     const shared_ptr<Model> &model);

    void init(AnyTableReader &propertyTable);

    //TODO: add more default shapes and sizes
    void init(Shape::Type collisionShape = Shape::Type::SPHERE);

    const Shape::Type getShapeTypeFromString(const String shape);
};
} // namespace G3D
