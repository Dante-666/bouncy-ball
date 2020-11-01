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

/** \brief Used by Entity to implement the physical behavior
 * What is a Behavior? It is how the entity is supposed to interact in the
 * Physical realm.
 *
 * How is it implemented? By having a linked list which applies the behavior, if
 * enabled, to the entity
 */

#include "Iterator.h"

namespace G3D {

template <typename E> class BehaviorChain {
    BehaviorChain *next = nullptr;

public:
    friend class Iterator<BehaviorChain>;
    BehaviorChain() = default;
    // Very simple implementation of BehaviorChain class.
    // TODO: add management here where we can add/remove behaviors
    // TODO: insertBehavior
    void addBehavior(BehaviorChain *n) {
        if (next)
            next->addBehavior(n);
        else
            next = n;
    }

    const virtual String getName() const { return "Base"; }

    virtual void apply(E *pEntity) {
        if (next)
            next->apply(pEntity);
    }

    virtual void makeGUI(class GuiPane *pane, class GApp *app) {}

    Iterator<BehaviorChain> begin() const { return Iterator(next); }
    const Iterator<BehaviorChain> end() const {
        return Iterator<BehaviorChain>();
    }

    // TODO: removeBehavior
    void clear() {}
};
} // namespace G3D
