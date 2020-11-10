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

#include "G3D-base/G3D-base.h"

/** \brief
 */

namespace G3D {
template <typename E> class ForceField : public BehaviorChain<E> {

protected:
    Vector3 m_force;

public:
    ForceField() = default;

    ForceField(const ForceField &);

    explicit ForceField(const Any &);

    ForceField &operator=(const Any &);

    Any toAny() const;

    const virtual String getName() const override;

    virtual void apply(E *pEntity) override;
};
} // namespace G3D
