#pragma once
#include "../../PM3DApi/Api/Public/GameObject/Basic/BasicShape.h"

class Pine : public PM3D_API::BasicShape
{
public:
    Pine() : BasicShape("pine") {}

private:
    std::wstring GetShaderFileName() override { return L"NewShader.fx"; }
    std::string GetMeshFileName() override { return "tree_pineTallA.obj"; }
};
