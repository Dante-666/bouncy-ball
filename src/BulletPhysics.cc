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
      m_broadPhase(new btDbvtBroadphase()), m_solverPool(createSolverPool()),
      m_solver(new btSequentialImpulseConstraintSolverMt()),
      m_dynamicsWorld(new btDiscreteDynamicsWorldMt(
          m_collisionDispatcher, m_broadPhase, m_solverPool, m_solver,
          m_collisionConfig)) {
    // Initialize gravity
    m_dynamicsWorld->setGravity(btVector3(0, -10, 0));
    btSetTaskScheduler(btGetTBBTaskScheduler());
}

BulletPhysics::~BulletPhysics() {
    m_dynamicBodyMap.clear();
    m_collisionShapes.clear();
    delete m_dynamicsWorld;
    delete m_solver;
    delete m_broadPhase;
    delete m_collisionDispatcher;
    delete m_collisionConfig;
}

void BulletPhysics::insertEntity(G3D::VisibleEntity *entity) {
    btRigidBody *body = RigidBodyFactory::create(entity);

    // TODO: is this needed?
    // One possible reason could be to maintain the pointer at memory
    m_collisionShapes.push_back(body->getCollisionShape());

    m_dynamicsWorld->addRigidBody(body);

    m_dynamicBodyMap.insert(std::pair(entity, body));
}

void BulletPhysics::removeEntity(G3D::VisibleEntity *entity) {
    auto iter = m_dynamicBodyMap.find(entity);
    debugAssertM(iter != m_dynamicBodyMap.end(),
                 "Entity to be removed is not present in the map");
    m_dynamicBodyMap.erase(iter);
    // TODO: more stuff for removing the entity from the dynamics world also
    // to be performed here
}

G3D::CoordinateFrame BulletPhysics::getFrame(G3D::VisibleEntity *entity) {
    auto iter = m_dynamicBodyMap.find(entity);
    debugAssertM(iter != m_dynamicBodyMap.end(),
                 "Queried entity is not present in the map");
    auto collisionObject = iter->second;
    btRigidBody *rigidBody = btRigidBody::upcast(collisionObject);

    btTransform trans;
    if (rigidBody && rigidBody->getMotionState()) {
        rigidBody->getMotionState()->getWorldTransform(trans);
    } else {
        trans = collisionObject->getWorldTransform();
    }

    return Frame::convert(trans);
}

void BulletPhysics::applyForce(G3D::VisibleEntity *entity, G3D::Point3 force) {
    auto iter = m_dynamicBodyMap.find(entity);
    debugAssertM(iter != m_dynamicBodyMap.end(),
                 "Queried entity is not present in the map");
    auto collisionObject = iter->second;
    btRigidBody *rigidBody = btRigidBody::upcast(collisionObject);
    rigidBody->applyCentralForce(Vector::convert(force));
}

void BulletPhysics::simulate(float deltaTime) {
    m_dynamicsWorld->stepSimulation(deltaTime);
}

btConstraintSolverPoolMt *BulletPhysics::createSolverPool() {
    btConstraintSolverPoolMt *solverPool;

    btConstraintSolver *solvers[BT_MAX_THREAD_COUNT];
    int maxThreadCount = BT_MAX_THREAD_COUNT;
    for (int i = 0; i < maxThreadCount; ++i) {
        solvers[i] = new btSequentialImpulseConstraintSolver();
    }
    return new btConstraintSolverPoolMt(solvers, maxThreadCount);
}
