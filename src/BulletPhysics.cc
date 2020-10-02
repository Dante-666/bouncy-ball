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
#include "BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h"
#include "LinearMath/btDefaultMotionState.h"
#include "G3D-app/ArticulatedModel.h"
#include "G3D-app/Shape.h"
#include "G3D-app/VisibleEntity.h"
#include "G3D-gfx/CPUVertexArray.h"
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
    m_staticBodySet.clear();
    m_dynamicBodyMap.clear();
    m_collisionShapes.clear();
    delete m_dynamicsWorld;
    delete m_solver;
    delete m_broadPhase;
    delete m_collisionDispatcher;
    delete m_collisionConfig;
}

void BulletPhysics::insertRigidEntity(G3D::RigidEntity *entity) {
    debugAssertM(
        entity->canCauseCollisions(),
        "Attempt to include a RigidEntity which cannot cause collisions");

    G3D::Shape::Type collisionShape = entity->m_collisionShape;
    btCollisionShape *colShape;
    // TODO: break this down into a separate function
    switch (collisionShape) {
    case G3D::Shape::Type::SPHERE: {
        shared_ptr<G3D::SphereShape> shape =
            dynamic_pointer_cast<G3D::SphereShape>(entity->m_shape);
        float area = shape->area();
        btScalar radius = sqrt(area / (4 * M_PI));

        colShape = new btSphereShape(radius);
    } break;
    case G3D::Shape::Type::MESH: {
        shared_ptr<G3D::MeshShape> shape =
            dynamic_pointer_cast<G3D::MeshShape>(entity->m_shape);

        btIndexedMesh *mesh = new btIndexedMesh();
        mesh->m_numTriangles = shape->indexArray().size() / 3;
        mesh->m_triangleIndexBase =
            (const unsigned char *)shape->indexArray().getCArray();
        mesh->m_triangleIndexStride = 3 * 4;
        mesh->m_numVertices = shape->vertexArray().size();
        mesh->m_vertexBase =
            (const unsigned char *)shape->vertexArray().getCArray();
        mesh->m_vertexStride = 3 * 4;

        btTriangleIndexVertexArray *vertexArray =
            new btTriangleIndexVertexArray();
        vertexArray->addIndexedMesh(*mesh);
        colShape = new btBvhTriangleMeshShape(vertexArray, false);
    } break;
    default:
        debugAssertM(false, "Type not implemented yet");
    }

    // TODO: is this needed?
    m_collisionShapes.push_back(colShape);

    // Create Dynamic Objects
    btTransform startTransform = convertFromG3D(entity->frame());
    btScalar mass = entity->mass();

    // rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f) || entity->canChange();

    // TODO: read from Any file as well
    btVector3 localInertia = btVector3(0, 0, 0);
    if (isDynamic)
        colShape->calculateLocalInertia(mass, localInertia);

    // using motionstate is recommended, it provides interpolation
    // capabilities, and only synchronizes 'active' objects
    btDefaultMotionState *motionState =
        new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, colShape,
                                                    localInertia);
    btRigidBody *body = new btRigidBody(rbInfo);
    body->setRollingFriction(entity->m_rollingFriction);

    m_dynamicsWorld->addRigidBody(body);

    if (isDynamic) {
        m_dynamicBodyMap.insert(std::pair(entity, body));
    } else {
        m_staticBodySet.insert(body);
    }
}

void BulletPhysics::removeRigidEntity(G3D::RigidEntity *entity) {
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

    return convertToG3D(trans);
}

void BulletPhysics::applyForce(G3D::VisibleEntity *entity, G3D::Point3 force) {
    auto iter = m_dynamicBodyMap.find(entity);
    debugAssertM(iter != m_dynamicBodyMap.end(),
                 "Queried entity is not present in the map");
    auto collisionObject = iter->second;
    btRigidBody *rigidBody = btRigidBody::upcast(collisionObject);
    rigidBody->applyCentralForce(convertFromG3D(force));
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

const inline G3D::CoordinateFrame
BulletPhysics::convertToG3D(const btTransform transform) {
    G3D::Matrix3 rotation = convertToG3D(transform.getBasis());
    G3D::Point3 translation = convertToG3D(transform.getOrigin());
    return G3D::CoordinateFrame(rotation, translation);
}

const inline G3D::Matrix3
BulletPhysics::convertToG3D(const btMatrix3x3 matrix) {
    G3D::Vector3 rows[3];
    for (int i = 0; i < 3; i++) {
        rows[i] = convertToG3D(matrix[i]);
    }
    return G3D::Matrix3::fromRows(rows[0], rows[1], rows[2]);
}

const inline G3D::Point3 BulletPhysics::convertToG3D(const btVector3 vector) {
    return G3D::Point3(vector.x(), vector.y(), vector.z());
}

const inline btTransform
BulletPhysics::convertFromG3D(const G3D::CoordinateFrame frame) {
    btMatrix3x3 basis = convertFromG3D(frame.rotation);
    btVector3 origin = convertFromG3D(frame.translation);
    return btTransform(basis, origin);
}

const inline btMatrix3x3
BulletPhysics::convertFromG3D(const G3D::Matrix3 matrix) {
    btVector3 rows[3];
    for (int i = 0; i < 3; i++) {
        rows[i] = convertFromG3D(matrix.row(i));
    }
    return btMatrix3x3(rows[0], rows[1], rows[2]);
}

const inline btVector3
BulletPhysics::convertFromG3D(const G3D::Vector3 vector) {
    return btVector3(vector.x, vector.y, vector.z);
}
