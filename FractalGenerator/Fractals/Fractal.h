/*********************************************************************************************
**
**	File Name:		fractal.h
**	Description:	This is the header file that contains the abstract class for all
**                  of the fractals
**
**	Author:			Clarke Needles
**	Created:		5/23/2025
**
**********************************************************************************************/

#pragma once

#include <thread>
#include <vector>
#include <string>
#include <Windows.h>
#include <functional>
#include <immintrin.h>
#include <emmintrin.h>
#include "../Colour.h"
#include "../Gif.h"
#include "../Resource.h"

class App;

class Fractal
{
private:
    std::shared_ptr<App> m_app;

    double m_xMin, m_xMax, m_yMin, m_yMax;

protected:
    // Fractal iterations
    const int m_maxIterations = 10000;

    // Escape boundary value
    const float m_rMax = 4.0;

    // SSE and AVX
    // SSE uses 128 bit reg's --> fits 2 64 bit doubles, and 4 32 bit floats
    // AVX uses 256 bit reg's --> fits 4 64 bit doubles, and 8 32 bit floats
    const short int m_sseVectSizeD = 2;
    const short int m_sseVectSizeF = 4;
    const short int m_avxVectSizeD = 6;
    const short int m_avxVectSizeF = 8;

    // Switching condition float --> double
    // When resolution gets low
    const float m_floatToDouble = 0.0001f;

    // Zoom factor
    const float m_zoomFactor = 1.5f;

public:
    enum class ZoomType
    {
        ZOOM_IN,
        ZOOM_OUT
    };

private:
    // FOR RENDERING WITH CPP //

    // Determining iterations with CPP with floats
    virtual int GetCPPIterF(float, float) const = 0;

    // Determining iterations with CPP with doubles
    virtual int GetCPPIterD(double, double) const = 0;

    // Determining if a point is apart of the fractal in C++
    void UseCPP(
        Colour* pixelBuffer,
        int yStart,
        int yEnd,
        bool useFloat);


    // FOR RENDERING WITH SSE //

    // Determining iterations with SSE with floats
    virtual __m128i GetSSEIterF(__m128, __m128) const = 0;

    // Determining iterations with SSE with doubles
    virtual __m128i GetSSEIterD(__m128d, __m128d) const = 0;

    // Determining if a point is apart of the fractal in SSE
    void UseSSE(
        Colour* pixelBuffer,
        int yStart,
        int yEnd,
        bool useFloat);


    // FOR RENDERING WITH AVX //

    // Determining iterations with AVX with floats
    virtual __m256i GetAVXIterF(__m256, __m256) const = 0;

    // Determining iterations with AVX with doubles
    virtual __m256i GetAVXIterD(__m256d, __m256d) const = 0;

    // Determining if a point is apart of the fractal in AVX
    void UseAVX(
        Colour* pixelBuffer,
        int yStart,
        int yEnd,
        bool useFloat);


    // HELPER FUNCTIONS //

    // Map iterations to a gradient
    void MapColour(
        Colour* pixelBuffer,
        uint8_t n);

public:
    Fractal(std::shared_ptr<App> app, double xMin, double xMax, double yMin, double yMax)
        : m_app(std::move(app)), m_xMin(xMin), m_xMax(xMax), m_yMin(yMin), m_yMax(yMax)
    {
    }

    ~Fractal()
    {
    }

    // Function to render the fractal (May use multithreading depending on user selection)
    void Render(Colour* pixelBuffer);

    // Transferring the pixelBuffer bitmap to the main screen and writing to the gif
    void Draw(
        HDC hdc,
        Colour* pixelBuffer,
        GifWriter* gif,
        bool recording);

    // Zooming in on the current fractal
    void ZoomScreen(ZoomType zoom);

    // Moving the screen on the current fractal
    void MoveScreen(POINT* clickPoint);
};
