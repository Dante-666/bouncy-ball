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

#include "ForceField.h"

#include "G3D-base/G3D-base.h"
#include "PhysicsEntity.h"

/** \brief
 */

namespace G3D {
template <typename E> class Attractor : public ForceField<E> {
    //shared_ptr<PhysicsEntity> m_dummy;

public:
    Attractor() = default;

    Attractor(const Attractor &);

    explicit Attractor(const Any &);

    Attractor &operator=(const Any &);

    Any toAny() const;

    const virtual String getName() const override;

    virtual void apply(E *pEntity) override;
};
} // namespace G3D
