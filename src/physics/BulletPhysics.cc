/** Copyright 2020 Blood Eagle Studio
 *
 * You may not use, not distribute and not modify this code
 * under any manifestable possibility and if such a scenario
 * occurs, any changes to the code must be reviewed by the
 * original author of this project.
 *
 *  Author : Siddharth J Singh(dante)
 */

#include "physics/BulletPhysics.h"

#include "Constraint.h"
#include "G3D-app/Entity.h"
#include "GhostEntity.h"
#include "PhysicsEntity.h"

#include "G3D-base/Vector3.h"

//#include "behavior/Behavior.h"

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
    if (auto physics = dynamic_cast<const G3D::PhysicsEntity *>(entity)) {
        btRigidBody *body = PhysicsBodyFactory::create(physics);
        m_dynamicsWorld->addRigidBody(body);
        object = body;
    } else if (auto gEntity = dynamic_cast<const G3D::GhostEntity *>(entity)) {
        btGhostObject *ghost = GhostObjectFactory::create(gEntity);
        m_dynamicsWorld->addCollisionObject(ghost,
                                            btBroadphaseProxy::StaticFilter);
        m_dynamicsWorld->getBroadphase()
            ->getOverlappingPairCache()
            ->setInternalGhostPairCallback(new btGhostPairCallback());
        object = ghost;
    } else {
        // debugAssertM(false, "Unknown G3D entity subclass was passed");
        return;
    }

    m_dynamicBodyMap.insert({entity, object});
}

void BulletPhysics::removeEntity(const G3D::Entity *entity) {
    auto iter = m_dynamicBodyMap.left.find(entity);
    debugAssertM(iter != m_dynamicBodyMap.left.end(),
                 "Entity to be removed is not present in the map");
    m_dynamicBodyMap.left.erase(entity);

    if (auto ghost = btGhostObject::upcast(iter->get_right())) {
        m_dynamicsWorld->removeCollisionObject(ghost);
    } else if (auto body = btRigidBody::upcast(iter->get_right())) {
        m_dynamicsWorld->removeRigidBody(body);
    }
}

void BulletPhysics::applyForceField(const G3D::Entity *field,
                                    const G3D::Vector3 force,
                                    const FieldType type) {
    auto iter = m_dynamicBodyMap.left.find(field);
    debugAssertM(iter != m_dynamicBodyMap.left.end(),
                 "Queried entity is not present in the map");
    auto fieldObject = iter->get_right();
    auto forcefield = btGhostObject::upcast(fieldObject);

    btTransform trans = forcefield->getWorldTransform();

    for (int i = 0; i < forcefield->getNumOverlappingObjects(); i++) {
        auto reactor = forcefield->getOverlappingObject(i);
        btRigidBody *body = btRigidBody::upcast(reactor);
        if (body) {
            if (type == DIRECTIONAL) {
                body->applyCentralForce(trans.getBasis() *
                                        Vector::convert(force));
            } else if (type == RADIAL) {
                /*auto reactT = body->getWorldTransform();
                auto attractPos = btVector3(12, 15, 0);
                auto reactPos = reactT.getOrigin();

                auto dirVec = attractPos - reactPos;
                auto radialDamp = fmax(0.002, pow(dirVec.length(), 2));
                auto appliedForce =
                    force.length() * dirVec.normalize() * radialDamp;
                body->applyCentralForce(force.length() * dirVec.normalize() *
                                        radialDamp);*/
                // auto constraint = btGeneric6DofConstraint();
            }
        }
    }
}

G3D::Constraint *const
BulletPhysics::addConstraint(const G3D::Entity *entityA,
                             const G3D::Entity *entityB) {
    auto iterA = m_dynamicBodyMap.left.find(entityA);
    debugAssertM(iterA != m_dynamicBodyMap.left.end(),
                 "Queried entity is not present in the map");
    auto iterB = m_dynamicBodyMap.left.find(entityB);
    debugAssertM(iterB != m_dynamicBodyMap.left.end(),
                 "Queried entity is not present in the map");
    auto matrix = btMatrix3x3();
    auto trans = btTransform();
    trans.setIdentity();

    btRigidBody *rigidBodyA = btRigidBody::upcast(iterA->get_right());
    btRigidBody *rigidBodyB = btRigidBody::upcast(iterB->get_right());

    auto constraint = new btGeneric6DofConstraint(*rigidBodyA, *rigidBodyB,
                                                  trans, trans, true);
    m_dynamicsWorld->addConstraint(constraint, true);

    auto g3dConstraint = new G3D::Constraint();
    g3dConstraint->setType(G3D::Constraint::TYPE::GENERIC6DOF);

    m_constraintMap.insert({g3dConstraint, constraint});

    return g3dConstraint;
}

