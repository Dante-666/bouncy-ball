#include "behavior/Attractor.h"

#include "PhysicsEntity.h"
#include "PhysicsScene.h"

namespace G3D {

template <>
Attractor<GhostEntity>::Attractor(const Attractor<GhostEntity> &ff) {
    //m_dummy = ff.m_dummy;
}

template <> Attractor<GhostEntity>::Attractor(const Any &any) {
    *this = Attractor();
    any.verifyName("Attractor");
    AnyTableReader r(any);
    //r.getIfPresent("dummy1", *m_dummy);
}

template <typename E> const String Attractor<E>::getName() const {
    return "Attractor";
}

template <> Any Attractor<GhostEntity>::toAny() const {
    Any any(Any::TABLE, "Attractor");
    any["force"] = m_force;
    return any;
}

template <>
Attractor<GhostEntity> &Attractor<GhostEntity>::operator=(const Any &a) {
    return *this = Attractor(a);
}

template <> void Attractor<GhostEntity>::apply(GhostEntity *gEntity) {
    // This is supposed to add a positional constraint and this is purely
    // implementation dependent.
    /*gEntity->m_physicsScene->getPhysicsEngine()->applyForceField(
        gEntity, m_force, PurePhysics::RADIAL);*/
    BehaviorChain::apply(gEntity);
}
} // namespace G3D
