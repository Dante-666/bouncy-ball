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
namespace G3D {

template<typename E>
class BehaviorChain {
    BehaviorChain *next = nullptr; // 1. "next" pointer in the base class
public:
    BehaviorChain() = default; 
    // Very simple implementation of BehaviorChain class.
    // TODO: add management here where we can add/remove behaviors
    // TODO: removeBehavior, insertBehavior
    void addBehavior(BehaviorChain *n) {
        if (next)
            next->addBehavior(n);
        else
            next = n;
    }

    // TODO: Make this to Entity
    virtual void apply(E *pEntity) {
        if (next)
            next->apply(pEntity);
    }
};
} // namespace G3D
