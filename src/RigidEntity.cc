/** Copyright 2020 Blood Eagle Studio
 *
 * You may not use, not distribute and not modify this code
 * under any manifestable possibility and if such a scenario
 * occurs, any changes to the code must be reviewed by the
 * original author of this project.
 *
 *  Author : Siddharth J Singh(dante)
 */

#include "RigidEntity.h"

namespace G3D {
RigidEntity::RigidEntity() : VisibleEntity(){};

void RigidEntity::onSimulation(SimTime absoluteTime, SimTime deltaTime) {
    debugAssertM(true, "inside onSimulation");
    // Idea is to do some simulation business here
}

void RigidEntity::updateFrame(CoordinateFrame frame) {
    m_frame = frame;
    debugAssert(m_frame.rotation.isOrthonormal());

    // An object that moves farther than this in one frame is assumed to
    // have teleported and will be assigned zero velocity
    const float MAX_DISTANCE = 20.0f;

    if ((m_previousFrame.translation - m_frame.translation).squaredLength() >
        square(MAX_DISTANCE)) {
        // Teleport
        m_previousFrame = m_frame;
    }
}

shared_ptr<Entity> RigidEntity::create(const String &name, Scene *scene,
                                       AnyTableReader &propertyTable,
                                       const ModelTable &modelTable,
                                       const Scene::LoadOptions &loadOptions) {

    // Don't initialize in the constructor, where it is unsafe to throw Any
    // parse exceptions
    shared_ptr<RigidEntity> rigidEntity(new RigidEntity());

    // Initialize each base class, which parses its own fields
    rigidEntity->Entity::init(name, scene, propertyTable);
    rigidEntity->VisibleEntity::init(propertyTable, modelTable);
    rigidEntity->RigidEntity::init(propertyTable);

    // Verify that all fields were read by the base classes
    propertyTable.verifyDone();

    return rigidEntity;
}

shared_ptr<Entity> RigidEntity::create(const String &name, Scene *scene,
                                       const CFrame &position,
                                       const shared_ptr<Model> &model) {

    // Don't initialize in the constructor, where it is unsafe to throw Any
    // parse exceptions
    shared_ptr<RigidEntity> rigidEntity(new RigidEntity());

    // Initialize each base class, which parses its own fields
    rigidEntity->Entity::init(name, scene, position,
                              shared_ptr<Entity::Track>(), true, true);
    rigidEntity->VisibleEntity::init(
        model, true, Surface::ExpressiveLightScatteringProperties(),
        ArticulatedModel::PoseSpline());
    rigidEntity->RigidEntity::init();

    return rigidEntity;
}

void RigidEntity::init(AnyTableReader &propertyTable) {
    /*Vector3 v;
    propertyTable.getIfPresent("velocity", v);
    Sphere s(1.5f);
    propertyTable.getIfPresent("collisionSphere", s);*/

    init();
}

void RigidEntity::init() {
    /*
    m_velocity = velocity;
    m_collisionProxySphere = collisionProxy;
    m_desiredOSVelocity     = Vector3::zero();
    m_desiredYawVelocity    = 0;
    m_desiredPitchVelocity  = 0;
    m_heading               = 0;
    m_headTilt              = 0;*/
}
} // namespace G3D
