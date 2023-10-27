﻿#include "../../Public/Light/PointLight.h"

PM3D_API::ShaderLightDefaultParameters PM3D_API::PointLight::GetShaderLightDefaultParameters(
    GameObject* gameObject) const
{
    const auto mVLight = DirectX::XMMatrixLookAtLH(
        DirectX::XMVectorSet(worldPosition.x, worldPosition.y, worldPosition.z, 1.0f),
        DirectX::XMVectorSet(gameObject->GetWorldPosition().x, gameObject->GetWorldPosition().y,
                             gameObject->GetWorldPosition().z, 1.0f),
        DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f)
    );

    constexpr float fov = DirectX::XM_PI / 4;
    constexpr float aspectRatio = 1.0f;
    constexpr float nearPlane = 2.0;
    constexpr float farPlane = 100.0;

    const auto mPLight = DirectX::XMMatrixPerspectiveFovLH(
        fov,
        aspectRatio,
        nearPlane,
        farPlane
    );

    const auto matWorldViewProj = mVLight * mPLight;

    return {
        true,
        static_cast<int>(LightType::POINT),
        DirectX::XMFLOAT4(worldPosition.x, worldPosition.y, worldPosition.z, 1.0f),
        {0.0f, 0.0f, 0.0f, 0.0f},

        {0.2f * color.x, 0.2f * color.y, 0.2f * color.z, 1.0f},
        {color.x, color.y, color.z, 1.0f},
        {0.6f * color.x, 0.6f * color.y, 0.6f * color.z, 1.0f},

        intensity,

        0.0f,
        0.0f,

        matWorldViewProj
    };
}
