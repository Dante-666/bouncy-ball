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

#include "G3D-app/VisibleEntity.h"
#include "RigidEntity.h"

/** \brief Physics Abstraction Layer
 *
 * This is the abstraction layer for physics and the general idea is that it can
 * be inherited to make different implementations work for different physics
 * engines. Almost all of them work in a similar fashion, in which a certain
 * timestep has to be provided by the user, or in this case, the renderer and
 * the physics simulation undergoes the time evolution.
 *
 * List of Matter supported:
 * 1. StaticMatter : Is immovable and doesn't deform (ground)
 * 2. RigidMatter  : It may move around and collide but won't change shape
 *		     (a box with high plasticity)
 *
 */

class PurePhysics {
public:
    PurePhysics() = default;
    virtual ~PurePhysics() = default;
    /** Insertion routine for different objects. Since G3D provides the m_pose
     * as a protected member for VisibleEntity, we cannot create a generic
     * CollisionEntity from it.
     * TODO: The VisibleEntity itself can be superclass CollisionEntity and when
     * generalization is needed, deprecate the individual interfaces.
     * TODO: Implement this class using at least 2-3 physics engines, first with
     * bullet and next with PhysX also try with a AI based engine as that is
     * supposed to be a bit faster
     */
    virtual void insertRigidEntity(G3D::RigidEntity *entity) = 0;
    virtual void removeRigidEntity(G3D::RigidEntity *entity) = 0;

    /** Extract the CoordinateFrame information from whatever RT datatypes the
     * underlying physics engine supports so that G3D can make the corresponding
     * updates to the entity. This should be a generic method and we should
     * dynamically cast it for all possible cases to determine the type of
     * object.
     */
    virtual G3D::CoordinateFrame getFrame(G3D::VisibleEntity *entity) = 0;

    /** Apply a force to the object */
    virtual void applyForce(G3D::VisibleEntity *entity, G3D::Point3 force) = 0;

    /** Override this method in the implementation to update the underlying
     * physics world
     */
    virtual void simulate(float deltaTime) = 0;
};
