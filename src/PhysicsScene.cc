/** Copyright 2020 Blood Eagle Studio
 *
 * You may not use, not distribute and not modify this code
 * under any manifestable possibility and if such a scenario
 * occurs, any changes to the code must be reviewed by the
 * original author of this project.
 *
 *  Author : Siddharth J Singh(dante)
 */

#include "PhysicsScene.h"

namespace G3D {
PhysicsScene::PhysicsScene(const shared_ptr<AmbientOcclusion> &ao)
    : Scene(ao), m_physics(std::make_shared<BulletPhysics>()) {}

shared_ptr<PhysicsScene>
PhysicsScene::create(const shared_ptr<AmbientOcclusion> &ao) {
    return createShared<PhysicsScene>(ao);
}

shared_ptr<Entity> PhysicsScene::insert(const shared_ptr<Entity> &entity) {
    debugAssert(notNull(entity));
    // handle the insertion and TODO deletion of the RigidEntity a bit
    // differently and update the correlation map which has the information
    const shared_ptr<RigidEntity> &rigid =
        dynamic_pointer_cast<RigidEntity>(entity);

    if (notNull(rigid)) {
        debugAssertM(!m_entityTable.containsKey(entity->name()),
                     "Two Entitys with the same name, \"" + entity->name() +
                         ":" + numSphere + "\"");

        m_entityTable.set(rigid->name(), entity);
        m_entityArray.append(rigid);
        m_lastStructuralChangeTime = System::time();

        m_physics->insertRigidEntity(rigid.get());
        
	m_lastVisibleChangeTime = System::time();
        if (entity->name() == "player") {
            m_player = rigid;
	    m_playerMotion = rigid->frame();
        }
        entity->onSimulation(m_time, 0);
        Array<shared_ptr<Surface>> ignore;
        entity->onPose(ignore);
    } else {
        return Scene::insert(entity);
    }

    return entity;
}

void PhysicsScene::clear() { Scene::clear(); };

void PhysicsScene::onSimulation(SimTime deltaTime) {
    // TODO: update this to drop 1 ball every second
    if (!isNaN(deltaTime) && !(deltaTime == 0.0)) {
        m_physics->simulate(deltaTime);
        m_sec += deltaTime;
    }

    /*if (m_sec > 5.f && m_time < 16.f) {
        m_sec = 0.f;
        String name = "rigid" + String(numSphere++);
        Scene::LoadOptions options;
        Any anyDef = Any::parse(STR(RigidEntity {
            model = "playerModel";
            frame = CFrame::fromXYZYPRDegrees(5, 10, 0, 0, 0, 0);
            mass = 1.0;
            collisionShape = "SPHERE";
            shape = Sphere(1.0);
        }));
        AnyTableReader propertyTable(anyDef);
        shared_ptr<Entity> entity = RigidEntity::create(
            name, this, propertyTable, m_modelTable, options);
        insert(entity);
    }*/

    /*if (m_time > 10.f) {
        remove(m_player);
    }*/

    // This method needs to be called so that we can have objects updating
    // themselves by overriding the onSimulation methods defined within them,
    // this way we can support both G3D animations and the custom physical
    // simulations
    Scene::onSimulation(deltaTime);
};

shared_ptr<PurePhysics> PhysicsScene::getPhysicsEngine() { return m_physics; }

shared_ptr<VisibleEntity> PhysicsScene::getPlayer() { return m_player; }

} // namespace G3D
