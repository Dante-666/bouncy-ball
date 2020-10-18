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
GhostEntity::GhostEntity() : MarkerEntity(){};

void GhostEntity::onSimulation(SimTime absoluteTime, SimTime deltaTime) {

    // TODO: apply all behaviors here
    this->apply(this);
    /*void ForceFieldEntity::onSimulation(SimTime absoluteTime, SimTime
    deltaTime) { PhysicsScene *physicsScene = dynamic_cast<PhysicsScene
    *>(m_scene); if (physicsScene) {
    //TODO: pass the force here as well
        physicsScene->getPhysicsEngine()->applyForceField(this, m_force);
    }*/
}

shared_ptr<Entity> GhostEntity::create(const String &name, Scene *scene,
                                       AnyTableReader &propertyTable,
                                       const ModelTable &modelTable,
                                       const Scene::LoadOptions &loadOptions) {
    // Don't initialize in the constructor, where it is unsafe to throw Any
    // parse exceptions
    shared_ptr<GhostEntity> physEntity(new GhostEntity());

    // Initialize each base class, which parses its own fields
    physEntity->Entity::init(name, scene, propertyTable);
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

    Array<Box> boxArray;
    Color3 color = Color3::white();
    propertyTable.getIfPresent("color", color);

    // TODO: remove all these lists and have a custom getter setter
    const G3D::PropertyChain *link =
        dynamic_cast<const G3D::PropertyChain *>(this);
    while (link) {
        if (link->getName() == "AShape") {

            break;
        } else if (link->getName() == "Base") {
        }
        link = link->getNext();
    }
    const AShape *cshape = dynamic_cast<const G3D::AShape *>(link);
    shared_ptr<G3D::Shape> gShape = cshape->getShape();
    if (gShape->type() == G3D::Shape::Type::BOX) {
        boxArray.push_back(gShape->box());
    } else {
        debugAssertM(false, "Adding a non-box like shape for GhostEntity");
    }

    MarkerEntity::init(boxArray, color);

    // Hold a pointer to this for ez behavior modification
    m_physicsScene = dynamic_cast<PhysicsScene *>(m_scene);
    debugAssertM(m_physicsScene, "Scene is not a PhysicsScene");

    // Add default behaviors
    // this->addBehavior(new FetchPhysicsUpdate());
}

} // namespace G3D
