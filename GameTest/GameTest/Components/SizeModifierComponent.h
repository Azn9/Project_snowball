#pragma once
#include <DirectXMath.h>
#include <iostream>

#include "Core/Imgui/imgui.h"
#include "Api/Public/Component/Component.h"
#include "Api/Public/GameObject/GameObject.h"
#include "Api/Public/Component/Basic/Physics/SphereCollider.h"
#include "Api/Public/EventSystem/EventSystem.h"
#include "Api/Public/EventSystem/CollisionObstacleEvent.h"
#include "GameTest/Event/GameOverEvent.h"

class SizeModifierComponent final : public PM3D_API::Component
{
public:

	SizeModifierComponent() {
		PM3D_API::EventSystem::Subscribe([this](const CollisionObstacleEvent& event)
			{
				_collisionHappend = true;
			});


		PM3D_API::EventSystem::Subscribe([this](const RestartEvent&)
			{
				_resetRequested = true;
			});
		
	}

	void PhysicsUpdate() override
	{
		physx::PxShape* shape = parentObject->GetComponent<PM3D_API::SphereCollider>()->getShape();
		if(_resetRequested)
		{
			_resetRequested = false;
			shape->setGeometry(physx::PxSphereGeometry(0.2f));
			parentObject->SetWorldScale(DirectX::XMFLOAT3(0.2f,0.2f,0.2f));
			return;
		}
		
		DirectX::XMFLOAT3 preScale = parentObject->GetWorldScale();

		if (_collisionHappend)
		{
			parentObject->SetWorldScale(DirectX::XMFLOAT3(
				preScale.x * 0.5f,
				preScale.y * 0.5f,
				preScale.z * 0.5f
			));
			_collisionHappend = false;
			shape->setGeometry(physx::PxSphereGeometry(preScale.x * .5f));

			if (preScale.x * .5f < 0.1f)
				PM3D_API::EventSystem::Publish(GameOverEvent(false));
		}
		else
		{
			parentObject->SetWorldScale(DirectX::XMFLOAT3(
				preScale.x * _sizeModificationSpeed,
				preScale.y * _sizeModificationSpeed,
				preScale.z * _sizeModificationSpeed
			));
			shape->setGeometry(physx::PxSphereGeometry(preScale.x * _sizeModificationSpeed));
		}
	}

	void DrawDebugInfo() const override
	{
		ImGui::Text("Speed");
		ImGui::SameLine(100); ImGui::Text(std::to_string(_sizeModificationSpeed).c_str());
	}

private:
	float _sizeModificationSpeed = 1.002f;
	bool _collisionHappend = false;
	bool _resetRequested = false;
};
