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

    // TODO: remove this ground shape and add one from G3D itself
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
    debugAssertM(
        entity->canCauseCollisions(),
        "Attempt to include a RigidEntity which cannot cause collisions");

    G3D::String collisionShape = entity->getCollisionShape();
    btCollisionShape *colShape;
    if (collisionShape == "Sphere") {
        // TODO: remove the G3D shapes as they do a lot more and require
        // extra computation load while addition but the structure is still okay
        shared_ptr<G3D::SphereShape> shape =
            dynamic_pointer_cast<G3D::SphereShape>(entity->getShape());
        float area = shape->area();
        btScalar radius = sqrt(area / (4 * M_PI));

        colShape = new btSphereShape(radius);
        // TODO: is this needed?
        m_collisionShapes.push_back(colShape);
    }

    // Create Dynamic Objects
    btTransform startTransform = convertFromG3D(entity->frame());
    btScalar mass = entity->mass();

    // TODO: read from Any file as well
    btVector3 localInertia = btVector3(0, 0, 0);
    colShape->calculateLocalInertia(mass, localInertia);

    // using motionstate is recommended, it provides interpolation capabilities,
    // and only synchronizes 'active' objects
    btDefaultMotionState *motionState =
        new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, colShape,
                                                    localInertia);
    btRigidBody *body = new btRigidBody(rbInfo);

    m_dynamicsWorld->addRigidBody(body);
    
    //Add this data to the map
    m_render2physicsMap.insert(std::pair(entity, body));
}

void BulletPhysics::removeRigidEntity(G3D::RigidEntity *entity) {
    auto iter = m_render2physicsMap.find(entity);
    debugAssertM(iter != m_render2physicsMap.end(),
                 "Entity to be removed is not present in the map");
    m_render2physicsMap.erase(iter);
    // TODO: more stuff for removing the entity from the dynamics world also to
    // be performed here
}

G3D::CoordinateFrame BulletPhysics::getFrame(G3D::VisibleEntity *entity) {
    auto iter = m_render2physicsMap.find(entity);
    debugAssertM(iter != m_render2physicsMap.end(),
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

void BulletPhysics::simulate(float deltaTime) {
    m_dynamicsWorld->stepSimulation(deltaTime);
}

G3D::CoordinateFrame BulletPhysics::convertToG3D(const btTransform transform) {
    G3D::Matrix3 rotation = convertToG3D(transform.getBasis());
    G3D::Point3 translation = convertToG3D(transform.getOrigin());
    return G3D::CoordinateFrame(rotation, translation);
}

G3D::Matrix3 BulletPhysics::convertToG3D(const btMatrix3x3 matrix) {
    G3D::Vector3 rows[3];
    for (int i = 0; i < 3; i++) {
        rows[i] = convertToG3D(matrix[i]);
    }
    return G3D::Matrix3::fromRows(rows[0], rows[1], rows[2]);
}

G3D::Point3 BulletPhysics::convertToG3D(const btVector3 vector) {
    return G3D::Point3(vector.x(), vector.y(), vector.z());
}

btTransform BulletPhysics::convertFromG3D(const G3D::CoordinateFrame frame) {
    btMatrix3x3 basis = convertFromG3D(frame.rotation);
    btVector3 origin = convertFromG3D(frame.translation);
    return btTransform(basis, origin);
}
btMatrix3x3 BulletPhysics::convertFromG3D(const G3D::Matrix3 matrix) {
    btVector3 rows[3];
    for (int i = 0; i < 3; i++) {
        rows[i] = convertFromG3D(matrix.row(i));
    }
    return btMatrix3x3(rows[0], rows[1], rows[2]);
}
btVector3 BulletPhysics::convertFromG3D(const G3D::Vector3 vector) {
    return btVector3(vector.x, vector.y, vector.z); 
}
