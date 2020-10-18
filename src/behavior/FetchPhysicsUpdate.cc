#include "behavior/FetchPhysicsUpdate.h"

#include "PhysicsEntity.h"
#include "PhysicsScene.h"

namespace G3D {

template<> void FetchPhysicsUpdate<PhysicsEntity>::apply(PhysicsEntity *pEntity) {
    if (!pEntity->canChange())
        return;
    pEntity->setFrame(
        pEntity->m_physicsScene->getPhysicsEngine()->getFrame(pEntity));
    BehaviorChain::apply(pEntity);
}
} // namespace G3D
