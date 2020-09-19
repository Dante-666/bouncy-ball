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
    : Scene(ao), m_physics(std::make_unique<BulletPhysics>()) {}

shared_ptr<PhysicsScene>
PhysicsScene::create(const shared_ptr<AmbientOcclusion> &ao) {
    return createShared<PhysicsScene>(ao);
}
// shared_ptr<Entity> PhysicsScene::insert(const shared_ptr<Entity> &entity) {}

/*shared_ptr<Entity>
PhysicsScene::createEntity(const String &name, const Any &any,
                           const Scene::LoadOptions &options){};

shared_ptr<Entity>
PhysicsScene::createEntity(const String &entityType, const String &name,
                           const Any &any, const Scene::LoadOptions &options){};



Any PhysicsScene::load(const String &sceneName,
                       const LoadOptions &loadOptions){};*/
void PhysicsScene::clear(){
    Scene::clear();
};
void PhysicsScene::onSimulation(SimTime deltaTime) {
    if(isNaN(deltaTime) || deltaTime == 0.0) return;
    m_physics->simulate(deltaTime);

    //auto frame = m_physics->getFrame(nullptr);
    for (int i = 0; i < m_entityArray.size(); ++i) {
        const shared_ptr<Entity> entity = m_entityArray[i];
        
	const shared_ptr<RigidEntity>& re = dynamic_pointer_cast<RigidEntity>(entity);
	if(re)
            re->updateFrame(m_physics->getFrame(nullptr));
    }
};
} // namespace G3D
