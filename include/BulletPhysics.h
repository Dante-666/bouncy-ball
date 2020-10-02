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
#include "BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h"

#include "BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolverMt.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.h"

#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"

#include "G3D-base/Matrix3x4.h"
#include "G3D-base/Vector3.h"
#include "PurePhysics.h"
//#include "btBulletDynamicsCommon.h"

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

    btAlignedObjectArray<btCollisionShape *> m_collisionShapes;
    // TODO: seems to be but it's always better to keep a map of these entries
    // TODO: do we actually need to store this? we definetly need a pointer to
    // store the btCollisionObject array which is obtained from the
    // dynamicsWorld
    // TODO: How to handle the lifecycle management? Suppose a scenario in which
    // the RigidEntity is not visible anymore, then we don't need to simulate
    // that or if it goes out of scope, then also the simulation needs to be
    // filtered for collision detection only between relevant objects A map
    // might be the best option to update and track entities which were added
    std::map<G3D::VisibleEntity *, btCollisionObject *> m_dynamicBodyMap;
    std::set<btCollisionObject *> m_staticBodySet;

public:
    // TODO: remove objects created for testing
    BulletPhysics();
    virtual ~BulletPhysics() override;

    // TODO:Does the object need to be moved or copied? For now it seems no, so
    // delete the move and copy constructors if there is a need, then implement
    // these.
    BulletPhysics(const BulletPhysics &) = delete;
    BulletPhysics &operator=(const BulletPhysics &) = delete;

    BulletPhysics(BulletPhysics &&) = delete;
    BulletPhysics &&operator=(BulletPhysics &&) = delete;

    virtual void insertRigidEntity(G3D::RigidEntity *entity) override;

    virtual void removeRigidEntity(G3D::RigidEntity *entity) override;

    virtual G3D::CoordinateFrame getFrame(G3D::VisibleEntity *entity) override;

    virtual void applyForce(G3D::VisibleEntity *entity, G3D::Point3 force) override;

    virtual void simulate(float deltaTime) override;

private:
    // For creating the solver pool
    btConstraintSolverPoolMt *createSolverPool();

    // Helper functions for easy conversion of data
    const inline G3D::CoordinateFrame convertToG3D(const btTransform transform);
    const inline G3D::Matrix3 convertToG3D(const btMatrix3x3 matrix);
    const inline G3D::Point3 convertToG3D(const btVector3 vector);

    const inline btTransform convertFromG3D(const G3D::CoordinateFrame frame);
    const inline btMatrix3x3 convertFromG3D(const G3D::Matrix3 matrix);
    const inline btVector3 convertFromG3D(const G3D::Vector3 vector);
};
