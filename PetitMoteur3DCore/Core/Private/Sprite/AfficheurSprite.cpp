﻿#include "Core/Public/Sprite/AfficheurSprite.h"

#include <d3dcompiler.h>

#include "Core/Public/Util/resource.h"
#include "Core/Public/Core/MoteurWindows.h"
#include "Core/Public/Object/Panneau.h"
#include "Core/Public/Util/util.h"

namespace PM3D
{
    struct ShadersParams
    {
        XMMATRIX matWVP; // la matrice totale
    };

    // Definir l’organisation de notre sommet
    D3D11_INPUT_ELEMENT_DESC CSommetSprite::layout[] =
    {
        {
            "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
            D3D11_INPUT_PER_VERTEX_DATA, 0
        },
        {
            "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,
            D3D11_INPUT_PER_VERTEX_DATA, 0
        }
    };
    UINT CSommetSprite::numElements = ARRAYSIZE(layout);

    CSommetSprite CAfficheurSprite::sommets[6] = {
        CSommetSprite(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)),
        CSommetSprite(XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f)),
        CSommetSprite(XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)),
        CSommetSprite(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)),
        CSommetSprite(XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)),
        CSommetSprite(XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f))
    };

    CAfficheurSprite::CAfficheurSprite(
        CDispositifD3D11* _pDispositif
    )
        : pDispositif(_pDispositif)
          , pVertexBuffer(nullptr)
          , pConstantBuffer(nullptr)
          , pEffet(nullptr)
          , pTechnique(nullptr)
          , pPasse(nullptr)
          , pVertexLayout(nullptr)
          , pSampleState(nullptr)
    {
        // Création du vertex buffer et copie des sommets
        ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_IMMUTABLE;
        bd.ByteWidth = sizeof(sommets);
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA InitData;
        ZeroMemory(&InitData, sizeof(InitData));
        InitData.pSysMem = sommets;

        DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer),
                  DXE_CREATIONVERTEXBUFFER);

        // Initialisation de l’effet
        CAfficheurSprite::InitEffet();
    }

    CAfficheurSprite::~CAfficheurSprite()
    {
        DXRelacher(pConstantBuffer);
        DXRelacher(pSampleState);
        DXRelacher(pEffet);
        DXRelacher(pVertexLayout);
        DXRelacher(pVertexBuffer);
    }

    void CAfficheurSprite::AjouterSprite(std::string NomTexture, int _x, int _y, int _dx, int _dy)
    {
        float dx, dy;

        // Initialisation de la texture
        CGestionnaireDeTextures& TexturesManager = CMoteurWindows::GetInstance().GetTextureManager();
        const std::wstring ws(NomTexture.begin(), NomTexture.end());
        auto pSprite = std::make_unique<CSprite>();
        pSprite->pTextureD3D = TexturesManager.GetNewTexture(ws.c_str(), pDispositif)->GetD3DTexture();

        // Obtenir les dimensions de la texture si _dx et _dy sont à 0;
        if (_dx == 0 && _dy == 0)
        {
            ID3D11Resource* pResource;
            ID3D11Texture2D* pTextureInterface = 0;

            pSprite->pTextureD3D->GetResource(&pResource);
            pResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);

            D3D11_TEXTURE2D_DESC desc;
            pTextureInterface->GetDesc(&desc);

            DXRelacher(pResource);
            DXRelacher(pTextureInterface);

            dx = static_cast<float>(desc.Width);
            dy = static_cast<float>(desc.Height);
        }
        else
        {
            dx = static_cast<float>(_dx);
            dy = static_cast<float>(_dy);
        }

        // Dimension en facteur
        const float facteurX = dx * 2.0f / pDispositif->GetLargeur();
        const float facteurY = dy * 2.0f / pDispositif->GetHauteur();

        // Position en coordonnées logiques
        // 0,0 pixel = -1,1
        const auto x = static_cast<float>(_x);
        const auto y = static_cast<float>(_y);
        const float posX = x * 2.0f / pDispositif->GetLargeur() - 1.0f;
        const float posY = 1.0f - y * 2.0f / pDispositif->GetHauteur();

        pSprite->matPosDim = XMMatrixScaling(facteurX, facteurY, 1.0f) * XMMatrixTranslation(posX, posY, 0.0f);

        // On l’ajoute à notre vecteur
        tabSprites.push_back(std::move(pSprite));
    }

    void CAfficheurSprite::AjouterPanneau(const std::string& NomTexture, const XMFLOAT3& _position, float _dx,
                                          float _dy)
    {
        // Initialisation de la texture
        CGestionnaireDeTextures& TexturesManager = CMoteurWindows::GetInstance().GetTextureManager();

        const std::wstring ws(NomTexture.begin(), NomTexture.end());
        auto pPanneau = std::make_unique<CPanneau>();

        pPanneau->pTextureD3D = TexturesManager.GetNewTexture(ws, pDispositif)->GetD3DTexture();

        // Obtenir la dimension de la texture si _dx et _dy sont à 0;
        if (_dx == 0.0f && _dy == 0.0f)
        {
            ID3D11Resource* pResource;
            ID3D11Texture2D* pTextureInterface;

            pPanneau->pTextureD3D->GetResource(&pResource);
            pResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);

            D3D11_TEXTURE2D_DESC desc;
            pTextureInterface->GetDesc(&desc);

            DXRelacher(pResource);
            DXRelacher(pTextureInterface);

            pPanneau->dimension.x = static_cast<float>(desc.Width);
            pPanneau->dimension.y = static_cast<float>(desc.Height);

            // Dimension en facteur
            pPanneau->dimension.x = pPanneau->dimension.x * 2.0f / static_cast<float>(pDispositif->GetLargeur());
            pPanneau->dimension.y = pPanneau->dimension.y * 2.0f / static_cast<float>(pDispositif->GetHauteur());
        }
        else
        {
            pPanneau->dimension.x = _dx;
            pPanneau->dimension.y = _dy;
        }

        // Position en coordonnées du monde
        const XMMATRIX& viewProj = CMoteurWindows::GetInstance().GetMatViewProj();
        pPanneau->position = _position;
        pPanneau->matPosDim = XMMatrixScaling(pPanneau->dimension.x, pPanneau->dimension.y, 1.0f) * XMMatrixTranslation(
            pPanneau->position.x, pPanneau->position.y, pPanneau->position.z) * viewProj;

        // On l’ajoute à notre vecteur
        tabSprites.push_back(std::move(pPanneau));
    }

    void CAfficheurSprite::AjouterSpriteTexte(ID3D11ShaderResourceView* pTexture, int _x, int _y)
    {
        std::unique_ptr<CSprite> pSprite = std::make_unique<CSprite>();
        pSprite->pTextureD3D = pTexture;
        // Obtenir la dimension de la texture;
        ID3D11Resource* pResource;
        ID3D11Texture2D* pTextureInterface = 0;
        pSprite->pTextureD3D->GetResource(&pResource);
        pResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
        D3D11_TEXTURE2D_DESC desc;
        pTextureInterface->GetDesc(&desc);
        DXRelacher(pResource);
        DXRelacher(pTextureInterface);
        const float dx = float(desc.Width);
        const float dy = float(desc.Height);
        // Dimension en facteur
        const float facteurX = dx * 2.0f / pDispositif->GetLargeur();
        const float facteurY = dy * 2.0f / pDispositif->GetHauteur();
        // Position en coordonnées logiques, 0,0 pixel = -1,1
        const float x = float(_x);
        const float y = float(_y);
        const float posX = x * 2.0f / pDispositif->GetLargeur() - 1.0f;
        const float posY = 1.0f - y * 2.0f / pDispositif->GetHauteur();
        pSprite->matPosDim = XMMatrixScaling(facteurX, facteurY, 1.0f) *
            XMMatrixTranslation(posX, posY, 0.0f);
        // On l’ajoute à notre vecteur
        tabSprites.push_back(std::move(pSprite));
    }

    void CAfficheurSprite::InitEffet()
    {
        // Compilation et chargement du vertex shader
        ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

        // Création d’un tampon pour les constantes de l’effet
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(ShadersParams);
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;

        pD3DDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer);
        // Pour l’effet
        ID3DBlob* pFXBlob = nullptr;

        DXEssayer(D3DCompileFromFile(L"shader/Sprite1.fx", 0, 0, 0,
                                     "fx_5_0", 0, 0,
                                     &pFXBlob, 0),
                  DXE_ERREURCREATION_FX);

        D3DX11CreateEffectFromMemory(pFXBlob->GetBufferPointer(),
                                     pFXBlob->GetBufferSize(), 0, pD3DDevice, &pEffet);

        pFXBlob->Release();

        pTechnique = pEffet->GetTechniqueByIndex(0);
        pPasse = pTechnique->GetPassByIndex(0);

        // Créer l’organisation des sommets pour le VS de notre effet
        D3DX11_PASS_SHADER_DESC effectVSDesc;
        pPasse->GetVertexShaderDesc(&effectVSDesc);

        D3DX11_EFFECT_SHADER_DESC effectVSDesc2;
        effectVSDesc.pShaderVariable->GetShaderDesc(effectVSDesc.ShaderIndex, &effectVSDesc2);

        const void* vsCodePtr = effectVSDesc2.pBytecode;
        const unsigned vsCodeLen = effectVSDesc2.BytecodeLength;
        pVertexLayout = nullptr;
        DXEssayer(pD3DDevice->CreateInputLayout(
                      CSommetSprite::layout,
                      CSommetSprite::numElements,
                      vsCodePtr,
                      vsCodeLen,
                      &pVertexLayout),
                  DXE_CREATIONLAYOUT);

        // Initialisation des paramètres de sampling de la texture
        D3D11_SAMPLER_DESC samplerDesc;
        samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = 4;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samplerDesc.BorderColor[0] = 0;
        samplerDesc.BorderColor[1] = 0;
        samplerDesc.BorderColor[2] = 0;
        samplerDesc.BorderColor[3] = 0;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        // Création de l’état de sampling
        pD3DDevice->CreateSamplerState(&samplerDesc, &pSampleState);
    }

    void CAfficheurSprite::Draw()
    {
        // Obtenir le contexte
        ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();

        // Choisir la topologie des primitives
        pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // Source des sommets
        UINT stride = sizeof(CSommetSprite);
        const UINT offset = 0;
        pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

        // input layout des sommets
        pImmediateContext->IASetInputLayout(pVertexLayout);

        // Le sampler state
        ID3DX11EffectSamplerVariable* variableSampler;
        variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
        variableSampler->SetSampler(0, pSampleState);
        pPasse->Apply(0, pImmediateContext);

        ID3DX11EffectConstantBuffer* pCB = pEffet->GetConstantBufferByName("param");

        ID3DX11EffectShaderResourceVariable* variableTexture;
        variableTexture = pEffet->GetVariableByName("textureEntree")->AsShaderResource();

        // Faire le rendu de tous nos sprites
        for (int i = 0; i < tabSprites.size(); ++i)
        {
            // Initialiser et sélectionner les « constantes » de l’effet
            ShadersParams sp;
            sp.matWVP = XMMatrixTranspose(tabSprites[i]->matPosDim);
            pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);

            pCB->SetConstantBuffer(pConstantBuffer);

            // Activation de la texture
            variableTexture->SetResource(tabSprites[i]->pTextureD3D);

            pPasse->Apply(0, pImmediateContext);

            // **** Rendu de l’objet
            pImmediateContext->Draw(6, 0);
        }
    }
}
