/** Copyright 2020 Blood Eagle Studio
 *
 * You may not use, not distribute and not modify this code
 * under any manifestable possibility and if such a scenario
 * occurs, any changes to the code must be reviewed by the
 * original author of this project.
 *
 *  Author : Siddharth J Singh(dante)
 */

#include "RigidEntity.h"

#include "PhysicsScene.h"

namespace G3D {
RigidEntity::RigidEntity() : VisibleEntity(){};

void RigidEntity::onSimulation(SimTime absoluteTime, SimTime deltaTime) {
    if (!this->canChange() || isBeingEdited)
        return;
    PhysicsScene *physicsScene = dynamic_cast<PhysicsScene *>(m_scene);
    if (physicsScene) {
        setFrame(physicsScene->getPhysicsEngine()->getFrame(this));
    }
}

const void RigidEntity::updatePhysicsFrame() {//const CFrame &frame) {
    if (isBeingEdited)
        return;
    PhysicsScene *physicsScene = dynamic_cast<PhysicsScene *>(m_scene);
    if (physicsScene) {
        physicsScene->getPhysicsEngine()->setFrame(this, this->frame());
    }
}

shared_ptr<Entity> RigidEntity::create(const String &name, Scene *scene,
                                       AnyTableReader &propertyTable,
                                       const ModelTable &modelTable,
                                       const Scene::LoadOptions &loadOptions) {
    // Don't initialize in the constructor, where it is unsafe to throw Any
    // parse exceptions
    shared_ptr<RigidEntity> rigidEntity(new RigidEntity());

    // Initialize each base class, which parses its own fields
    rigidEntity->Entity::init(name, scene, propertyTable);
    rigidEntity->VisibleEntity::init(propertyTable, modelTable);
    rigidEntity->RigidEntity::init(propertyTable);

    // Verify that all fields were read by the base classes
    propertyTable.verifyDone();

    return rigidEntity;
}

shared_ptr<Entity> RigidEntity::create(const String &name, Scene *scene,
                                       const CFrame &position,
                                       const shared_ptr<Model> &model) {
    // Don't initialize in the constructor, where it is unsafe to throw Any
    // parse exceptions
    shared_ptr<RigidEntity> rigidEntity(new RigidEntity());

    // Initialize each base class, which parses its own fields
    rigidEntity->Entity::init(name, scene, position,
                              shared_ptr<Entity::Track>(), true, true);
    rigidEntity->VisibleEntity::init(
        model, true, Surface::ExpressiveLightScatteringProperties(),
        ArticulatedModel::PoseSpline());
    rigidEntity->RigidEntity::init();

    return rigidEntity;
}

void RigidEntity::init(AnyTableReader &propertyTable) {
    // Default expectation of this entity is to cause collisions and it is a
    // part of physical simulation so set those parameters accordingly
    bool c = true;
    propertyTable.setReadStatus("canCauseCollisions", false);
    propertyTable.getIfPresent("canCauseCollisions", c);
    setCanCauseCollisions(c);

    bool p = true;
    propertyTable.getIfPresent("physicalSimulation", p);
    setPhysicalSimulation(p);

    propertyTable.getIfPresent("rollingFriction", m_rollingFriction);

    String collisionShape = "SPHERE";
    propertyTable.getIfPresent("collisionShape", collisionShape);
    m_collisionShape = getShapeTypeFromString(collisionShape);
    switch (m_collisionShape) {
    case Shape::Type::SPHERE: {
        Sphere sphere;
        propertyTable.getIfPresent("shape", sphere);
        m_shape = createShared<SphereShape>(sphere);
    } break;
    case Shape::Type::BOX: {
        Box box;
        propertyTable.getIfPresent("shape", box);
        auto vol = box.volume();
        m_shape = createShared<BoxShape>(box);
    } break;
    case Shape::Type::CYLINDER: {
        Cylinder cylinder;
        propertyTable.getIfPresent("shape", cylinder);
        m_shape = createShared<CylinderShape>(cylinder);
    } break;
    /*case Shape::Type::CAPSULE: {
        Capsule capsule;
        propertyTable.getIfPresent("shape", capsule);
        m_shape = createShared<CapsuleShape>(capsule);
    } break;*/
    case Shape::Type::PLANE: {
        Plane plane;
        propertyTable.getIfPresent("shape", plane);
        m_shape = createShared<PlaneShape>(plane);
    } break;
    case Shape::Type::MESH: {
        const shared_ptr<ArticulatedModel> model =
            dynamic_pointer_cast<G3D::ArticulatedModel>(this->model());
        debugAssertM(model->meshArray().size() == 1,
                     "Attempt to add a model with multiple Meshes");
        ArticulatedModel::Mesh *firstMesh = model->meshArray()[0];
        CPUVertexArray &fullVertex = firstMesh->geometry->cpuVertexArray;

        Array<int> index = firstMesh->cpuIndexArray;
        Array<Point3> vertex;
        for (int i = 0; i < fullVertex.vertex.size(); i++) {
            vertex.push_back(fullVertex.vertex[i].position);
        }
        m_shape = createShared<MeshShape>(vertex, index);
    } break;
    default: {
        debugAssertM(false, "Unknown collisionShape parameter was passed");
    }
    }
}

void RigidEntity::init(Shape::Type collisionShape) {
    m_collisionShape = collisionShape;
    switch (m_collisionShape) {
    case Shape::Type::SPHERE: {
        Sphere sphere(1.0);
        m_shape = createShared<SphereShape>(sphere);
    } break;
    case Shape::Type::BOX: {
        Box box;
        m_shape = createShared<BoxShape>(box);
    } break;
    case Shape::Type::CYLINDER: {
        Cylinder cylinder;
        m_shape = createShared<CylinderShape>(cylinder);
    } break;
    /*case Shape::Type::CAPSULE: {
        Capsule capsule;
        m_shape = createShared<CapsuleShape>(capsule);
    } break;*/
    case Shape::Type::PLANE: {
        Plane plane;
        m_shape = createShared<PlaneShape>(plane);
    } break;
    case Shape::Type::MESH: {
        // TODO: this needs to be handled a bit more properly
        // maybe use a MeshShape?
    } break;
    default: {
        debugAssertM(false, "Unknown collisionShape parameter was passed");
    }
    }
}

const Shape::Type RigidEntity::getShapeTypeFromString(const String shape) {
    if (shape == "SPHERE") {
        return Shape::Type::SPHERE;
    } else if (shape == "BOX") {
        return Shape::Type::BOX;
    } else if (shape == "CYLINDER") {
        return Shape::Type::CYLINDER;
    } else if (shape == "CAPSULE") {
        return Shape::Type::CAPSULE;
    } else if (shape == "PLANE") {
        return Shape::Type::PLANE;
    } else if (shape == "MESH") {
        return Shape::Type::MESH;
    } else {
        alwaysAssertM(false, "invalid shape provided");
        return Shape::Type::NONE;
    }
}

} // namespace G3D
