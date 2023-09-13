﻿#include "StdAfx.h"
#include "Core/dispositifd3d11.h"
#include "Texture/Texture.h"
#include "Util/resource.h"
#include "Util/util.h"
#include "Texture/DDSTextureLoader.h"

using namespace DirectX;

namespace PM3D
{
CTexture::CTexture(const std::wstring& filename,
                   CDispositifD3D11*
                   pDispositif)
	: m_Filename(filename)
	  , m_Texture(nullptr)
{
	ID3D11Device* pDevice = pDispositif->GetD3DDevice();
	// Charger la texture en ressource
	DXEssayer(CreateDDSTextureFromFile(pDevice,
		m_Filename.c_str(),
		nullptr,
		&m_Texture), DXE_FICHIERTEXTUREINTROUVABLE);
}

CTexture::~CTexture()
{
	DXRelacher(m_Texture);
}

}