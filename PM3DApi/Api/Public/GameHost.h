﻿#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include <DirectXMath.h>

// ReSharper disable once CppUnusedIncludeDirective
#include "../../../PetitMoteur3D/stdafx.h" // NECESSAIRE POUR COMPILER DispositifD3D11.h

#include "../../../PetitMoteur3D/Core/Public/Core/DispositifD3D11.h"
#include "Debug/DebugRenderer.h"
#include "Scene/Scene.h"

namespace PM3D_API
{
    class GameHost
    {
    public:
        // === Singleton ===

        GameHost() = default;
        virtual ~GameHost();
        
        void Update(double deltaTime) const;
        void FixedUpdate(double fixedDeltaTime) const;
        void InitializeScene();

        PM3D::CDispositifD3D11* GetDispositif() const { return dispositif; }
        void SetDispositif(PM3D::CDispositifD3D11* dispositif);
        
        float GetAspectRatio() const;
        float GetScreenWidth() const;
        float GetScreenHeight() const;

        GameHost(GameHost&) = delete;
        void operator =(GameHost&) = delete;
        GameHost(GameHost&&) = delete;
        void operator =(GameHost&&) = delete;

        static GameHost* GetInstance();

        // === Interface ===

        virtual void Initialize() = 0;
        virtual void Draw();

        Scene* GetScene() const { return scene; }

        // === Other ===
        void AddDebugRenderer(std::unique_ptr<DebugRenderer>&& child) { debugRenderers.push_back(std::move(child)); }
        const std::vector<std::unique_ptr<DebugRenderer>>& GetDebugRenderers() { return debugRenderers; }

    protected:
        std::vector<std::unique_ptr<DebugRenderer>> debugRenderers;
        virtual void SetScene(Scene* newScene);

    private:
        Scene* scene;
        PM3D::CDispositifD3D11* dispositif;

    };
}
