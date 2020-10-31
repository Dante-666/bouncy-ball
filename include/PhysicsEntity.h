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

/** \brief Derived from a VisibleEntity
 * Designed to be the interface the the physics object and must be able to add
 * runtime behavior which could also be captured by the Any specification. This
 * is important because ultimately the objects are being constructed initially
 * from a Scene. So it's much better to take in the features of Scene and mould
 * custom classes around it.
 *
 * The SceneEditor revealed a problem with ongoing simulation, if an object is
 * grabbed while editing and when it is unselected, the rigid entity has to be
 * reitialized with a new rigid object at least for bullet. Seems like a dirty
 * way of doing it but also makes sense. This also let's us thing about
 * lifecycle management by another object or by a behavior.
 *
 * What is PropertyChain? To construct/delete/modify physics object which may
 * involve storing the states.
 *
 * What is BehaviorChain? It is how the entity is supposed to interact in the
 * Physical realm. Like manipulator, teleporter. There may be some complex
 * interactions which may not be forseen yet
 */

#include "G3D-app/G3D-app.h"
#include "G3D-base/G3D-base.h"
#include "G3D-gfx/G3D-gfx.h"

#include "behavior/BChain.h"
#include "behavior/PChain.h"

#include "behavior/AShape.h"
#include "behavior/Solid.h"

#include "behavior/FetchPhysicsUpdate.h"

#include <memory>

namespace G3D {

class PhysicsScene;

class PhysicsEntity : public VisibleEntity,
                      private BehaviorChain<PhysicsEntity>,
                      public PropertyChain {
    PhysicsScene *m_physicsScene;

public:
    friend class FetchPhysicsUpdate<PhysicsEntity>;

    PhysicsEntity();

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
