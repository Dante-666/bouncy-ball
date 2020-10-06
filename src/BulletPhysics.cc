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

#include "ForceFieldEntity.h"
#include "G3D-app/MarkerEntity.h"
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
    delete m_dynamicsWorld;
    delete m_solver;
    delete m_broadPhase;
    delete m_collisionDispatcher;
    delete m_collisionConfig;
}

void BulletPhysics::insertEntity(const G3D::Entity *entity) {
    // Add insertions of btRigidBody and btGhostObject differently in this
    // function itself and have factory methods to ease out the implementation
    btCollisionObject *object = nullptr;
    if (auto rigid = dynamic_cast<const G3D::RigidEntity *>(entity)) {
        btRigidBody *body = RigidBodyFactory::create(rigid);
        m_dynamicsWorld->addRigidBody(body);
        object = body;
    } else if (auto marker = dynamic_cast<const G3D::MarkerEntity *>(entity)) {
        btGhostObject *ghost = GhostObjectFactory::create(marker);
        m_dynamicsWorld->addCollisionObject(ghost,
                                            btBroadphaseProxy::StaticFilter);
        m_dynamicsWorld->getBroadphase()
            ->getOverlappingPairCache()
            ->setInternalGhostPairCallback(new btGhostPairCallback());
        object = ghost;
    } else if (auto forceF =
                   dynamic_cast<const G3D::ForceFieldEntity *>(entity)) {
        btGhostObject *ghost = GhostObjectFactory::create(forceF);
        m_dynamicsWorld->addCollisionObject(ghost,
                                            btBroadphaseProxy::StaticFilter);
        m_dynamicsWorld->getBroadphase()
            ->getOverlappingPairCache()
            ->setInternalGhostPairCallback(new btGhostPairCallback());
        object = ghost;
        // BulletPhysics::m_ghostObject = ghost;
        // TODO: fix null
        //applyForceField(entity, nullptr);
    } else {
        // debugAssertM(false, "Unknown G3D entity subclass was passed");
        return;
    }

    m_dynamicBodyMap.insert(std::pair(entity, object));
}

void BulletPhysics::removeEntity(const G3D::Entity *entity) {
    auto iter = m_dynamicBodyMap.find(entity);
    debugAssertM(iter != m_dynamicBodyMap.end(),
                 "Entity to be removed is not present in the map");
    m_dynamicBodyMap.erase(iter);
    // TODO: more stuff for removing the entity from the dynamics world also
    // to be performed here
}

//TODO: supply the force out here
void BulletPhysics::applyForceField(const G3D::Entity *field) {
    auto iter = m_dynamicBodyMap.find(field);
    debugAssertM(iter != m_dynamicBodyMap.end(),
                 "Queried entity is not present in the map");
    auto fieldObject = iter->second;
    auto forcefield = btGhostObject::upcast(fieldObject);

    for (int i = 0; i < forcefield->getNumOverlappingObjects(); i++) {
	auto reactor = forcefield->getOverlappingObject(i);
	btRigidBody *body = btRigidBody::upcast(reactor);
	body->applyCentralForce(btVector3(0, 0, 9));
    }
}

G3D::CoordinateFrame BulletPhysics::getFrame(const G3D::Entity *entity) {
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
void BulletPhysics::setFrame(const G3D::Entity *entity,
                             const G3D::CoordinateFrame frame) {}

void BulletPhysics::applyForce(G3D::Entity *entity, G3D::Point3 force) {
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
