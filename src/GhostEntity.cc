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
#include "G3D-gfx/Profiler.h"
#include "PhysicsScene.h"
#include "behavior/BChain.h"
#include "behavior/PChain.h"

namespace G3D {
GhostEntity::GhostEntity() : MarkerEntity(){};

void GhostEntity::onSimulation(SimTime absoluteTime, SimTime deltaTime) {

    this->apply(this);
}

shared_ptr<Entity> GhostEntity::create(const String &name, Scene *scene,
                                       AnyTableReader &propertyTable,
                                       const ModelTable &modelTable,
                                       const Scene::LoadOptions &loadOptions) {
    shared_ptr<GhostEntity> physEntity(new GhostEntity());

    physEntity->Entity::init(name, scene, propertyTable);
    physEntity->GhostEntity::init(propertyTable);

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

    const G3D::PropertyChain *link =
        dynamic_cast<const G3D::PropertyChain *>(this);

    for (auto it = link->begin(); !it.isOver(); it.advance()) {
        if (it->getName() == "AShape") {
            const AShape *cshape = dynamic_cast<const G3D::AShape *>(*it);
            shared_ptr<G3D::Shape> gShape = cshape->getShape();
            if (gShape->type() == G3D::Shape::Type::BOX) {
                boxArray.push_back(gShape->box());
            } else {
                debugAssertM(false,
                             "Adding a non-box like shape for GhostEntity");
            }
            break;
        }
    }

    MarkerEntity::init(boxArray, color);

    // Hold a pointer to this for ez behavior modification
    m_physicsScene = dynamic_cast<PhysicsScene *>(m_scene);
    debugAssertM(m_physicsScene, "Scene is not a PhysicsScene");

    ForceField<GhostEntity> ff;
    if (propertyTable.getIfPresent("field", ff)) {
        this->addBehavior(new ForceField<GhostEntity>(ff));
    }
    Attractor<GhostEntity> att;
    if (propertyTable.getIfPresent("attractor", att)) {
        this->addBehavior(new Attractor<GhostEntity>(att));
    }
}

Any GhostEntity::toAny(const bool forceAll) const {
    Any a = Entity::toAny(forceAll);
    a.setName("GhostEntity");
    a["color"] = m_color;
    // No need for a massive GhostEntity, initialize it to 0 all the time
    a.remove("mass");

    const G3D::PropertyChain *link =
        dynamic_cast<const G3D::PropertyChain *>(this);
    for (auto it = link->begin(); !it.isOver(); it.advance()) {
        if (it->getName() == "AShape") {
            const AShape *ashape = dynamic_cast<const G3D::AShape *>(*it);
            a.set("ashape", *ashape);
        }
    }
    
    return a;
}

} // namespace G3D
