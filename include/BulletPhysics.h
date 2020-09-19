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

#include "btBulletDynamicsCommon.h"
#include "PurePhysics.h"

/** \brief Bullet Physics implementations
 *
 * This attempts to implement the PurePhysics class via the bullet library.  The
 * PurePhysics class was in fact modelled in the image of bullet.
 *
 */

class BulletPhysics : PurePhysics {
    /** 
     * Bullet works best with raw pointers so take care with this class
     */
    btCollisionConfiguration *m_collisionConfig;
    btCollisionDispatcher *m_collisionDispatcher;
    btBroadphaseInterface *m_broadPhase;
    btSequentialImpulseConstraintSolver *m_solver;
    btDiscreteDynamicsWorld *m_dynamicsWorld;

    btAlignedObjectArray<btCollisionShape *> m_collisionShapes;
    // TODO: check if the order of these objects is maintained in all scenarios,
    // then a simple counter and a map can be used to keep track of the shapes
    // which were added here for faster query.
    // TODO: do we actually need to store this? we definetly need a pointer to
    // store the btCollisionObject array which is obtained from the dynamicsWorld
    // TODO: How to handle the lifecycle management? Suppose a scenario in which
    // the RigidEntity is not visible anymore, then we don't need to simulate that
    // or if it goes out of scope, then also the simulation needs to be filtered
    // for collision detection only between relevant objects

public:
    // TODO: remove objects created for testing
    BulletPhysics();
    virtual ~BulletPhysics() override;

    // TODO:Does the object need to be moved or copied? For now it seems no, so
    // delete the move and copy constructors if there is a need, then implement
    // these.
    BulletPhysics(const BulletPhysics&) = delete;
    BulletPhysics& operator=(const BulletPhysics &) = delete;

    BulletPhysics(BulletPhysics &&) = delete;
    BulletPhysics &&operator=(BulletPhysics &&) = delete;

    virtual void insertRigidEntity(G3D::RigidEntity *entity) override;
    virtual G3D::CoordinateFrame getFrame(G3D::VisibleEntity *entity) override;

    // TODO:This method has to be overriden apart from a few custom methods which
    // will interface the objects between this world and G3D world. That would
    // require some major hard work and design challenges
    virtual void simulate(float deltaTime) override;
};
