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

#include "Constraint.h"
#include "G3D-app/Entity.h"
#include "G3D-app/G3D-app.h"
#include "G3D-base/G3D-base.h"
#include "G3D-base/Matrix3x4.h"
#include "G3D-base/Vector3.h"
#include "G3D-gfx/G3D-gfx.h"

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
    enum FieldType { DIRECTIONAL, RADIAL };

    PurePhysics() = default;
    virtual ~PurePhysics() = default;
    /** Insertion routine for different objects. Since G3D provides the m_pose
     * as a protected member for VisibleEntity, we cannot create a generic
     * CollisionEntity from it.
     * TODO: Implement this class using at least 2-3 physics engines, first with
     * bullet and next with PhysX also try with a AI based engine as that is
     * supposed to be a bit faster
     */
    virtual void insertEntity(const G3D::Entity *entity) = 0;

    virtual void removeEntity(const G3D::Entity *entity) = 0;

    /** #33 needs an interface like this
     */
    virtual void applyForceField(const G3D::Entity *field,
                                 const G3D::Vector3 force,
                                 const FieldType type = DIRECTIONAL) = 0;

    virtual G3D::Constraint *const
    addConstraint(const G3D::Entity *entityA, const G3D::Entity *entityB) = 0;

    virtual void removeConstraint(const G3D::Constraint *const constraint) = 0;

    /** This may return nullptr when we don't have any colliders, take care of
     * this in the client */
    virtual const G3D::Entity *const
    getPrimaryCollider(const G3D::Entity *trigger) = 0;

    virtual void ignoreCollisionCheck(const G3D::Entity *trigger,
                                      const G3D::Entity *collider) = 0;

    /** Extract the CoordinateFrame information from whatever RT datatypes the
     * underlying physics engine supports so that G3D can make the corresponding
     * updates to the entity. This should be a generic method and we should
     * dynamically cast it for all possible cases to determine the type of
     * object.
     */
    virtual G3D::CoordinateFrame getFrame(const G3D::Entity *entity) = 0;
    // TODO: to be solved as a wider problem of object management after
    // behavioral entity is in action
    virtual void reconstructRigidBody(const G3D::Entity *entity) = 0;

    /** Apply a force to the object */
    virtual void applyForce(G3D::Entity *entity, G3D::Point3 force) = 0;

    /** Override this method in the implementation to update the underlying
     * physics world
     */
    virtual void simulate(float deltaTime) = 0;
};
