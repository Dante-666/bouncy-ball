#include "behavior/Solid.h"

#include "PhysicsEntity.h"
#include "PhysicsScene.h"

namespace G3D {

Solid::Solid(const Solid &opq) { m_phased = opq.m_phased; }

Solid::Solid(const Any &any) {
    *this = Solid();
    any.verifyName("Solid");
    AnyTableReader r(any);
    r.getIfPresent("phased", m_phased);
    r.getIfPresent("rollingFriction", m_rollingFriction);
}

Any Solid::toAny() const {
    Any any(Any::TABLE, "Solid");
    any["phased"] = m_phased;
    any["rollingFriction"] = m_rollingFriction;
    return any;
}

const String Solid::getName() const { return "Solid"; }

const float Solid::getRollingFriction() const { return m_rollingFriction; }
} // namespace G3D