void BulletPhysics::removeConstraint(const G3D::Constraint *const constraint) {
    auto iter = m_constraintMap.find(constraint);
    debugAssertM(iter != m_constraintMap.end(),
                 "Queried constraint is not present in the map");

    auto pConstraint = iter->second;
    m_dynamicsWorld->removeConstraint(pConstraint);

    m_constraintMap.erase(iter);

    delete pConstraint;
    delete constraint;
}

const G3D::Entity *const
BulletPhysics::getPrimaryCollider(const G3D::Entity *field) {
    auto iter = m_dynamicBodyMap.left.find(field);
    debugAssertM(iter != m_dynamicBodyMap.left.end(),
                 "Queried entity is not present in the map");
    auto fieldObject = iter->get_right();
    auto forcefield = btGhostObject::upcast(fieldObject);

    btTransform trans = forcefield->getWorldTransform();

    for (int i = 0; i < forcefield->getNumOverlappingObjects(); i++) {
        auto reactor = forcefield->getOverlappingObject(i);
        if (reactor && fieldObject->checkCollideWith(reactor)) {
            if (auto body = btRigidBody::upcast(reactor)) {
                auto iter = m_dynamicBodyMap.right.find(body);
                debugAssertM(iter != m_dynamicBodyMap.right.end(),
                             "Queried body is not present in the map");
                return iter->get_left();
            }
        }
    }
    return nullptr;
}

void BulletPhysics::ignoreCollisionCheck(const G3D::Entity *trigger,
                                         const G3D::Entity *collider) {
    auto trigIter = m_dynamicBodyMap.left.find(trigger);
    debugAssertM(trigIter != m_dynamicBodyMap.left.end(),
                 "Queried entity is not present in the map");
    auto colIter = m_dynamicBodyMap.left.find(collider);
    debugAssertM(trigIter != m_dynamicBodyMap.left.end(),
                 "Queried entity is not present in the map");

    auto triggerCO = trigIter->get_right();
    if (triggerCO->checkCollideWith(colIter->get_right())) {
        triggerCO->setIgnoreCollisionCheck(colIter->get_right(), true);
    }
}

G3D::CoordinateFrame BulletPhysics::getFrame(const G3D::Entity *entity) {
    auto iter = m_dynamicBodyMap.left.find(entity);
    debugAssertM(iter != m_dynamicBodyMap.left.end(),
                 "Queried entity is not present in the map");
    auto collisionObject = iter->get_right();
    btRigidBody *rigidBody = btRigidBody::upcast(collisionObject);

    btTransform trans;
    if (rigidBody && rigidBody->getMotionState()) {
        rigidBody->getMotionState()->getWorldTransform(trans);
    } else {
        trans = collisionObject->getWorldTransform();
    }

    return Frame::convert(trans);
}
void BulletPhysics::reconstructRigidBody(const G3D::Entity *entity) {
    auto iter = m_dynamicBodyMap.left.find(entity);
    debugAssertM(iter != m_dynamicBodyMap.left.end(),
                 "Queried entity is not present in the map");
    auto collisionObject = iter->get_right();
    btRigidBody *rigidBody = btRigidBody::upcast(collisionObject);

    m_dynamicsWorld->removeRigidBody(rigidBody);

    // delete this pointer
    delete rigidBody;
    if (auto rigid = dynamic_cast<const G3D::PhysicsEntity *>(entity)) {
        btRigidBody *body = PhysicsBodyFactory::create(rigid);
        m_dynamicsWorld->addRigidBody(body);

        m_dynamicBodyMap.left.replace_data(iter, body);
    }
}

void BulletPhysics::applyForce(G3D::Entity *entity, G3D::Point3 force) {
    auto iter = m_dynamicBodyMap.left.find(entity);
    debugAssertM(iter != m_dynamicBodyMap.left.end(),
                 "Queried entity is not present in the map");
    auto collisionObject = iter->get_right();
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
