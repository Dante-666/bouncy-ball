/** Copyright 2020 Blood Eagle Studio
 *
 * You may not use, not distribute and not modify this code
 * under any manifestable possibility and if such a scenario
 * occurs, any changes to the code must be reviewed by the
 * original author of this project.
 *
 *  Author : Siddharth J Singh(dante)
 */

#include "GhostEntity.h"
#include "PhysicsScene.h"

namespace G3D {
GhostEntity::GhostEntity() : MarkerEntity() {};

void GhostEntity::onSimulation(SimTime absoluteTime, SimTime deltaTime) {

    // TODO: apply all behaviors here
    this->apply(this);
}

shared_ptr<Entity> GhostEntity::create(
    const String &name, Scene *scene, AnyTableReader &propertyTable,
    const ModelTable &modelTable, const Scene::LoadOptions &loadOptions) {
    // Don't initialize in the constructor, where it is unsafe to throw Any
    // parse exceptions
    shared_ptr<GhostEntity> physEntity(new GhostEntity());

    // Initialize each base class, which parses its own fields
    physEntity->Entity::init(name, scene, propertyTable);
    physEntity->MarkerEntity::init(propertyTable);
    physEntity->GhostEntity::init(propertyTable);

    // Verify that all fields were read by the base classes
    propertyTable.verifyDone();

    return physEntity;
}

// Initialize all behaviors
void GhostEntity::init(AnyTableReader &propertyTable) {
    AShape ashape;
    if (propertyTable.getIfPresent("ashape", ashape)) {
        // copy the object using new keyword as this one will be gone
        this->addProperty(new AShape(ashape));
    }
    
    // Hold a pointer to this for ez behavior modification
    m_physicsScene = dynamic_cast<PhysicsScene *>(m_scene);
    debugAssertM(m_physicsScene, "Scene is not a PhysicsScene");

    // Add default behaviors
    //this->addBehavior(new FetchPhysicsUpdate());
}

} // namespace G3D
