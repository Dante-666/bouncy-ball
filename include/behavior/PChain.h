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

#include "G3D-base/G3DString.h"
#include "G3D-base/debug.h"

namespace G3D {

class PhysicsEntity;

class PropertyChain {
    PropertyChain *next = nullptr;

public:
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
    // Property always may have to be constructed and has to do with creation
    // deletion
    // TODO: Make this to Entity
    // TODO: Deprecate
    const PropertyChain *getNext() const { return next; }

    class Iterator {
        PropertyChain *node;
        bool isDone = false;

    public:
        Iterator(PropertyChain *node = nullptr) { this->node = node; }
        bool isOver() { return isDone; }
        bool hasNext() { return node->next != nullptr; }
        void advance() {
            if (hasNext()) {
                node = node->next;
            } else {
                isDone = true;
            }
        }

        /*inline bool operator!=(const Iterator &other) const {
            return !(*this == other);
        }*/

        PropertyChain *operator->() const {
            debugAssert(isValidHeapPointer(node));
            return node;
        }

        PropertyChain *operator*() const {
            debugAssert(isValidHeapPointer(node));
            return node;
        }
    };

    Iterator begin() const { return Iterator(next); }
    const Iterator end() const { return Iterator(); }

    // TODO: removeBehavior
    void clear() {}
};
} // namespace G3D
