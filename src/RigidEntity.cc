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
#include "G3D-app/ArticulatedModel.h"
#include "PhysicsScene.h"
#include <memory>

namespace G3D {
RigidEntity::RigidEntity() : VisibleEntity(){};

void RigidEntity::onSimulation(SimTime absoluteTime, SimTime deltaTime) {
    if (!this->canChange())
        return;
    PhysicsScene *physicsScene = dynamic_cast<PhysicsScene *>(m_scene);
    if (physicsScene) {
        updateFrame(physicsScene->getPhysicsEngine()->getFrame(this));
    }
}

void RigidEntity::updateFrame(CoordinateFrame frame) {
    m_previousFrame = m_frame;
    m_frame = frame;
    debugAssert(m_frame.rotation.isOrthonormal());

    // An object that moves farther than this in one frame is assumed to
    // have teleported and will be assigned zero velocity
    const float MAX_DISTANCE = 20.0f;

    if ((m_previousFrame.translation - m_frame.translation).squaredLength() >
        square(MAX_DISTANCE)) {
        // Teleport
        m_previousFrame = m_frame;
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
    case Shape::Type::MESH: {
        // TODO: Create a MeshShape to hold the coordinates and pass this as
        // data to the physics engine
        const shared_ptr<ArticulatedModel> model =
            dynamic_pointer_cast<G3D::ArticulatedModel>(this->model());
        debugAssertM(model->meshArray().size() == 1,
                     "Attempt to add a model with multiple Meshes");
	ArticulatedModel::Mesh* firstMesh = model->meshArray()[0];
        CPUVertexArray &fullVertex = firstMesh->geometry->cpuVertexArray;

	Array<int> index = firstMesh->cpuIndexArray;
        Array<Point3> vertex;
        for (int i = 0; i < fullVertex.vertex.size(); i++) {
            vertex.push_back(fullVertex.vertex[i].position);
        }
        m_shape =
            createShared<MeshShape>(vertex, index);
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
    } else if (shape == "MESH") {
        return Shape::Type::MESH;
    } else {
        alwaysAssertM(false, "invalid shape provided");
        return Shape::Type::NONE;
    }
}

} // namespace G3D
