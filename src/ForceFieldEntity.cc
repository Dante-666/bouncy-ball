/** Copyright 2020 Blood Eagle Studio
 *
 * You may not use, not distribute and not modify this code
 * under any manifestable possibility and if such a scenario
 * occurs, any changes to the code must be reviewed by the
 * original author of this project.
 *
 *  Author : Siddharth J Singh(dante)
 */

#include "ForceFieldEntity.h"

#include "PhysicsScene.h"

namespace G3D {
ForceFieldEntity::ForceFieldEntity() : MarkerEntity(){};

void ForceFieldEntity::onSimulation(SimTime absoluteTime, SimTime deltaTime) {
    PhysicsScene *physicsScene = dynamic_cast<PhysicsScene *>(m_scene);
    if (physicsScene) {
	//TODO: pass the force here as well
        physicsScene->getPhysicsEngine()->applyForceField(this, m_force);
    }
}

shared_ptr<Entity> ForceFieldEntity::create(
    const String &name, Scene *scene, AnyTableReader &propertyTable,
    const ModelTable &modelTable, const Scene::LoadOptions &loadOptions) {
    // Don't initialize in the constructor, where it is unsafe to throw Any
    // parse exceptions
    shared_ptr<ForceFieldEntity> ffEntity(new ForceFieldEntity());

    // Initialize each base class, which parses its own fields
    ffEntity->Entity::init(name, scene, propertyTable);
    ffEntity->MarkerEntity::init(propertyTable);
    ffEntity->ForceFieldEntity::init(propertyTable);

    // Verify that all fields were read by the base classes
    propertyTable.verifyDone();

    return ffEntity;
}

shared_ptr<Entity> ForceFieldEntity::create(
    const String &name, Scene *scene, const Array<Box> &osBoxArray,
    const Color3 &color, const CFrame &frame, const shared_ptr<Track> &track,
    bool canChange, bool shouldBeSaved, const Vector3 force) {
    // Don't initialize in the constructor, where it is unsafe to throw Any
    // parse exceptions
    shared_ptr<ForceFieldEntity> ffEntity(new ForceFieldEntity());

    // Initialize each base class, which parses its own fields
    ffEntity->Entity::init(name, scene, frame, shared_ptr<Entity::Track>(),
                           true, true);
    ffEntity->MarkerEntity::init(osBoxArray, color);
    ffEntity->ForceFieldEntity::init(force);

    return ffEntity;
}

void ForceFieldEntity::init(AnyTableReader &propertyTable) {
    // Default expectation of this entity is to cause collisions and it is a
    // part of physical simulation so set those parameters accordingly
    propertyTable.getIfPresent("force", m_force);
}

void ForceFieldEntity::init(Vector3 force) {
    m_force = force;
}

} // namespace G3D
