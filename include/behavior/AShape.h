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

#include "G3D-app/GuiDropDownList.h"
#include "PChain.h"

#include "G3D-app/ArticulatedModel.h"
#include "G3D-app/GuiControl.h"
#include "G3D-app/Shape.h"
#include "G3D-base/G3D-base.h"

/** \brief This behavior enables the Opaqueness of the object in the Physics
 * engine. It means if the object reacts to collisions or not. Also requires a
 * collision shape to hold that data.
 */

namespace G3D {
class AShape : public PropertyChain {
    // Add shapes etc.
    // TODO: Explore object construction by deriving shapes directly from Mesh.
    shared_ptr<Shape> m_shape;
    GuiDropDownList *m_shapeDropDownList;
    String m_anySpec;

    void assignShape(const Any &);

public:
    AShape() = default;

    AShape(const AShape &);

    explicit AShape(const Any &);

    AShape &operator=(const Any &);

    Any toAny() const;


    const virtual String getName() const override;

    void setShape(shared_ptr<Shape>);

    const shared_ptr<Shape> getShape() const;

    void shapeSetterCallBack();

    virtual void makeGUI(class GuiPane *pane, class GApp *app) override;
};
} // namespace G3D
