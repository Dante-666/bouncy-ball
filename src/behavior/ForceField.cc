#include "behavior/ForceField.h"

#include "PhysicsEntity.h"
#include "PhysicsScene.h"

namespace G3D {

template<> ForceField<GhostEntity>::ForceField(const ForceField<GhostEntity> &ff) {
    m_force = ff.m_force;
}

template<> ForceField<GhostEntity>::ForceField(const Any &any) {
    *this = ForceField();
    any.verifyName("ForceField");
    AnyTableReader r(any);
    r.getIfPresent("force", m_force);
}

/*template <typename E> ForceField<E>::ForceField(const Any &any) {
    *this = ForceField();
    any.verifyName("ForceField");
    AnyTableReader r(any);
    r.getIfPresent("force", m_force);
}*/

template<typename E> const String ForceField<E>::getName() const {
   return "ForceField"; 
} 

template <> Any ForceField<GhostEntity>::toAny() const {
    Any any(Any::TABLE, "ForceField");
    any["force"] = m_force;
    return any;
}

template <>
ForceField<GhostEntity> &ForceField<GhostEntity>::operator=(const Any &a) {
    return *this = ForceField(a);
}

template <> void ForceField<GhostEntity>::apply(GhostEntity *gEntity) {
    /*if (!gEntity->canChange())
        return;
    gEntity->m_physicsScene->getPhysicsEngine()->applyForceField(gEntity,
                                                                 m_force);
    BehaviorChain::apply(gEntity);*/
}
} // namespace G3D
