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

#include "G3D-base/G3DString.h"
#include <map>

/** \brief Used by chains to move forward or backward and querying
 *
 */
namespace G3D {
template <typename C> class Iterator {
    C *node;
    std::map<String, C *> _m_intern;
    bool isDone = false;
    bool _map_empty = true;

    void _m_intern_fill() {
        C *first = node;
        while (!isDone) {
            _m_intern[node->getName()] = node;
            advance();
        }
        node = first;
    }

public:
    Iterator(C *node = nullptr) { this->node = node; }
    bool isOver() { return isDone; }
    bool hasNext() { return node->next != nullptr; }
    void advance() {
        if (hasNext()) {
            node = node->next;
        } else {
            isDone = true;
        }
    }

    C *getByName(const String name) {
        if (_map_empty) {
            _m_intern_fill();
            _map_empty = false;
        }
        auto it = _m_intern.find(name);
        if (it != _m_intern.end()) {
            return _m_intern[name];
        } else {
            return nullptr;
        }
    }
    /*inline bool operator!=(const Iterator &other) const {
    return !(*this == other);
}*/

    C *operator->() const {
        debugAssert(isValidHeapPointer(node));
        return node;
    }

    C *operator*() const {
        debugAssert(isValidHeapPointer(node));
        return node;
    }
};
} // namespace G3D
