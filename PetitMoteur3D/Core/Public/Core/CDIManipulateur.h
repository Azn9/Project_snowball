﻿#pragma once

#include <dinput.h>

class CDIManipulateur
{
public:
    CDIManipulateur()
    {
        pDirectInput = nullptr;
        pClavier = nullptr;
        pSouris = nullptr;
        pJoystick = nullptr;
    }
    ~CDIManipulateur();
    
    void StatutClavier();
    bool ToucheAppuyee(UINT touche);

    void SaisirEtatSouris();
    const DIMOUSESTATE& EtatSouris() { return mouseState;}

    bool Init(HINSTANCE hInstance, HWND hWnd);
private:
    IDirectInput8* pDirectInput;
    IDirectInputDevice8* pClavier;
    IDirectInputDevice8* pSouris;
    IDirectInputDevice8* pJoystick;
    static bool bDejaInit;

    char tamponClavier[256];
    DIMOUSESTATE mouseState;
};
