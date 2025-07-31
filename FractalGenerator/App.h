/*********************************************************************************************
**
**	File Name:		App.h
**	Description:	This is the main header file that runs the window and handles all the
**                  events
**
**	Author:			Clarke Needles
**	Created:		7/29/2025
**
**********************************************************************************************/

#pragma once

#include <Windows.h>
#include <tchar.h>
#include <cmath>
#include <ShlObj.h>
#include <stdlib.h>
#include <memory>
#include <stdint.h>
#include <filesystem>
#include "Resource.h"
#include "Fractals/Fractal.h"
#include "Fractals/Fractals.h"

class App : public std::enable_shared_from_this<App>
{
public:
    int m_widthW = 900;
    int m_heightW = 600;

    int m_widthB = 128;
    int m_heightB = 25;

    int m_gifDelay = 10;

private:
    // Window variables
    HWND m_hWnd{};
    HINSTANCE m_hInst{};
    LPCWSTR m_pszWindowClass = _T("FractalApp");
    LPCWSTR m_pszTitle = _T("Fractal Playground App");

    // Alrogrithm timing variables
    LARGE_INTEGER m_liFrequency{};
    LARGE_INTEGER m_liStartTime{};
    LARGE_INTEGER m_liEndTime{};
    LARGE_INTEGER m_liTicks{};

    // App menu variables
    struct MenuOptions
    {
        UINT m_language = ID_LANGUAGE_CPP;
        UINT m_fractal = ID_FRACTAL_MANDELBROT;
        UINT m_gradient = ID_GRADIENT_1;
    } m_menuOptionsOn;

    // App related variables
    bool m_bRender{};
    bool m_bTimer{};
    bool m_bCanZoom{};
    bool m_bRecording{};

    // WndProc variables
    PAINTSTRUCT m_ps{};
    POINT m_clickPoint{};
    Colour* m_pixelBuffer = (Colour*)_aligned_malloc(sizeof(Colour) * m_widthW * m_heightW, 32);
    GifWriter m_gif = { NULL, NULL, NULL };
    std::unique_ptr<Fractal> m_fractal;

public:
    App();

    ~App();

    // App entry
    int Run(HINSTANCE hInstance, int nCmdShow);

    // Get methods
    UINT GetLanguage();
    UINT GetFractal();
    UINT GetGradient();

private:
    // Static WndProc callback
    static LRESULT CALLBACK StaticWndProc(HWND, UINT, WPARAM, LPARAM);

    // Non-static callback
    LRESULT WndProc(HWND, UINT, WPARAM, LPARAM);
};
