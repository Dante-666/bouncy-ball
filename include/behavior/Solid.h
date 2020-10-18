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

#include "PChain.h"

#include "G3D-app/ArticulatedModel.h"
#include "G3D-app/Shape.h"
#include "G3D-base/G3D-base.h"

/** \brief This behavior enables the Opaqueness of the object in the Physics
 * engine. It means if the object reacts to collisions or not. Also requires a
 * collision shape to hold that data.
 */

namespace G3D {
class Solid : public PropertyChain {
    float m_rollingFriction = 0.3;
    bool m_phased = false;
public:
    Solid() : PropertyChain() {}

    Solid(const Solid &opq);

    explicit Solid(const Any &any);

    Solid &operator=(const Any &a) { return *this = Solid(a); }

    Any toAny() const;

    const virtual String getName() const override;

    const float getRollingFriction() const;
};
} // namespace G3D
