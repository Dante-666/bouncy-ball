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

#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h"
#include "BulletCollision/CollisionShapes/btCylinderShape.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletCollision/CollisionShapes/btStaticPlaneShape.h"
#include "BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "LinearMath/btDefaultMotionState.h"

#include "G3D-app/VisibleEntity.h"
#include "G3D-base/Matrix3x4.h"
#include "G3D-base/Vector3.h"
#include "PhysicsFactory.h"

/**
 * Implementation for bullet */
class ColShapeFactory : public CollisionFactory<btCollisionShape> {
public:
    static btCollisionShape *create(G3D::VisibleEntity *);
};

class RigidBodyFactory : public CollisionFactory<btRigidBody> {
public:
    static btRigidBody *create(G3D::VisibleEntity *);
};

template class DataFactory<btTransform, G3D::CoordinateFrame>;

/**
 * Cannot inline a few of these methods because of use withing two classes
 * apparantly
 */
class Frame {
public:
    static const btTransform convert(G3D::CoordinateFrame);
    static const G3D::CoordinateFrame convert(btTransform);
};

class Matrix : public DataFactory<btMatrix3x3, G3D::Matrix3> {
public:
    static const btMatrix3x3 convert(G3D::Matrix3);
    static const G3D::Matrix3 convert(btMatrix3x3);
};

class Vector : public DataFactory<btVector3, G3D::Vector3> {
public:
    static const btVector3 convert(G3D::Vector3);
    static const G3D::Vector3 convert(btVector3);
};
