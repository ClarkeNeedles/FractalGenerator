/*********************************************************************************************
**
**	File Name:		fractal.cpp
**	Description:	This is the header file that contains the function definiton for all of
**					the fractals.
**
**	Author:			Clarke Needles
**	Created:		6/20/2025
**
**********************************************************************************************/

#include "../App.h"
#include "fractal.h"

void Fractal::UseCPP(Colour* pixelBuffer, int yStart, int yEnd, bool useFloat)
{
    // Variables for updating the x and y values
    // Essentially mapping a complex plane point to a pixel
    double dx = (m_xMax - m_xMin) / static_cast<double>(m_app->m_widthW);
    double dy = (m_yMax - m_yMin) / static_cast<double>(m_app->m_heightW);

    double yval = m_yMin + yStart * dy;
    for (int y = yStart; y < yEnd; ++y)
    {
        double xval = m_xMin;
        for (int x = 0; x < m_app->m_widthW; ++x)
        {
            int n;
            if (useFloat)
            {
                n = GetCPPIterF(static_cast<float>(xval), static_cast<float>(yval));
            }
            else
            {
                n = GetCPPIterD(xval, yval);
            }

            MapColour(&pixelBuffer[y * m_app->m_widthW + x], static_cast<uint8_t>(n));
            xval += dx;
        }
        yval += dy;
    }
}

void Fractal::UseSSE(Colour* pixelBuffer, int yStart, int yEnd, bool useFloat)
{
    if (useFloat)
    {
        float dx = static_cast<float>((m_xMax - m_xMin) / m_app->m_widthW);
        float dy = static_cast<float>((m_yMax - m_yMin) / m_app->m_heightW);
        const __m128 xShift_coeffs = _mm_set_ps(3.0f, 2.0f, 1.0f, 0.0f);
        __m128 yval = _mm_add_ps(_mm_set1_ps(static_cast<float>(m_yMin)), _mm_mul_ps(_mm_set1_ps(static_cast<float>(yStart)), _mm_set1_ps(dy))); // Setting initial yval to startin range of y
        __m128 xShift = _mm_mul_ps(_mm_set1_ps(dx), xShift_coeffs); // Amount to shift x for each number we will be processing
        __m128 dxSSE = _mm_set1_ps(m_sseVectSizeF * dx); // Amount to change multiplied by the number of floats calculated in parallel
        __m128 dySSE = _mm_set1_ps(dy); // The change for y each time will be the default

        for (int y = yStart; y < yEnd; ++y)
        {
            __m128 xval = _mm_add_ps(_mm_set1_ps(static_cast<float>(m_xMin)), xShift); // Initial x values for first floats

            for (int x = 0; x < m_app->m_widthW; x += m_sseVectSizeF) // Increase by the amount of floats being processed each time
            {
                __m128i iter = GetSSEIterF(xval, yval); // Calculate amount of iterations for the floats

                int* n_int = (int*)(&iter); // Pointer to the number of iterations
                int pixel_i = y * m_app->m_widthW + x; // Current pixel index

                // Colour the pixels that are loaded
                for (int i = 0; i < m_sseVectSizeF; ++i, ++pixel_i)
                {
                    uint8_t n = (uint8_t)(n_int[i]); // Changing the pointer to unsigned int
                    MapColour(&pixelBuffer[pixel_i], n);
                }

                xval = _mm_add_ps(xval, dxSSE); // Updating the x values
            }

            yval = _mm_add_ps(yval, dySSE); // Updating the y value
        }
    }
    else
    {
        double dx = (m_xMax - m_xMin) / static_cast<double>(m_app->m_widthW);
        double dy = (m_yMax - m_yMin) / static_cast<double>(m_app->m_heightW);
        __m128d xShift_coeffs = _mm_set_pd(1.0, 0.0); // 2 doubles
        __m128d yval = _mm_add_pd(_mm_set1_pd(m_yMin), _mm_mul_pd(_mm_set1_pd(yStart), _mm_set1_pd(dy))); // Setting initial yval
        __m128d xShift = _mm_mul_pd(_mm_set1_pd(dx), xShift_coeffs); // Amount to shift x for each pair of numbers
        __m128d dxSSE = _mm_set1_pd(m_sseVectSizeD * dx); // Amount to change for 2 doubles at a time
        __m128d dySSE = _mm_set1_pd(dy); // The change in y is the default value

        for (int y = yStart; y < yEnd; ++y) {
            __m128d xval = _mm_add_pd(_mm_set1_pd(m_xMin), xShift); // Initial x values for the first 2 doubles

            for (int x = 0; x < m_app->m_widthW; x += m_sseVectSizeD) { // Increase by the number of doubles being processed per iteration
                __m128i iter = GetSSEIterD(xval, yval); // Calculate iterations for the doubles

                int* n_int = (int*)(&iter); // Pointer to the number of iterations
                int pixel_i = y * m_app->m_widthW + x; // Current pixel index

                // Colour the pixels for the doubles being calculated in parallel
                for (int i = 0; i < m_sseVectSizeD; ++i, ++pixel_i) {
                    uint8_t n = (uint8_t)(n_int[i]); // Changing the pointer to unsigned int
                    MapColour(&pixelBuffer[pixel_i], n);
                }

                xval = _mm_add_pd(xval, dxSSE); // Update the x values
            }

            yval = _mm_add_pd(yval, dySSE); // Update the y value
        }
    }
}

