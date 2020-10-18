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

#include "G3D-app/G3D-app.h"
#include "G3D-base/G3D-base.h"
#include "G3D-gfx/G3D-gfx.h"

#include "physics/BulletPhysics.h"
#include "physics/PurePhysics.h"

/** \brief An inherited Scene with support for Physics
 *
 * This is the PhysicsScene which inherits from the G3D::Scene and has extended
 * the former by accomodating for a custom onSimulate method that runs a Physics
 * simulation from an external Physics engine and updates the G3D objects
 * accordingly. Right now, this implementation only supports a single physics
 * world but later, we might need different worlds to run in parallel and on
 * demand.
 */

namespace G3D {
class PhysicsScene : public Scene {
    // TODO: make the conversion from using concrtete type to abstract type
    shared_ptr<PurePhysics> m_physics;
    shared_ptr<Entity> m_player;
    //shared_ptr<Entity> m_forceField;
    int numSphere = 1;

public:
    CoordinateFrame m_playerMotion;

protected:
    PhysicsScene(const shared_ptr<AmbientOcclusion> &ao);

public:
    static shared_ptr<PhysicsScene>
    create(const shared_ptr<AmbientOcclusion> &ao);
    /** This should be overriden to support the physics entities, models don't
     * have to be since they are decorated instances of the entity themselves
     */
    virtual shared_ptr<Entity>
    insert(const shared_ptr<Entity> &entity) override;

    virtual Any load(const String& sceneName, const LoadOptions& loadOptions = LoadOptions()) override;

    /** Remove all original Scene objects alone with the custom Physics objects
     */
    virtual void clear() override;

    /** This is the crux method which needs to be run, using both the existing
     * simple animations and the Physics based natural animation may be needed
     * in a few instances, so best idea would be to use both. Either case, run
     * the external Physics engine simulation in this method.
     */
    virtual void onSimulation(SimTime deltaTime) override;

    shared_ptr<PurePhysics> getPhysicsEngine();

    shared_ptr<Entity> getPlayer();

    void addBoxArray(String name, Vector2 grid, Vector3 position,
                     Vector3 direction);
};
} // namespace G3D
