/** Copyright 2020 Blood Eagle Studio
 *
 * You may not use, not distribute and not modify this code
 * under any manifestable possibility and if such a scenario
 * occurs, any changes to the code must be reviewed by the
 * original author of this project.
 *
 *  Author : Siddharth J Singh(dante)
 */

#pragma once

/** \brief Derived from a MarkerEntity
 * This is basically a triggerable object, any MarkerEntity is. Here, the
 * trigger is time based and user can set the shape/type of the desired
 * PhysicsEntity to be spawned.
 */

#include "G3D-app/G3D-app.h"
#include "G3D-base/G3D-base.h"
#include "G3D-gfx/G3D-gfx.h"

#include "GhostEntity.h"
#include "PhysicsScene.h"

namespace G3D {

class PhysicsScene;

class SpawnerEntity : public GhostEntity {
    float m_time = 0;
    float m_timer = 0;
    bool m_spawn = false;
    int id;
    Any prototype;

    std::vector<shared_ptr<Entity>> m_spawnedEntities;

public:
    SpawnerEntity();

    /** update the pose of object here by querying the physics engine */
    virtual void onSimulation(SimTime absoluteTime, SimTime deltaTime) override;

    /** For deserialization from Any / loading from file */
    static shared_ptr<Entity> create(const String &name, Scene *scene,
                                     AnyTableReader &propertyTable,
                                     const ModelTable &modelTable,
                                     const Scene::LoadOptions &loadOptions);

    void init(AnyTableReader &propertyTable);

    virtual Any toAny(const bool forceAll) const override;

    virtual void makeGUI(class GuiPane *pane, class GApp *app) override;

protected:
    class GuiDropDownList *m_propertyDropDownList;

    void onPropertyDropDownAction();
};
} // namespace G3D
