#pragma once

#pragma once
#include <iostream>
#include <PxPhysicsAPI.h>
#include <vector>
#include <algorithm>
#include "FilterGroup.h"
#include "Api/Public/EventSystem/EventSystem.h"
#include "Api/Public/EventSystem/CollisionObstacleEvent.h"
#include <Api/Public/EventSystem/CollisionSkierEvent.h>

using namespace physx;

class SimulationCallback : public PxSimulationEventCallback, public PxContactModifyCallback
{
    std::vector<PxShape*> dynamicToShrink{};

public:
    // Function called with default contact
    void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
    {
        for (PxU32 i = 0; i < nbPairs; i++)
        {
            const PxContactPair modifyPair = pairs[i];
            PxShape* shape0 = modifyPair.shapes[0];
            PxShape* shape1 = modifyPair.shapes[1];
            auto test = shape0->getSimulationFilterData().word0;
            auto test2 = shape1->getSimulationFilterData().word0;
            if ((shape0->getSimulationFilterData().word0 == FilterGroup::eSNOWBALL && shape1->getSimulationFilterData().
                    word0 == FilterGroup::eOBSTACLE)
                || shape1->getSimulationFilterData().word0 == FilterGroup::eSNOWBALL && shape0->
                getSimulationFilterData().word0 == FilterGroup::eOBSTACLE)
            {
                PM3D_API::EventSystem::Publish(CollisionObstacleEvent{});
            }
            if ((shape0->getSimulationFilterData().word0 == FilterGroup::eSNOWBALL && shape1->getSimulationFilterData().
                    word0 == FilterGroup::eSKIER)
                || shape1->getSimulationFilterData().word0 == FilterGroup::eSNOWBALL && shape0->
                getSimulationFilterData().word0 == FilterGroup::eSKIER)
            {
                if (shape0->getSimulationFilterData().word0 == FilterGroup::eSKIER)
                {
                    if (shape1->getSimulationFilterData().word2 == BuriablePenguin::eCanBury)
                        PM3D_API::EventSystem::Publish(CollisionSkierEvent{shape0->getSimulationFilterData().word1});
                    else
                        PM3D_API::EventSystem::Publish(CollisionObstacleEvent{});
                }
                else if (shape1->getSimulationFilterData().word0 == FilterGroup::eSKIER)
                {
                    if (shape0->getSimulationFilterData().word2 == BuriablePenguin::eCanBury)
                        PM3D_API::EventSystem::Publish(CollisionSkierEvent{shape0->getSimulationFilterData().word1});
                    else
                        PM3D_API::EventSystem::Publish(CollisionObstacleEvent{});
                }
            }
            if ((shape0->getSimulationFilterData().word0 == FilterGroup::eOBSTACLE && shape1->getSimulationFilterData().
                    word0 == FilterGroup::eSKIER)
                || shape1->getSimulationFilterData().word0 == FilterGroup::eOBSTACLE && shape0->
                getSimulationFilterData().word0 == FilterGroup::eSKIER)
            {
                if (shape0->getSimulationFilterData().word0 == FilterGroup::eSKIER)
                    PM3D_API::EventSystem::Publish(CollisionSkierEvent{shape0->getSimulationFilterData().word1});
                else
                    PM3D_API::EventSystem::Publish(CollisionSkierEvent{shape1->getSimulationFilterData().word1});
            }
        }
    }

    // Function called on OnContactModify
    void onContactModify(PxContactModifyPair* const pairs, PxU32 count)
    {
        for (PxU32 i = 0; i < count; i++)
        {
            PxContactModifyPair& modifyPair = pairs[i];
            if (modifyPair.shape[0]->getSimulationFilterData().word0 == FilterGroup::eSNOWBALL || modifyPair.shape[1]->
                getSimulationFilterData().word0 == FilterGroup::eSNOWBALL)
            {
                physx::PxVec3 normalContact = modifyPair.contacts.getNormal(i);
                if (normalContact.x >= 0)
                {
                    normalContact = PxVec3(0.5f, 0.5f, -.5f);
                }
                else
                {
                    normalContact = PxVec3(-0.5f, 0.5f, -.5f);
                }
                modifyPair.contacts.setNormal(i, normalContact.getNormalized());
                modifyPair.contacts.setRestitution(i, 1.0f);
                PM3D_API::EventSystem::Publish(CollisionObstacleEvent{});
            }
        }
    }

    void applyAfterContactModification(float& size)
    {
        for (auto shape : dynamicToShrink)
        {
            PxRigidDynamic* snowBall = static_cast<PxRigidDynamic*>(shape->getActor());
            size *= 0.5f;
            shape->setGeometry(PxSphereGeometry(size));
        }
        dynamicToShrink.clear();
    }

    void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
    {
    }

    void onWake(PxActor** actors, PxU32 count)
    {
    }

    void onSleep(PxActor** actors, PxU32 count)
    {
    }

    void onTrigger(PxTriggerPair* pairs, PxU32 count)
    {
    }

    void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count)
    {
    }
};
