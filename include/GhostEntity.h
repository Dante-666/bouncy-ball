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

/** \brief Derived from a MarkerEntity
 * Designed to behave like a RigidBody based on inputs from a Physics Engine and
 * interact with other rigid bodies and soft materials which can work with that
 * engine.
 */

#include "G3D-app/ArticulatedModel.h"
#include "G3D-app/G3D-app.h"
#include "G3D-base/G3D-base.h"
#include "G3D-gfx/G3D-gfx.h"

#include "behavior/BChain.h"
#include "behavior/PChain.h"

#include "behavior/AShape.h"

#include <memory>

namespace G3D {

class PhysicsScene;

class GhostEntity : public MarkerEntity,
                    private BehaviorChain<GhostEntity>,
                    public PropertyChain {
    PhysicsScene *m_physicsScene;

public:
    // add friend classes here which implement behavior

    GhostEntity();

    /** update the pose of object here by querying the physics engine */
    virtual void onSimulation(SimTime absoluteTime, SimTime deltaTime) override;

    /** For deserialization from Any / loading from file */
    static shared_ptr<Entity> create(const String &name, Scene *scene,
                                     AnyTableReader &propertyTable,
                                     const ModelTable &modelTable,
                                     const Scene::LoadOptions &loadOptions);

    /** For programmatic construction at runtime */
    static shared_ptr<Entity> create(const String &name, Scene *scene,
                                     const Array<Box> &osBoxArray,
                                     const Color3 &color, const CFrame &frame,
                                     const shared_ptr<Track> &track,
                                     bool canChange, bool shouldBeSaved,
                                     const Vector3 force);

    void init(AnyTableReader &propertyTable);

    // TODO: add more default shapes and sizes
    void init(Vector3 force);

    // const Shape::Type getShapeTypeFromString(const String shape);
};
} // namespace G3D
