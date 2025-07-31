/*********************************************************************************************
**
**	File Name:		mandelbrot.h
**	Description:	This is the header file that contains the class definition for the
**                  mandelbrot fractal
**
**	Author:			Clarke Needles
**	Created:		5/23/2025
**
**********************************************************************************************/

#pragma once

#include "Fractal.h"

class Mandelbrot : public Fractal
{
private:
    int GetCPPIterF(float xval, float yval) const override;

    int GetCPPIterD(double xval, double yval) const override;

    __m128i GetSSEIterF(__m128 xval, __m128 yval) const override;

    __m128i GetSSEIterD(__m128d xval, __m128d yval) const override;

    __m256i GetAVXIterF(__m256 xval, __m256 yval) const override;

    __m256i GetAVXIterD(__m256d xval, __m256d yval) const override;

public:
    Mandelbrot(std::shared_ptr<App> app) : Fractal(app, -2.5, 1.5, -1.5, 1.75)
    {
    }

    ~Mandelbrot() {}
};