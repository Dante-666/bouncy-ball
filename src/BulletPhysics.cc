/** Copyright 2020 Blood Eagle Studio
 *
 * You may not use, not distribute and not modify this code 
 * under any manifestable possibility and if such a scenario
 * occurs, any changes to the code must be reviewed by the 
 * original author of this project.
 *
 *  Author : Siddharth J Singh(dante)
 */

#include "BulletPhysics.h"
#include "RigidEntity.h"

BulletPhysics::BulletPhysics()
    : m_collisionConfig(new btDefaultCollisionConfiguration()),
      m_collisionDispatcher(new btCollisionDispatcher(m_collisionConfig)),
      m_broadPhase(new btDbvtBroadphase()),
      m_solver(new btSequentialImpulseConstraintSolver()),
      m_dynamicsWorld(new btDiscreteDynamicsWorld(
          m_collisionDispatcher, m_broadPhase, m_solver, m_collisionConfig)) {
    // Initialize gravity
    m_dynamicsWorld->setGravity(btVector3(0, -10, 0));
    //add a couple of objects here to see how the simulation runs

    btCollisionShape *groundShape =
        new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

    m_collisionShapes.push_back(groundShape);

    btTransform groundTransform;
    groundTransform.setIdentity();
    groundTransform.setOrigin(btVector3(0, -50, 0));

    btScalar mass(0.);

    // rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
        groundShape->calculateLocalInertia(mass, localInertia);

    // using motionstate is optional, it provides interpolation capabilities,
    // and only synchronizes 'active' objects
    btDefaultMotionState *myMotionState =
        new btDefaultMotionState(groundTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState,
                                                    groundShape, localInertia);
    btRigidBody *body = new btRigidBody(rbInfo);

    // add the body to the dynamics world
    m_dynamicsWorld->addRigidBody(body);

    btCollisionShape *colShape = new btSphereShape(btScalar(1.));
    m_collisionShapes.push_back(colShape);

    /// Create Dynamic Objects
    btTransform startTransform;
    startTransform.setIdentity();

    mass = 1.f;

    localInertia = btVector3(0, 0, 0);
    colShape->calculateLocalInertia(mass, localInertia);

    startTransform.setOrigin(btVector3(2, 15, 0));

    // using motionstate is recommended, it provides interpolation capabilities,
    // and only synchronizes 'active' objects
    btDefaultMotionState *myMotionState2 =
        new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo2(mass, myMotionState2,
                                                     colShape, localInertia);
    btRigidBody *body2 = new btRigidBody(rbInfo2);

    m_dynamicsWorld->addRigidBody(body2);
}

BulletPhysics::~BulletPhysics() {
    m_collisionShapes.clear();
    delete m_dynamicsWorld;
    delete m_solver;
    delete m_broadPhase;
    delete m_collisionDispatcher;
    delete m_collisionConfig;
}

void BulletPhysics::insertRigidEntity(G3D::RigidEntity *entity) {
}

    
G3D::CoordinateFrame BulletPhysics::getFrame(G3D::VisibleEntity *entity) {

    auto rba= m_dynamicsWorld->getCollisionObjectArray();
    auto rb = btRigidBody::upcast(rba[1]);
    //did this guy die?
    btTransform trans;
    rb->getMotionState()->getWorldTransform(trans);

    G3D::Vector3 rows[3];
    //const btVector3 bcols[3];
    btMatrix3x3 bBasis = trans.getBasis();
    for(int i = 0; i < 3; i++) {
        const auto brow = bBasis[i];
        rows[i] = G3D::Vector3(brow.x(), brow.y(), brow.z());
    }

    G3D::Matrix3 rotation = G3D::Matrix3::fromRows(rows[0], rows[1], rows[2]);
    btVector3 bOrigin = trans.getOrigin();
    G3D::Point3 translation(bOrigin.x(), bOrigin.y(), bOrigin.z());

    return G3D::CoordinateFrame(rotation, translation);
}
void BulletPhysics::simulate(float deltaTime) {
    m_dynamicsWorld->stepSimulation(deltaTime);
}
