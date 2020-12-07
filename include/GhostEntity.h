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

/** \brief Derived from a MarkerEntity
 * Designed to behave like a trigger sort of entity which can be used to obtain
 * the entities overlapping this bounding box.
 */

#include "G3D-app/ArticulatedModel.h"
#include "G3D-app/G3D-app.h"
#include "G3D-base/G3D-base.h"
#include "G3D-gfx/G3D-gfx.h"

#include "behavior/BChain.h"
#include "behavior/PChain.h"

#include "behavior/AShape.h"

#include "behavior/Attractor.h"
#include "behavior/ForceField.h"

#include <memory>

namespace G3D {

class PhysicsScene;

class GhostEntity : public MarkerEntity, public PropertyChain {

protected:
    PhysicsScene *m_physicsScene;

public:
    GhostEntity();

    void init(AnyTableReader &propertyTable);

    virtual Any toAny(const bool forceAll) const override;
};
} // namespace G3D
