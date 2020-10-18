/** Copyright 2020 Blood Eagle Studio
 *
 * You may not use, not distribute and not modify this code
 * under any manifestable possibility and if such a scenario
 * occurs, any changes to the code must be reviewed by the
 * original author of this project.
 *
 *  Author : Siddharth J Singh(dante)
 */

#include "physics/BulletFactory.h"
#include "G3D-app/Shape.h"

#include "GhostEntity.h"
#include "PhysicsEntity.h"

#include "behavior/AShape.h"
#include "behavior/PChain.h"
#include "behavior/Solid.h"

btCollisionShape *CollisionShapeFactory::create(const G3D::AShape *ashape) {
    shared_ptr<G3D::Shape> gShape = ashape->getShape();
    debugAssertM(gShape, "shape was not created.");
    auto type = ashape->getShape()->type();
    switch (type) {
    case G3D::Shape::Type::SPHERE: {
        shared_ptr<G3D::SphereShape> shape =
            dynamic_pointer_cast<G3D::SphereShape>(gShape);
        float area = shape->area();
        btScalar radius = sqrt(area / (4 * M_PI));

        return new btSphereShape(radius);
    } break;
    case G3D::Shape::Type::BOX: {
        shared_ptr<G3D::BoxShape> shape =
            dynamic_pointer_cast<G3D::BoxShape>(gShape);
        G3D::Vector3 corner = shape->box().corner(7);

        return new btBoxShape(Vector::convert(corner));
    } break;
    case G3D::Shape::Type::CYLINDER: {
        shared_ptr<G3D::CylinderShape> shape =
            dynamic_pointer_cast<G3D::CylinderShape>(gShape);
        float area = shape->area();

        return new btCylinderShape(btVector3(1, 1, 1));
    } break;
    case G3D::Shape::Type::PLANE: {
        shared_ptr<G3D::PlaneShape> shape =
            dynamic_pointer_cast<G3D::PlaneShape>(gShape);
        btVector3 pNormal = btVector3(0, 1, 0);
        btScalar pConst = 100;
        return new btStaticPlaneShape(pNormal, pConst);

    } break;
    case G3D::Shape::Type::MESH: {
        shared_ptr<G3D::MeshShape> shape =
            dynamic_pointer_cast<G3D::MeshShape>(gShape);
        // TODO: this creates a problem #14
        // G3D::AABox box = shape->boundingAABox();

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
        return new btBvhTriangleMeshShape(vertexArray, false);
    } break;

    default:
        debugAssertM(false, "Type not implemented yet");
    }

    return new btSphereShape(1.f);
}

btRigidBody *PhysicsBodyFactory::create(const G3D::PhysicsEntity *pEntity) {
    const G3D::PropertyChain *link =
        dynamic_cast<const G3D::PropertyChain *>(pEntity);

    btCollisionShape *colShape = nullptr;
    while (link) {
        if (link->getName() == "AShape") {
            colShape = CollisionShapeFactory::create(
                dynamic_cast<const G3D::AShape *>(link));
            break;
        } else if (link->getName() == "Base") {
        }
        link = link->getNext();
    }
    // btCollisionShape *colShape = new btSphereShape(1.0);

    btScalar mass = pEntity->mass();

    // pEntitybody is dynamic if and only if mass is non zero, otherwise
    // static
    bool isDynamic = (mass != 0.f) || pEntity->canChange();

    // TODO: read from Any file as well
    btVector3 localInertia = btVector3(0, 0, 0);
    if (isDynamic)
        colShape->calculateLocalInertia(mass, localInertia);

    // using motionstate is recommended, it provides interpolation
    // capabilities, and only synchronizes 'active' objects
    btDefaultMotionState *motionState =
        new btDefaultMotionState(Frame::convert(pEntity->frame()));
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, colShape,
                                                    localInertia);

    btRigidBody *body = new btRigidBody(rbInfo);

    link = dynamic_cast<const G3D::PropertyChain *>(pEntity);
    while (link) {
        if (link->getName() == "Solid") {
            const G3D::Solid *solid = dynamic_cast<const G3D::Solid *>(link);
            body->setRollingFriction(solid->getRollingFriction());
            break;
        } else if (link->getName() == "Base") {
        }
        link = link->getNext();
    }

    return body;
}

btGhostObject *GhostObjectFactory::create(const G3D::GhostEntity *gEntity) {
    btGhostObject *ghost = new btGhostObject();
    const G3D::PropertyChain *link =
        dynamic_cast<const G3D::PropertyChain *>(gEntity);
    btCollisionShape *colShape = nullptr;
    while (link) {
        if (link->getName() == "AShape") {
            colShape = CollisionShapeFactory::create(
                dynamic_cast<const G3D::AShape *>(link));
            break;
        } else if (link->getName() == "Base") {
        }
        link = link->getNext();
    }
    ghost->setCollisionShape(colShape);
    ghost->setWorldTransform(Frame::convert(gEntity->frame()));
    ghost->setCollisionFlags(
        btCollisionObject::CollisionFlags::CF_STATIC_OBJECT |
        btCollisionObject::CollisionFlags::CF_NO_CONTACT_RESPONSE);

    return ghost;
}

const btTransform Frame::convert(G3D::CoordinateFrame frame) {
    btMatrix3x3 basis = Matrix::convert(frame.rotation);
    btVector3 origin = Vector::convert(frame.translation);

    return btTransform(basis, origin);
}

const G3D::CoordinateFrame Frame::convert(const btTransform transform) {
    G3D::Matrix3 rotation = Matrix::convert(transform.getBasis());
    G3D::Point3 translation = Vector::convert(transform.getOrigin());
    return G3D::CoordinateFrame(rotation, translation);
}

const G3D::Matrix3 Matrix::convert(const btMatrix3x3 matrix) {
    G3D::Vector3 rows[3];
    for (int i = 0; i < 3; i++) {
        rows[i] = Vector::convert(matrix[i]);
    }
    return G3D::Matrix3::fromRows(rows[0], rows[1], rows[2]);
}

const btMatrix3x3 Matrix::convert(const G3D::Matrix3 matrix) {
    btVector3 rows[3];
    for (int i = 0; i < 3; i++) {
        rows[i] = Vector::convert(matrix.row(i));
    }
    return btMatrix3x3(rows[0], rows[1], rows[2]);
}

const G3D::Point3 Vector::convert(const btVector3 vector) {
    return G3D::Point3(vector.x(), vector.y(), vector.z());
}

const btVector3 Vector::convert(const G3D::Vector3 vector) {
    return btVector3(vector.x, vector.y, vector.z);
}
