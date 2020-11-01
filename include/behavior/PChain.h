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

/** \brief Used by PhysicsEntity to implement the physical behavior
 * What is a Behavior? It is how the entity is supposed to interact in the
 * Physical realm.
 *
 * How is it implemented? By having a linked list which applies the behavior, if
 * enabled, to the entity
 */

#include "Iterator.h"

namespace G3D {

class PhysicsEntity;

class PropertyChain {
    PropertyChain *next = nullptr;

public:
    friend class Iterator<PropertyChain>;
    PropertyChain() = default;
    // void setNextC(PropertyChain *n) { next = n; }
    // Very simple implementation of PropertyChain class.
    // TODO: add management here where we can add/remove behaviors
    void addProperty(PropertyChain *n) {
        if (next)
            next->addProperty(n);
        else
            next = n;
    }

    const virtual String getName() const { return "Base"; }

    virtual void makeGUI(class GuiPane *pane, class GApp *app) {}

    Iterator<PropertyChain> begin() const { return Iterator(next); }
    const Iterator<PropertyChain> end() const {
        return Iterator<PropertyChain>();
    }

    // TODO: removeBehavior
    void clear() {}
};
} // namespace G3D