void Fractal::UseAVX(Colour* pixelBuffer, int yStart, int yEnd, bool useFloat)
{
    if (useFloat)
    {
        float dx = static_cast<float>((m_xMax - m_xMin) / m_app->m_widthW);
        float dy = static_cast<float>((m_yMax - m_yMin) / m_app->m_heightW);
        const __m256 xShift_coeffs = _mm256_set_ps(7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f, 0.0f); // Shifting coefficients values
        __m256 yval = _mm256_add_ps(_mm256_set1_ps(static_cast<float>(m_yMin)), _mm256_mul_ps(_mm256_set1_ps(static_cast<float>(yStart)), _mm256_set1_ps(dy))); // Setting initial yval to startin range of y
        __m256 xShift = _mm256_mul_ps(_mm256_set1_ps(dx), xShift_coeffs); // Amount to shift x for each number we will be processing
        __m256 dxSSE = _mm256_set1_ps(m_avxVectSizeF * dx); // Amount to change multiplied by the number of floats calculated in parallel
        __m256 dySSE = _mm256_set1_ps(dy); // The change for y each time will be the default

        for (int y = yStart; y < yEnd; ++y)
        {
            __m256 xval = _mm256_add_ps(_mm256_set1_ps(static_cast<float>(m_xMin)), xShift); // Initial x values for first floats

            for (int x = 0; x < m_app->m_widthW; x += m_avxVectSizeF) // Increase by the amount of floats being processed each time
            {
                __m256i N = GetAVXIterF(xval, yval); // Calculate amount of iterations for the floats

                int* N_int = (int*)(&N); // Pointer to the number of iterations
                int pixel_i = y * m_app->m_widthW + x; // Current pixel index

                // Colour the pixels that are loaded
                for (int i = 0; i < m_avxVectSizeF; ++i, ++pixel_i)
                {
                    uint8_t n = (uint8_t)(N_int[i]); // Changing the pointer to unsigned int
                    MapColour(&pixelBuffer[pixel_i], n);
                }

                xval = _mm256_add_ps(xval, dxSSE); // Updating the x values
            }

            yval = _mm256_add_ps(yval, dySSE); // Updating the y value
        }
    }
    else
    {
        double dx = (m_xMax - m_xMin) / static_cast<double>(m_app->m_widthW);
        double dy = (m_yMax - m_yMin) / static_cast<double>(m_app->m_heightW);
        const __m256d xShift_coeffs = _mm256_set_pd(3.0, 2.0, 1.0, 0.0); // Shifting coefficients values
        __m256d yval = _mm256_add_pd(_mm256_set1_pd(m_yMin), _mm256_mul_pd(_mm256_set1_pd(yStart), _mm256_set1_pd(dy))); // Setting initial yval to startin range of y
        __m256d xShift = _mm256_mul_pd(_mm256_set1_pd(dx), xShift_coeffs); // Amount to shift x for each number we will be processing
        __m256d dxSSE = _mm256_set1_pd(m_avxVectSizeD * dx); // Amount to change multiplied by the number of floats calculated in parallel
        __m256d dySSE = _mm256_set1_pd(dy); // The change for y each time will be the default

        for (int y = yStart; y < yEnd; ++y)
        {
            __m256d xval = _mm256_add_pd(_mm256_set1_pd(m_xMin), xShift); // Initial x values for first floats

            for (int x = 0; x < m_app->m_widthW; x += m_avxVectSizeD) // Increase by the amount of floats being processed each time
            {
                __m256i N = GetAVXIterD(xval, yval); // Calculate amount of iterations for the floats

                int* N_int = (int*)(&N); // Pointer to the number of iterations
                int pixel_i = y * m_app->m_widthW + x; // Current pixel index

                // Colour the pixels that are loaded
                for (int i = 0; i < m_avxVectSizeD; ++i, ++pixel_i)
                {
                    uint8_t n = (uint8_t)(N_int[i]); // Changing the pointer to unsigned int
                    MapColour(&pixelBuffer[pixel_i], n);
                }

                xval = _mm256_add_pd(xval, dxSSE); // Updating the x values
            }

            yval = _mm256_add_pd(yval, dySSE); // Updating the y value
        }
    }
}

