#pragma once
#include "Horloge.h"
#include "Singleton.h"

namespace PM3D
{
constexpr int ImagePerSecond = 60;
constexpr double EcartTemps = 1.0 / static_cast<double>(ImagePerSecond);

//
//   TEMPLATE : CMoteur
//
//   BUT : Template servant � construire un objet Moteur qui implantera les
//         aspects "g�n�riques" du moteur de jeu
//
//   COMMENTAIRES :
//
//        Comme plusieurs de nos objets repr�senteront des �l�ments uniques 
//        du syst�me (ex: le moteur lui-m�me, le lien vers 
//        le dispositif Direct3D), l'utilisation d'un singleton 
//        nous simplifiera plusieurs aspects.
//
template <class T> class CMoteur : public CSingleton<T>
{
public:
	virtual void Run()
	{
		bool bBoucle = true;

		while (bBoucle)
		{
			// Propre � la plateforme - (Conditions d'arr�t, interface, messages)
			bBoucle = RunSpecific();

			// appeler la fonction d'animation
			if (bBoucle)
			{
				bBoucle = Animation();
			}
		}
	}

	virtual int Initialisations()
	{
		// Propre � la plateforme
		InitialisationsSpecific();

		// * Initialisation du dispositif de rendu

		// * Initialisation de la sc�ne

		// * Initialisation des param�tres de l'animation et 
		//   pr�paration de la premi�re image
		InitAnimation();

		return 0;
	}

	virtual bool Animation()
	{
		// m�thode pour lire l�heure et calculer le
		// temps �coul�
		const int64_t TempsCompteurCourant = GetTimeSpecific();
		const double TempsEcoule = GetTimeIntervalsInSec(TempsCompteurPrecedent, TempsCompteurCourant);
		// Est-il temps de rendre l�image ?
		if (TempsEcoule > EcartTemps)
		{
			// Affichage optimis�
			// pDispositif->Present() ; // On enlevera � // � plus tard
			// On pr�pare la prochaine image
			// AnimeScene(TempsEcoule) ;
			// On rend l�image sur la surface de travail
			// (tampon d�arri�re plan)
			RenderScene();
			// Calcul du temps du prochain affichage
			TempsCompteurPrecedent = TempsCompteurCourant;
		}

		return true;
	}

protected:
	virtual ~CMoteur() = default;

	Horloge m_Horloge;

	// Variables pour le temps de l'animation
	int64_t TempsSuivant = 0;
	int64_t TempsCompteurPrecedent = 0;

	// Sp�cifiques - Doivent �tre implant�s
	virtual bool RunSpecific() = 0;
	virtual int InitialisationsSpecific() = 0;

	virtual int InitAnimation()
	{
		TempsSuivant = GetTimeSpecific();

		TempsCompteurPrecedent = TempsSuivant;

		// premi�re Image
		RenderScene();

		return true;
	}

	virtual bool RenderScene()
	{
		return true;
	}

	virtual int64_t GetTimeSpecific() =0;
	virtual double GetTimeIntervalsInSec(int64_t start, int64_t stop) = 0;

};

} // namespace PM3D
