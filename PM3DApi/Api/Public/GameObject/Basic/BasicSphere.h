﻿#pragma once
#include "BasicShape.h"
#include "Api/Public/GameObject/GameObject.h"
#include "Api/Public/Shader/Shader.h"

namespace PM3D_API
{
    class BasicSphere : public BasicShape
    {
    public:
        explicit BasicSphere(const std::string& name)
            : BasicShape(name)
        {
        }

        BasicSphere(const std::string& name, const DirectX::XMFLOAT3& worldPosition)
            : BasicShape(name, worldPosition)
        {
        }

        BasicSphere(const std::string& name, const DirectX::XMFLOAT3& worldPosition,
                    const DirectX::XMFLOAT3& worldRotation)
            : BasicShape(name, worldPosition, worldRotation)
        {
        }

        BasicSphere(const std::string& name, const DirectX::XMFLOAT3& worldPosition, const Quaternion& worldRotation)
            : BasicShape(name, worldPosition, worldRotation)
        {
        }

        BasicSphere(const std::string& name, const DirectX::XMFLOAT3& worldPosition,
                    const DirectX::XMFLOAT3& worldRotation,
                    const DirectX::XMFLOAT3& worldScale)
            : BasicShape(name, worldPosition, worldRotation, worldScale)
        {
        }

        BasicSphere(const std::string& name, const DirectX::XMFLOAT3& worldPosition, const Quaternion& worldRotation,
                    const DirectX::XMFLOAT3& worldScale)
            : BasicShape(name, worldPosition, worldRotation, worldScale)
        {
        }

        BasicSphere(const std::string& name, std::unique_ptr<PM3D_API::Shader>&& shader)
            : BasicShape(name, std::move(shader))
        {
        }

        BasicSphere(const std::string& name, std::unique_ptr<PM3D_API::Shader>&& shader,
                    const DirectX::XMFLOAT3& worldPosition)
            : BasicShape(name, std::move(shader), worldPosition)
        {
        }

        BasicSphere(const std::string& name, std::unique_ptr<PM3D_API::Shader>&& shader,
                    const DirectX::XMFLOAT3& worldPosition, const DirectX::XMFLOAT3& worldRotation)
            : BasicShape(name, std::move(shader), worldPosition, worldRotation)
        {
        }

        BasicSphere(const std::string& name, std::unique_ptr<PM3D_API::Shader>&& shader,
                    const DirectX::XMFLOAT3& worldPosition, const Quaternion& worldRotation)
            : BasicShape(name, std::move(shader), worldPosition, worldRotation)
        {
        }

        BasicSphere(const std::string& name, std::unique_ptr<PM3D_API::Shader>&& shader,
                    const DirectX::XMFLOAT3& worldPosition, const DirectX::XMFLOAT3& worldRotation,
                    const DirectX::XMFLOAT3& worldScale)
            : BasicShape(name, std::move(shader), worldPosition, worldRotation, worldScale)
        {
        }

        BasicSphere(const std::string& name, std::unique_ptr<PM3D_API::Shader>&& shader,
                    const DirectX::XMFLOAT3& worldPosition, const Quaternion& worldRotation,
                    const DirectX::XMFLOAT3& worldScale)
            : BasicShape(name, std::move(shader), worldPosition, worldRotation, worldScale)
        {
        }

    private:
        std::wstring GetShaderFileName() override { return L"shader/NewShader.fx"; }
        std::string GetMeshFileName() override { return "DefaultObj/sphere.obj"; }
    };
}