void Fractal::MapColour(Colour* pixelBuffer, uint8_t n)
{
    // Color mapping for points outside of the set
    // The gradient depends on the menu option

    UINT gradient = m_app->GetGradient();

    switch (gradient)
    {
    case ID_GRADIENT_1:
    {
        pixelBuffer->r = (n * 13) % 256;
        pixelBuffer->g = (n * 7) % 256;
        pixelBuffer->b = (n * 3) % 256;
        pixelBuffer->a = 0;
        break;
    }
    case ID_GRADIENT_2:
    {
        pixelBuffer->r = (n * 3) % 256;
        pixelBuffer->g = (n * 7) % 256;
        pixelBuffer->b = (n * 13) % 256;
        pixelBuffer->a = 0;
        break;
    }
    case ID_GRADIENT_3:
    {
        pixelBuffer->r = (int)(127.5 * (1 + sin(n * 0.1)));
        pixelBuffer->g = (int)(127.5 * (1 + sin(n * 0.15)));
        pixelBuffer->b = (int)(127.5 * (1 + sin(n * 0.2)));
        pixelBuffer->a = 0;
        break;
    }
    case ID_GRADIENT_4:
    {
        pixelBuffer->r = (int)(sqrt(n * 10)) % 256; // Slower ramp-up for red
        pixelBuffer->g = (int)(log(n + 1) * 50) % 256; // Logarithmic ramp-up for green
        pixelBuffer->b = (n * n) % 256;
        pixelBuffer->a = 0;
        break;
    }
    case ID_GRADIENT_5:
    {
        if (n < 50) {
            pixelBuffer->r = (n * 3) % 256;
            pixelBuffer->g = (n * 2) % 256;
            pixelBuffer->b = 0;
            pixelBuffer->a = 0;
        }
        else if (n < 100) {
            pixelBuffer->r = 0;
            pixelBuffer->g = (n * 4) % 256;
            pixelBuffer->b = (n * 6) % 256;
            pixelBuffer->a = 0;
        }
        else {
            pixelBuffer->r = (n * 8) % 256;
            pixelBuffer->g = (n * 3) % 256;
            pixelBuffer->b = (n * 1) % 256;
            pixelBuffer->a = 0;
        }
        break;
    }
    case ID_GRADIENT_6:
    {
        int r = (n * n) % 256;
        int g = (n * 3 + 50) % 256;
        int b = (n * 7 - 30) % 256;
        int a = 0;
        pixelBuffer->r = (r + b) % 256;
        pixelBuffer->g = (g + r) % 256;
        pixelBuffer->b = (b + g) % 256;
        pixelBuffer->a = 0;
        break;
    }
    case ID_GRADIENT_7:
    {
        pixelBuffer->r = (255 - ((n * 1) % 256));
        pixelBuffer->g = (255 - ((n * 5) % 256));
        pixelBuffer->b = (255 - ((n * 10) % 256));
        pixelBuffer->a = 0;
        break;
    }
    } // Switch
}

