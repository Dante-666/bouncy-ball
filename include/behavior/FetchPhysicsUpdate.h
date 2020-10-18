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

#include "BChain.h"

#include "G3D-app/ArticulatedModel.h"
#include "G3D-app/Shape.h"
#include "G3D-base/G3D-base.h"

/** \brief
 */

namespace G3D {
template <typename E> class FetchPhysicsUpdate : public BehaviorChain<E> {

public:
    FetchPhysicsUpdate() = default;

    virtual void apply(E *pEntity) override;
};
} // namespace G3D
