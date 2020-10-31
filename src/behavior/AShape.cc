#include "behavior/AShape.h"

#include "PhysicsEntity.h"
#include "PhysicsScene.h"

namespace G3D {

void AShape::assignShape(const Any &any) {
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
}

AShape::AShape(const AShape &opq) { m_shape = opq.m_shape; }

AShape::AShape(const Any &any) {
    *this = AShape();
    assignShape(any);
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
    return any;
}

AShape &AShape::operator=(const Any &a) { return *this = AShape(a); }

const String AShape::getName() const { return "AShape"; }

void AShape::setShape(shared_ptr<Shape> shape) { m_shape = shape; }

const shared_ptr<Shape> AShape::getShape() const { return m_shape; }

void AShape::shapeSetterCallBack() {
    auto any = Any::parse(m_anySpec);
    assignShape(any);
}

void AShape::makeGUI(GuiPane *pane, GApp *app) {
    auto any = toAny();
    m_anySpec = any.unparse();
    auto multiLineTextBox = pane->addMultiLineTextBox("Any Spec", &m_anySpec);
    multiLineTextBox->setWidth(240);
    multiLineTextBox->setHeight(100);
    pane->addButton("Apply",
                    GuiControl::Callback(this, &AShape::shapeSetterCallBack),
                    GuiTheme::NORMAL_BUTTON_STYLE);
}
} // namespace G3D
