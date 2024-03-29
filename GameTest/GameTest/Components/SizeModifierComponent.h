#pragma once

#include "Api/Public/Component/Component.h"

class SizeModifierComponent final : public PM3D_API::Component
{
public:
    SizeModifierComponent();

    void PhysicsUpdate() override;

    void DrawDebugInfo() const override;

    float currentSize = 1.f;

    inline static constexpr float MAX_SIZE = 4.f;

private:
    float _sizeModificationSpeed = 1.002f;
    float _sizeModificationSpeedDecrease = 0.993f;
    bool _collisionHappend = false;
    bool _resetRequested = false;
    bool _inTunnel = false;
};
