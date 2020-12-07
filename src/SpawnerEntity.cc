/** Copyright 2020 Blood Eagle Studio
 *
 * You may not use, not distribute and not modify this code
 * under any manifestable possibility and if such a scenario
 * occurs, any changes to the code must be reviewed by the
 * original author of this project.
 *
 *  Author : Siddharth J Singh(dante)
 */

#include "SpawnerEntity.h"

namespace G3D {

SpawnerEntity::SpawnerEntity() : GhostEntity(){};

void SpawnerEntity::onSimulation(SimTime absoluteTime, SimTime deltaTime) {
    m_timer += isNaN(deltaTime) ? 0 : deltaTime;
    if (m_timer > m_time) {
        // have a timer and spawn entities here
        AnyTableReader propTable(prototype);
        shared_ptr<Entity> entity = PhysicsEntity::create(
            this->m_name + String("_") + String(this->id++), this->m_scene, propTable,
            this->m_scene->modelTable(), Scene::LoadOptions());
	m_spawnedEntities.push_back(entity);
        // Should insert this for physics interactions
        m_scene->insert(entity);
	m_timer = 0;
    }
}

shared_ptr<Entity> SpawnerEntity::create(
    const String &name, Scene *scene, AnyTableReader &propertyTable,
    const ModelTable &modelTable, const Scene::LoadOptions &loadOptions) {
    // Don't initialize in the constructor, where it is unsafe to throw Any
    // parse exceptions
    shared_ptr<SpawnerEntity> spawnEntity(new SpawnerEntity());

    spawnEntity->Entity::init(name, scene, propertyTable);
    spawnEntity->GhostEntity::init(propertyTable);
    spawnEntity->init(propertyTable);

    propertyTable.verifyDone();

    return spawnEntity;
}
void SpawnerEntity::init(AnyTableReader &propertyTable) {
    propertyTable.getIfPresent("time", m_time);
    propertyTable.getIfPresent("spawn", m_spawn);
    id = 0;

    Any box(Any::TABLE, "PhysicsEntity");
    box["model"] = "boxModel";
    box["frame"] = CFrame(this->frame());
    box["mass"] = 0.25;
    Any ashape(Any::TABLE, "AShape");
    ashape["box"] = Box(Point3(-1, -1, -1), Point3(1, 1, 1));
    box["ashape"] = ashape;
    box["shouldBeSaved"] = false;
    prototype = box;
}

Any SpawnerEntity::toAny(const bool forceAll) const {
    Any a = GhostEntity::toAny(forceAll);
    a.setName("SpawnerEntity");
    a["time"] = m_time;
    a["spawn"] = m_spawn;

    return a;
}
void SpawnerEntity::makeGUI(class GuiPane *pane, class GApp *app) {}

} // namespace G3D
