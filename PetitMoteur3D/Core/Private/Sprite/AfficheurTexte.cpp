﻿#include "stdafx.h"
#include "Core/Public/Sprite/AfficheurTexte.h"

#include "Core/Public/Util/util.h"

ULONG_PTR PM3D::CAfficheurTexte::token = 0;

PM3D::CAfficheurTexte::CAfficheurTexte(PM3D::CDispositifD3D11* pDispositif, int largeur, int hauteur,
    Gdiplus::Font* pPolice)
: pDispo(pDispositif)
, TexWidth(largeur)
, TexHeight(hauteur)
, pFont(pPolice)
, pTexture(nullptr)
, pSurface(nullptr)
, pTextureView(nullptr)
, pCharBitmap(nullptr)
, pCharGraphics(nullptr)
, pBlackBrush(nullptr)

{
    // Créer le bitmap et un objet GRAPHICS (un dessinateur)
    pCharBitmap = std ::make_unique<Gdiplus ::Bitmap>(TexWidth, TexHeight,
    PixelFormat32bppARGB);
    pCharGraphics = std::make_unique<Gdiplus::Graphics>(pCharBitmap.get());
    // Paramètres de l’objet Graphics
    pCharGraphics->SetCompositingMode(Gdiplus ::CompositingModeSourceOver);
    pCharGraphics->SetCompositingQuality(Gdiplus ::CompositingQualityHighSpeed);
    pCharGraphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
    pCharGraphics->SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighSpeed);
    pCharGraphics->SetSmoothingMode(Gdiplus::SmoothingModeNone);
    pCharGraphics->SetPageUnit(Gdiplus::UnitPixel);
    Gdiplus::TextRenderingHint hint = Gdiplus::TextRenderingHintAntiAlias;
    //TextRenderingHintSystemDefault;
    pCharGraphics->SetTextRenderingHint(hint);
    // Un brosse noire pour le remplissage
    // Notez que la brosse aurait pu être passée
    // en paramètre pour plus de flexibilité
    pBlackBrush = std::make_unique<Gdiplus::SolidBrush>(Gdiplus ::Color(255,
    0, 0, 0));
    // On efface le bitmap (notez le NOIR TRANSPARENT...)
    pCharGraphics->Clear(Gdiplus ::Color(0, 0, 0, 0));
    // Nous pourrions ici écrire une valeur initiale sur le bitmap
    // std ::wstring s=L"Valeur initiale";
    // pCharGraphics->DrawString( s.c_str(), s.size(), pFont, PointF( 0.0f, 0.0f ), pBlackBrush.get() );
    // Accéder aux bits du bitmap
    Gdiplus::BitmapData bmData;

#pragma warning( push )
#pragma warning( disable : 4238 )
    
    pCharBitmap->LockBits(&Gdiplus::Rect(0, 0, TexWidth, TexHeight), Gdiplus ::ImageLockModeRead, PixelFormat32bppARGB, &bmData);

#pragma warning( pop )
    
    // Création d’une texture de même dimension sur la carte graphique
    D3D11_TEXTURE2D_DESC texDesc;
    texDesc.Width = TexWidth;
    texDesc.Height = TexHeight;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = bmData.Scan0;
    data.SysMemPitch = TexWidth * 4;
    data.SysMemSlicePitch = 0;
    // Création de la texture à partir des données du bitmap
    DXEssayer(pDispo-> GetD3DDevice()->CreateTexture2D(&texDesc, &data,
    &pTexture));
    // Création d’un « resourve view » pour accéder facilement à la texture
    // (comme pour les sprites)
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;
    DXEssayer(pDispositif->GetD3DDevice()->CreateShaderResourceView(pTexture,
    &srvDesc, &pTextureView));
    pCharBitmap->UnlockBits(&bmData);
}

PM3D::CAfficheurTexte::~CAfficheurTexte()
{
    DXRelacher(pTexture); 
}

void PM3D::CAfficheurTexte::Ecrire(const std::wstring& s)
{
    // Effacer
    pCharGraphics->Clear(Gdiplus ::Color(0, 0, 0, 0));
    // Écrire le nouveau texte
    pCharGraphics->DrawString(s.c_str(), static_cast<int>(s.size()), pFont,
    Gdiplus::PointF(0.0f, 0.0f), pBlackBrush.get());
    // Transférer
    Gdiplus::BitmapData bmData;

#pragma warning( push )
#pragma warning( disable : 4238 )
    
    pCharBitmap->LockBits(&Gdiplus::Rect(0, 0, TexWidth, TexHeight),
    Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmData);

#pragma warning( pop )
    
    pDispo->GetImmediateContext()->UpdateSubresource(pTexture, 0, 0,
    bmData.Scan0, TexWidth * 4, 0);
    pCharBitmap->UnlockBits(&bmData);
}

void PM3D::CAfficheurTexte::Init()
{
    Gdiplus::GdiplusStartupInput startupInput(0, TRUE, TRUE);
    Gdiplus::GdiplusStartupOutput startupOutput;
    GdiplusStartup(&token, &startupInput, &startupOutput);
}

void PM3D::CAfficheurTexte::Close()
{
    Gdiplus::GdiplusShutdown(token);
}
