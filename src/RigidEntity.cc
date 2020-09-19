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
#include "PhysicsScene.h"
#include <memory>

namespace G3D {
RigidEntity::RigidEntity() : VisibleEntity(){};

void RigidEntity::onSimulation(SimTime absoluteTime, SimTime deltaTime) {
    PhysicsScene* physicsScene = dynamic_cast<PhysicsScene*>(m_scene);
    if(physicsScene) {
	updateFrame(physicsScene->getPhysicsEngine()->getFrame(this));
    }
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
    String collisionShape = "Sphere";
    propertyTable.getIfPresent("collisionShape", collisionShape);
    m_collisionShape = "Sphere";
    if (m_collisionShape == "Sphere") {
	Sphere sphere;
	propertyTable.getIfPresent("shape", sphere);
	m_shape = createShared<SphereShape>(sphere);
    } else if (m_collisionShape == "Mesh") {
	//TODO: this needs to be handled a bit more properly
    } else {
	debugAssertM(false, "Unknown collisionShape parameter was passed");
    }
}

void RigidEntity::init(String collisionShape) {
    m_collisionShape = collisionShape;
    //Construct a default sphere for now
    Sphere sphere(1.0);
    m_shape = createShared<SphereShape>(sphere);
}

String RigidEntity::getCollisionShape() { return m_collisionShape; }

shared_ptr<Shape> RigidEntity::getShape() { return m_shape; }

} // namespace G3D