void Fractal::Render(Colour* pixelBuffer)
{
    // Dynamically changing from float to double when resolution gets low
    bool useDouble = m_yMax - m_yMin < m_floatToDouble;

    UINT language = m_app->GetLanguage();

    switch (language)
    {
    // Use multithreading
    case ID_LANGUAGE_CPP_MT:
    case ID_LANGUAGE_SSE_MT:
    case ID_LANGUAGE_AVX_MT:
    {
        int numThreads;
        int cores = std::thread::hardware_concurrency(); // Number of available CPU cores
        if (ID_LANGUAGE_AVX_MT == language)
        {
            // Hard coded value that actually speeds up AVX Multithreaded
            // Using to many cores is uncessary and slows it down, 8 is perfect
            numThreads = cores < 8 ? cores : 8;
        }
        else
        {
            numThreads = cores;
        }

        int stripHeight = m_app->m_heightW / numThreads;
        std::vector<std::thread> threads;

        // Assign a thread to each strip
        for (int i = 0; i < numThreads; ++i) {
            int yStart = i * stripHeight;

            if (useDouble)
            {
                switch (language)
                {
                case ID_LANGUAGE_CPP_MT:
                {
                    threads.emplace_back(std::bind(
                        &Fractal::UseCPP,
                        this,
                        pixelBuffer,
                        yStart,
                        yStart + stripHeight,
                        false));
                    break;
                }
                case ID_LANGUAGE_SSE_MT:
                {
                    threads.emplace_back(std::bind(
                        &Fractal::UseSSE,
                        this,
                        pixelBuffer,
                        yStart,
                        yStart + stripHeight,
                        false));
                    break;
                }
                case ID_LANGUAGE_AVX_MT:
                {
                    threads.emplace_back(std::bind(
                        &Fractal::UseAVX,
                        this,
                        pixelBuffer,
                        yStart,
                        yStart + stripHeight,
                        false));
                    break;
                }
                } // Switch
            }
            else
            {
                switch (language)
                {
                case ID_LANGUAGE_CPP_MT:
                {
                    threads.emplace_back(std::bind(
                        &Fractal::UseCPP,
                        this,
                        pixelBuffer,
                        yStart,
                        yStart + stripHeight,
                        true));
                    break;
                }
                case ID_LANGUAGE_SSE_MT:
                {
                    threads.emplace_back(std::bind(
                        &Fractal::UseSSE,
                        this,
                        pixelBuffer,
                        yStart,
                        yStart + stripHeight,
                        true));
                    break;
                }
                case ID_LANGUAGE_AVX_MT:
                {
                    threads.emplace_back(std::bind(
                        &Fractal::UseAVX,
                        this,
                        pixelBuffer,
                        yStart,
                        yStart + stripHeight,
                        true));
                    break;
                }
                } // Switch
            }
        }

        // Wait for all threads to complete
        for (auto& t : threads) {
            t.join();
        }

        break;
    }
    case ID_LANGUAGE_CPP:
    case ID_LANGUAGE_SSE:
    case ID_LANGUAGE_AVX:
    {
        if (useDouble)
        {
            switch (language)
            {
            case ID_LANGUAGE_CPP:
            {
                UseCPP(pixelBuffer, 0, m_app->m_heightW, false);
                break;
            }
            case ID_LANGUAGE_SSE:
            {
                UseSSE(pixelBuffer, 0, m_app->m_heightW, false);
                break;
            }
            case ID_LANGUAGE_AVX:
            {
                UseAVX(pixelBuffer, 0, m_app->m_heightW, false);
                break;
            }
            } // Switch
        }
        else
        {
            switch (language)
            {
            case ID_LANGUAGE_CPP:
            {
                UseCPP(pixelBuffer, 0, m_app->m_heightW, true);
                break;
            }
            case ID_LANGUAGE_SSE:
            {
                UseSSE(pixelBuffer, 0, m_app->m_heightW, true);
                break;
            }
            case ID_LANGUAGE_AVX:
            {
                UseAVX(pixelBuffer, 0, m_app->m_heightW, true);
                break;
            }
            } // Switch
        }

        break;
    }
    } // Switch
}

