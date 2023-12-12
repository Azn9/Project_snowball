﻿#pragma once

#include <mutex>

#include "Api/Public/Shader/Shader.h"
#include "Api/Public/Util/FileWatcher.h"

namespace PM3D_API
{
    class DefaultShaderInstanced final : public Shader
    {
    public:
        explicit DefaultShaderInstanced(const std::wstring& fileName);
        ~DefaultShaderInstanced() override;

        void Initialize(const std::wstring& wstring);
        void Destroy();

        ID3D11Buffer* GetShaderParametersBuffer() const override;
        ID3DX11EffectPass* GetPass() const override;
        ID3D11InputLayout* GetVertexLayout() const override;
        ID3D11Buffer* GetIndexBuffer() const override;

        ID3D11InputLayout* GetShadowVertexLayout() const override;
        ID3D11Texture2D* GetDepthTexture() const override;
        ID3D11DepthStencilView* GetDepthStencilView() const override;
        ID3D11ShaderResourceView* GetDepthShaderResourceView() const override;

        ID3DX11Effect* GetEffect() const override;

        ID3D11Buffer** GetVertexBufferPtr() const override;
        ID3D11Buffer** GetIndexBufferPtr() const override;

        void LoadLights(
            ID3D11DeviceContext* context,
            GameObject* gameObject
        ) override;

        void* PrepareParameters(
            DirectX::XMMATRIX matWorldViewProj,
            DirectX::XMMATRIX matWorld
        ) override;

        void ApplyMaterialParameters(
            void* shaderParameters,
            DirectX::XMVECTOR materialAmbiant,
            DirectX::XMVECTOR materialDiffuse,
            DirectX::XMVECTOR materialSpecular,
            float specularPower,
            ID3D11ShaderResourceView* albedoTexture,
            ID3D11ShaderResourceView* normalmapTexture
        ) override;

        void DeleteParameters(void* shader_parameters) override;

        void ApplyShaderParams() const override;

    protected:
        std::wstring fileName;

        ID3D11Buffer* shaderParametersBuffer;
        ID3DX11Effect* effect;
        ID3DX11EffectTechnique* technique;
        ID3DX11EffectPass* passe;
        ID3D11InputLayout* vertexLayout;

        ID3D11SamplerState* albedoSampleState;
        ID3D11SamplerState* normalmapSampleState;

        ID3D11InputLayout* vertexLayoutShadow;
        ID3D11Texture2D* depthTexture;
        ID3D11DepthStencilView* depthStencilView;
        ID3D11ShaderResourceView* depthShaderResourceView;

        mutable ID3D11Buffer* vertexBuffer;
        mutable ID3D11Buffer* indexBuffer;
        bool initialized;

    private:
        struct DefaultShaderInstancedParameters
        {
            DirectX::XMVECTOR cameraPos;
            DirectX::XMVECTOR cameraDir;

            DirectX::XMVECTOR materialAmbiant;
            DirectX::XMVECTOR materialDiffuse;
            DirectX::XMVECTOR materialSpecular;

            float materialSpecularPower;

            bool hasAlbedoTexture;
            bool hasNormalmapTexture;
        };

        FileWatcher fileWatcher;
        mutable std::mutex reloadingMutex{};
    };
}
