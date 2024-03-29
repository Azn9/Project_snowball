#include "Api/Public/Component/Basic/Physics/BoxCollider.h"
#include "Api/Public/Component/Basic/Physics/Rigidbody.h"
#include "Api/Public/GameHost.h"

void PM3D_API::BoxCollider::Initialize()
{
    Collider::Initialize();

    const auto pxPhysics = parentObject->GetScene()->GetPhysicsResolver()->GetPhysics();

    const auto worldScale = parentObject->GetWorldScale();
    shape = pxPhysics->createShape(physx::PxBoxGeometry{worldScale.x, worldScale.y, worldScale.z}, *material, true);

    const auto rigidbody = parentObject->GetComponent<Rigidbody>();
    // Ne peut pas être null, vérifié dans Collider::Initialize()

    const auto actor = rigidbody->GetActor();

    physx::PxShape* baseShape;
    actor->getShapes(&baseShape, 1, 0);

    actor->detachShape(*baseShape);
    actor->attachShape(*shape);

    // Set position and rotation
    const auto worldPosition = parentObject->GetWorldPosition();
    const auto worldRotation = parentObject->GetWorldRotationQuaternion();

    const auto globalPos = physx::PxTransform(
        physx::PxVec3(worldPosition.x, worldPosition.y, worldPosition.z),
        physx::PxQuat(worldRotation.x, worldRotation.y, worldRotation.z, worldRotation.w)
    );

    actor->setGlobalPose(globalPos);
}
