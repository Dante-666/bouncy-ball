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

#include "G3D-app/VisibleEntity.h"

/**
 * This is supposed to be a pure virtual class in which people must be able to
 * adda collisionshape and should have no ties with the physics engine itself.
 * The moment you make that, it should be done in a subclass called
 * BulletFactory let's say. The typename is the EngineObject which one wishes
 * to create */
template <typename EO> class CollisionFactory {
public:
    static EO *create(G3D::VisibleEntity *);
};

/**
 * These are useful data conversion utilities which should work based on type
 * deduction and are generic */
template <typename ED, typename GD> class DataFactory {
public:
static const inline ED convert(const GD);
static const inline GD convert(const ED);
};
/*template <typename GD, typename ED> class convertTo {};*/
