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

#include <map>
#include <set>

#include "BulletCollision/BroadphaseCollision/btDbvtBroadphase.h"
#include "BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolverMt.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"
#include "LinearMath/btDefaultMotionState.h"

#include "G3D-app/ArticulatedModel.h"
#include "G3D-app/Shape.h"
#include "G3D-app/VisibleEntity.h"
#include "G3D-base/CoordinateFrame.h"
#include "G3D-base/Vector3.h"
#include "G3D-gfx/CPUVertexArray.h"

#include "BulletFactory.h"
#include "PurePhysics.h"

/** \brief Bullet Physics implementations
 *
 * This attempts to implement the PurePhysics class via the bullet library.  The
 * PurePhysics class was in fact modelled in the image of bullet.
 *
 */

class BulletPhysics : public PurePhysics {
    /**
     * Bullet works best with raw pointers so take care with this class
     */
    btCollisionConfiguration *m_collisionConfig;
    btDispatcher *m_collisionDispatcher;
    btBroadphaseInterface *m_broadPhase;
    btConstraintSolverPoolMt *m_solverPool;
    btConstraintSolver *m_solver;
    btDynamicsWorld *m_dynamicsWorld;
    // TODO: How to handle the lifecycle management? Suppose a scenario in which
    // the RigidEntity is not visible anymore, then we don't need to simulate
    // that or if it goes out of scope, then also the simulation needs to be
    // filtered for collision detection only between relevant objects A map
    // might be the best option to update and track entities which were added
    // TODO: Lifecycle or Object Pool may be used inside the Factory itself and
    // have this class use it as a private member
    std::map<const G3D::Entity *, btCollisionObject *> m_dynamicBodyMap;

public:
    BulletPhysics();
    virtual ~BulletPhysics() override;

    // TODO:Does the object need to be moved or copied? For now it seems no, so
    // delete the move and copy constructors if there is a need, then implement
    // these.
    BulletPhysics(const BulletPhysics &) = delete;
    BulletPhysics &operator=(const BulletPhysics &) = delete;

    BulletPhysics(BulletPhysics &&) = delete;
    BulletPhysics &&operator=(BulletPhysics &&) = delete;

    virtual void insertEntity(const G3D::Entity *entity) override;

    virtual void removeEntity(const G3D::Entity *entity) override;

    virtual void applyForceField(const G3D::Entity *field, const G3D::Vector3 force) override;
   // , const G3D::VisibleEntity *reactor) override;

    virtual G3D::CoordinateFrame getFrame(const G3D::Entity *entity) override;

    virtual void setFrame(const G3D::Entity *entity,
                          const G3D::CoordinateFrame frame) override;

    virtual void applyForce(G3D::Entity *entity, G3D::Point3 force) override;

    virtual void simulate(float deltaTime) override;

private:
    // For creating the solver pool
    btConstraintSolverPoolMt *createSolverPool();
};
