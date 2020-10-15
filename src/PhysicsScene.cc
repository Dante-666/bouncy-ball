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
    m_physics->insertEntity(entity.get());
    if (entity->name() == "player") {
        m_player = entity;
        m_playerMotion = entity->frame();
    }
    Scene::insert(entity);
    return entity;
}

Any PhysicsScene::load(const String &sceneName,
                       const LoadOptions &loadOptions) {
    auto any = Scene::load(sceneName);
    return any;
}

void PhysicsScene::clear() { Scene::clear(); };

void PhysicsScene::onSimulation(SimTime deltaTime) {
    // TODO: update this to drop 1 ball every second
    if (!isNaN(deltaTime) && !(deltaTime == 0.0)) {
        m_physics->simulate(deltaTime);
    }

    // This method needs to be called so that we can have objects updating
    // themselves by overriding the onSimulation methods defined within them,
    // this way we can support both G3D animations and the custom physical
    // simulations
    Scene::onSimulation(deltaTime);
};

shared_ptr<PurePhysics> PhysicsScene::getPhysicsEngine() { return m_physics; }

shared_ptr<Entity> PhysicsScene::getPlayer() { return m_player; }

void PhysicsScene::addBoxArray(String name, Vector2 grid, Vector3 position,
                               Vector3 direction) {
    Scene::LoadOptions options;
    for (int j = 0; j < grid.y; j++) {
        for (int i = 0; i < grid.x; i++) {
            Any box(Any::TABLE, "RigidEntity");
            box["model"] = "boxModel";
            box["frame"] = CFrame::fromXYZYPRDegrees(position.x + i * (2 + 0.1),
                                                     position.y + j * (2 + 0.1),
                                                     0, 0, 0, 0);
            box["mass"] = 0.125;
            box["collisionShape"] = "BOX";
            box["shape"] = Box(Point3(-1, -1, -1), Point3(1, 1, 1));
            AnyTableReader propertyTable(box);
            auto sz = m_modelTable.size();
            shared_ptr<Entity> entity =
                RigidEntity::create(name + String(std::to_string(numSphere++)),
                                    this, propertyTable, m_modelTable, options);
            insert(entity);
        }
    }
}

} // namespace G3D