void Fractal::Draw(HDC hdc, Colour* pixelBuffer, GifWriter* gif, bool recording)
{
    // Define the bitmap
    BITMAPINFO bmpInfo;
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = m_app->m_widthW;
    bmpInfo.bmiHeader.biHeight = -m_app->m_heightW; // Negative to indicate top-down bitmap
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 32; // 32 bits per pixel (COLORREF format)
    bmpInfo.bmiHeader.biCompression = BI_RGB;

    // Transfer the pixelBuffer to the screen
    SetDIBitsToDevice(
        hdc,
        0, 0, m_app->m_widthW, m_app->m_heightW,         // Destination rectangle on the screen
        0, 0, 0, m_app->m_heightW,             // Source rectangle in the buffer
        pixelBuffer,                 // Pixel buffer source
        &bmpInfo,                    // Bitmap information
        DIB_RGB_COLORS               // Color format (RGB)
    );

    if (recording)
    {
        GifWriteFrame(gif, (uint8_t*)pixelBuffer, m_app->m_widthW, m_app->m_heightW, m_app->m_gifDelay);
    }
}

void Fractal::ZoomScreen(ZoomType zoomType)
{
    const double xRange = m_xMax - m_xMin, yRange = m_yMax - m_yMin;
    const double midPx = (m_xMax + m_xMin) / 2, midPy = (m_yMax + m_yMin) / 2;
    double dx = 0, dy = 0;

    // Zoom in/out according to the zoom factor
    switch (zoomType)
    {
    case ZoomType::ZOOM_IN:
    {
        // Calculating the change in x and y
        // Based on the zoom factor
        dx = xRange / m_zoomFactor / 2;
        dy = yRange / m_zoomFactor / 2;

        break;
    }
    case ZoomType::ZOOM_OUT:
    {
        // Update the boundaries of the complex plane
        // Based on the zoom factor
        dx = xRange * m_zoomFactor / 2;
        dy = yRange * m_zoomFactor / 2;

        break;
    }
    } // Switch

    // Updating x/y values with changes
    m_xMin = midPx - dx;
    m_xMax = midPx + dx;
    m_yMin = midPy - dy;
    m_yMax = midPy + dy;
}

void Fractal::MoveScreen(POINT* clickPoint)
{
    // Finding the current ranges of the window
    const double xRange = m_xMax - m_xMin, yRange = m_yMax - m_yMin;

    // Mapping the window pos to a complex plane pos
    // This will be the new center of the screen
    double xMouse = m_xMin + (clickPoint->x / static_cast<double>(m_app->m_widthW)) * (xRange);
    double yMouse = m_yMin + (clickPoint->y / static_cast<double>(m_app->m_heightW)) * (yRange);

    // Update the boundaries of the complex plane
    // Based on the pos of the mouse click
    m_xMin = xMouse - xRange / 2.0;
    m_xMax = xMouse + xRange / 2.0;
    m_yMin = yMouse - yRange / 2.0;
    m_yMax = yMouse + yRange / 2.0;
}