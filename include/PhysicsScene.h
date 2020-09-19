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

#include <G3D-base/G3D-base.h>
#include <G3D-gfx/G3D-gfx.h>
#include <G3D-app/G3D-app.h>

#include "PurePhysics.h"
#include "BulletPhysics.h"

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
    unique_ptr<BulletPhysics> m_physics;

protected:
    PhysicsScene(const shared_ptr<AmbientOcclusion> &ao);

public:
    static shared_ptr<PhysicsScene>
    create(const shared_ptr<AmbientOcclusion> &ao);
    /** This should be overriden to support the physics entities, models don't
     * have to be since they are quite independent from the entity themselves
     * TODO: maybe not
     *
    virtual shared_ptr<Entity>
    insert(const shared_ptr<Entity> &entity) override;*/

    /** We need to override this because of the way custom Physics objects are
     * being handled right now. Keep a separate array for the Physics objects
     * and update their Pose based on the simulation results obtained from the
     * underlying physics layer
     * TODO: may not need to do this/
    virtual shared_ptr<Entity> createEntity(
        const String &name, const Any &any,
        const Scene::LoadOptions &options = Scene::LoadOptions()) override;

    virtual shared_ptr<Entity> createEntity(
        const String &entityType, const String &name, const Any &any,
        const Scene::LoadOptions &options = Scene::LoadOptions()) override;*/

    /** Remove all original Scene objects alone with the custom Physics objects
     */
    virtual void clear() override;

    /** Because of custom objects in the scene, we need to override this method
     */
    // TODO:Disbale for now
    /*virtual Any load(const String &sceneName,
                     const LoadOptions &loadOptions = LoadOptions()) override;*/

    // TODO: there is a lot of dependency on the m_entityArray so we need to
    // override more methods which are seeming unreasonable for the design now
    // let's avoid doing all of that and use the original array itself but
    // handle the physics animation in the Entity's onSimulation method.
    // TODO: for simple objects, this seems okay and fine but how do we do this
    // for complicated types like character animations? that's a problem for
    // later stage I believe
    /** This is the crux method which needs to be run, using both the existing
     * simple animations and the Physics based natural animation may be needed
     * in a few instances, so best idea would be to use both. Either case, run
     * the external Physics engine simulation in this method.
     */
    virtual void onSimulation(SimTime deltaTime) override;
};
} // namespace G3D
