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
#include "G3D-base/G3DString.h"
#include "G3D-base/debugAssert.h"

namespace G3D {

template <typename E> class BehaviorChain {
    BehaviorChain *next = nullptr; // 1. "next" pointer in the base class
public:
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

    // TODO: Make this to Entity
    virtual void apply(E *pEntity) {
        if (next)
            next->apply(pEntity);
    }

    class Iterator {
        BehaviorChain *node;

    public:
        Iterator(BehaviorChain *node = nullptr) { this->node = node; }
        bool isLast() { return node->next == nullptr; }
        bool hasNext() { return node->next; }
        void advance() {
            if (hasNext()) {
                node = node->next;
            }
        }

        inline bool operator!=(const Iterator &other) const {
            return !(*this == other);
        }

        BehaviorChain *operator->() const {
            debugAssert(isValidHeapPointer(node));
            return node;
        }

        BehaviorChain *operator*() const {
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

/*        inline bool operator!=(const Iterator &other) const {
            return !(*this == other);
        }

        bool operator==(const Iterator &other) const {
            if (other.isDone || isDone) {
                // Common case; check against isDone.
                return (isDone == other.isDone);
            } else {
                return (node == other.node) && (index == other.index);
            }
        }

        Iterator &operator++() {
            debugAssert(!isDone);
            debugAssert(node != nullptr);
            debugAssert(isValidHeapPointer(node));
            debugAssert((node->next == nullptr) ||
                        isValidHeapPointer(node->next));
            node = node->next;
            findNext();
            debugAssert(isDone || isValidHeapPointer(node));
            return *this;
        }

        Iterator operator++(int) {
            Iterator old = *this;
            ++(*this);
            return old;
        }

        const BehaviorChain &operator*() const { return node; }

        const String &value() const { return node->getName(); }

        // const Key &key() const { return node->entry.key; }

        BehaviorChain *operator->() const {
            debugAssert(isValidHeapPointer(node));
            return &(node);
        }

        operator BehaviorChain *() const {
            debugAssert(isValidHeapPointer(node));
            return &(node->entry);
        }

        bool isValid() const { return !isDone; }
    };

    Iterator begin() const { return Iterator(m_numBuckets, m_bucket); }
    */
