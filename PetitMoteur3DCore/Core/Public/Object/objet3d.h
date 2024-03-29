#pragma once
#include <DirectXMath.h>

using namespace DirectX;

namespace PM3D
{
    //  Classe : CObjet3D
    //
    //  BUT : 	Classe de base de tous nos objets 3D
    //
    class CObjet3D
    {
    public:
        // Destructeur
        virtual ~CObjet3D() = default;

        virtual void Anime(float)
        {
        };
        virtual void Draw() = 0;
    };
} // namespace PM3D
