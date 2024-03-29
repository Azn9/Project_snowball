﻿#include "GameTest.h"

#include "Core/Public/Core/Moteur.h"
#include "Scenes/LoadingScene.h"

PM3D_API::GameHost* PM3D_API::GameHost::GetInstance()
{
    static GameTest instance;
    return &instance;
}

void GameTest::Initialize()
{
    SetScene(LoadingScene::GetInstancePtr());
    //SetScene(SRScene::GetInstancePtr());
}

void GameTest::InitializePostProcessParam() const
{
    postEffectPlane->enableAllPostEffects();

    postEffectPlane->SetShaderVariableValue("startScreenDistRadial", 0.4f);
    postEffectPlane->SetShaderVariableValue("speedLinesFrameLength", 25);
    postEffectPlane->SetTexturesShaderVariables("speedlines", std::vector<std::wstring>{
                                                    L"SpeedLines01.dds", L"Speedlines02.dds", L"Speedlines03.dds",
                                                    L"SpeedLines04.dds", L"Speedlines05.dds", L"Speedlines06.dds",
                                                    L"SpeedLines07.dds", L"Speedlines08.dds", L"Speedlines09.dds",
                                                    L"SpeedLines10.dds"
                                                });
    postEffectPlane->SetShaderVariableValue("startVelocityLineSpeed", 25);
    postEffectPlane->SetShaderVariableValue("endVelocityLineSpeed", 100);

    postEffectPlane->SetShaderVariableValue("startBlurDist", 300.0f);
    postEffectPlane->SetShaderVariableValue("endBlurDist", 400.0f);
    postEffectPlane->SetShaderVariableValue("blurKernelHalfSize", 2);

    postEffectPlane->SetShaderVariableValue("radialDistance", 0.1f);
    postEffectPlane->SetShaderVariableValue("startVelocityRadial", 35);
    postEffectPlane->SetShaderVariableValue("endVelocityRadial", 110);

    postEffectPlane->SetShaderVariableValue("vignettePower", 6.f);
    postEffectPlane->SetShaderVariableValue("vignetteColor", XMFLOAT4{0.0f, 0.0f, 0.f, 0.8f});
}
