#include "behavior/AShape.h"

#include "PhysicsEntity.h"
#include "PhysicsScene.h"

namespace G3D {

AShape::AShape(const AShape &opq) {
    m_shape = opq.m_shape;
    m_rollingFriction = opq.m_rollingFriction;
}

AShape::AShape(const Any &any) {
    *this = AShape();
    any.verifyName("AShape");
    AnyTableReader r(any);
    if (any.containsKey("sphere")) {
        Sphere sphere;
        if (r.getIfPresent("sphere", sphere))
            m_shape = std::make_shared<SphereShape>(sphere);
    }
    if (any.containsKey("box")) {
        Box box;
        if (r.getIfPresent("box", box))
            m_shape = std::make_shared<BoxShape>(box);
    }
    if (any.containsKey("cylinder")) {
        Cylinder cylinder;
        if (r.getIfPresent("cylinder", cylinder))
            m_shape = std::make_shared<CylinderShape>(cylinder);
    }
    if (any.containsKey("plane")) {
        Plane plane;
        if (r.getIfPresent("plane", plane))
            m_shape = std::make_shared<PlaneShape>(plane);
    }

    r.getIfPresent("rollingFriction", m_rollingFriction);
}

Any AShape::toAny() const {
    Any any(Any::TABLE, "AShape");
    switch (m_shape->type()) {
    case Shape::Type::SPHERE: {
        any["sphere"] = m_shape->sphere();
        break;
    }
    case Shape::Type::BOX: {
        any["box"] = m_shape->box();
        break;
    }
    case Shape::Type::CYLINDER: {
        any["cylinder"] = m_shape->cylinder();
        break;
    }
    case Shape::Type::PLANE: {
        any["plane"] = m_shape->plane();
        break;
    }
    }
    any["rollingFriction"] = m_rollingFriction;
    return any;
}

const String AShape::getName() const { return "AShape"; }

void AShape::setShape(shared_ptr<Shape> shape) { m_shape = shape; }

const shared_ptr<Shape> AShape::getShape() const { return m_shape; }

} // namespace G3D
