/*********************************************************************************************
**
**	File Name:		App.cpp
**	Description:	This is the main file that runs the window and handles all the
**                  events
**
**	Author:			Clarke Needles
**	Created:		7/29/2025
**
**********************************************************************************************/

#include "App.h"

App::App() : m_hWnd(nullptr)
{
    // Initialize performance frequency
    if (!QueryPerformanceFrequency(&m_liFrequency))
    {
        MessageBox(NULL, _T("QueryPerformanceFrequency failed!"), NULL, NULL);
    }
}

App::~App()
{
    _aligned_free(m_pixelBuffer);
}

int App::Run(HINSTANCE hInstance, int nCmdShow)
{
    // Define the window class
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = StaticWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = m_pszWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    // Register the window class
    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed!"),
            NULL,
            NULL);

        return 1;
    }

    m_hInst = hInstance;

    // Create the window
    m_hWnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        m_pszWindowClass,
        m_pszTitle,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT,
        m_widthW, m_heightW,
        NULL,
        NULL,
        hInstance,
        this
    );

    if (!m_hWnd)
    {
        MessageBox(NULL,
            _T("Call to CreateWindow failed!"),
            NULL,
            NULL);

        return 1;
    }

    // Add app menu
    HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_APP_MENU));
    if (hMenu) {
        SetMenu(m_hWnd, hMenu);
    }
    else {
        MessageBox(m_hWnd, _T("Failed to load menu."), _T("Error"), MB_OK | MB_ICONERROR);
    }

    // Store pointer to this instance in window's user data
    SetWindowLongPtr(m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    ShowWindow(m_hWnd,
        nCmdShow);
    UpdateWindow(m_hWnd);

    // Main message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

UINT App::GetLanguage()
{
    return m_menuOptionsOn.m_language;
}

UINT App::GetFractal()
{
    return m_menuOptionsOn.m_fractal;
}

UINT App::GetGradient()
{
    return m_menuOptionsOn.m_gradient;
}

LRESULT CALLBACK App::StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    App* pThis = nullptr;

    if (message == WM_NCCREATE)
    {
        // Extract the App pointer passed via CreateWindowEx (lParam contains `CREATESTRUCT*`)
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = reinterpret_cast<App*>(pCreate->lpCreateParams);

        // Associate the App instance with the window handle
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else
    {
        // Retrieve the App instance associated with the window
        pThis = reinterpret_cast<App*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    // Forward the message to WndProc
    if (pThis)
    {
        return pThis->WndProc(hWnd, message, wParam, lParam);
    }

    // Default handling for messages without an associated instance
    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT App::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        auto hdc = BeginPaint(hWnd, &m_ps);

        if (m_bRender)
        {
            // Transfer off-screen bitmap onto the window
            m_fractal->Draw(hdc, m_pixelBuffer, &m_gif, m_bRecording);
            // The image has been generated to the window
            m_bCanZoom = true;

            if (m_bTimer)
            {
                // End timer once rendered and loaded
                QueryPerformanceCounter(&m_liEndTime);

                m_liTicks.QuadPart = m_liEndTime.QuadPart - m_liStartTime.QuadPart;
                double dSeconds = static_cast<double>(m_liTicks.QuadPart) / m_liFrequency.QuadPart;
                std::wstring strText = std::format(L"{:.2f} ms", dSeconds * 1000);

                TextOut(hdc, 0, m_heightW - 79, strText.c_str(), static_cast<int>(strText.length()));

                m_bTimer = false;
            }

            m_bRender = false;
        }

        EndPaint(hWnd, &m_ps);
        break;
    }
    case WM_COMMAND:
    {
        auto param = LOWORD(wParam);
        switch (param)
        {
        case ID_RENDER_GENERATE:
        {
            // Start timer at the start of render
            m_bTimer = true;
            QueryPerformanceCounter(&m_liStartTime);

            // Render the selected fractal to the pixel buffer
            // Resetting the complex plane zoom size after generation button
            switch (m_menuOptionsOn.m_fractal)
            {
            case ID_FRACTAL_MANDELBROT:
            {
                m_fractal = std::make_unique<Mandelbrot>(shared_from_this());
                break;
            }
            case ID_FRACTAL_BURNINGSHIP:
            {
                m_fractal = std::make_unique<BurningShip>(shared_from_this());
                break;
            }
            case ID_FRACTAL_MULTIBROT:
            {
                m_fractal = std::make_unique<Multibrot>(shared_from_this());
                break;
            }
            case ID_FRACTAL_NOVA:
            {
                m_fractal = std::make_unique<Nova>(shared_from_this());
                break;
            }
            case ID_FRACTAL_PHEONIX:
            {
                m_fractal = std::make_unique<Pheonix>(shared_from_this());
                break;
            }
            } // Switch

            // Rendering the mandelbrot to the pixel buffer
            m_fractal->Render(m_pixelBuffer);

            // Painting to the window
            // Force a repaint to transfer the bitmap buffer to the window
            m_bRender = true;
            InvalidateRect(hWnd, NULL, TRUE);

            break;
        }
        case ID_RENDER_RECORD:
        {
            wchar_t folderPath[MAX_PATH] = { 0 };
            wchar_t text[256]; // Text buffer for getting the button text
            HWND hRecButton = GetDlgItem(hWnd, ID_RENDER_RECORD);
            GetWindowText(hRecButton, text, sizeof(text));

            if (!wcscmp(text, L"Start Recording"))
            {
                // Initialize BROWSEINFO structure
                BROWSEINFO bi = { 0 };
                bi.lpszTitle = L"Select a Folder"; // Title of the folder selection dialog
                bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE; // Return only folders, use modern dialog

                // Display the dialog box
                LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

                if (pidl)
                {
                    // Get the folder path from the item ID list
                    if (!SHGetPathFromIDList(pidl, folderPath))
                    {
                        MessageBox(NULL,
                            _T("Failed to get folder path."),
                            NULL,
                            NULL);

                        return 1;
                    }

                    // Free the PIDL allocated by SHBrowseForFolder
                    CoTaskMemFree(pidl);
                }
                else
                {
                    // No folder selected, exit
                    break;
                }

                SendMessage(hRecButton, WM_SETTEXT, 0, (LPARAM)L"Stop Recording");

                // This is converting the file path wchar_t to a char for GifBegin()
                wcscat_s(folderPath, L"\\output.gif"); // Add destination file to folder path
                size_t charLen = 0;
                wcstombs_s(&charLen, NULL, 0, folderPath, _TRUNCATE); // Get length for buffer
                char* fullPath = new char[charLen + 1];
                wcstombs_s(NULL, fullPath, charLen + 1, folderPath, _TRUNCATE); // Convet wchar_t to char

                // Start adding to the gif
                GifBegin(&m_gif, fullPath, m_widthW, m_heightW, m_gifDelay);
                m_bRecording = true;
            }
            else
            {
                SendMessage(hRecButton, WM_SETTEXT, 0, (LPARAM)L"Start Recording");

                GifEnd(&m_gif);
                m_bRecording = false;
            }

            break;
        }
        case ID_LANGUAGE_CPP:
        case ID_LANGUAGE_SSE:
        case ID_LANGUAGE_AVX:
        case ID_LANGUAGE_CPP_MT:
        case ID_LANGUAGE_SSE_MT:
        case ID_LANGUAGE_AVX_MT:
        {
            HMENU hMenu = GetMenu(hWnd);

            CheckMenuItem(hMenu, m_menuOptionsOn.m_language, MF_UNCHECKED);
            CheckMenuItem(hMenu, param, MF_CHECKED);

            m_menuOptionsOn.m_language = param;

            break;
        }
        case ID_FRACTAL_MANDELBROT:
        case ID_FRACTAL_BURNINGSHIP:
        case ID_FRACTAL_MULTIBROT:
        case ID_FRACTAL_NOVA:
        case ID_FRACTAL_PHEONIX:
        {
            HMENU hMenu = GetMenu(hWnd);

            CheckMenuItem(hMenu, m_menuOptionsOn.m_fractal, MF_UNCHECKED);
            CheckMenuItem(hMenu, param, MF_CHECKED);

            m_menuOptionsOn.m_fractal = param;

            break;
        }
        case ID_GRADIENT_1:
        case ID_GRADIENT_2:
        case ID_GRADIENT_3:
        case ID_GRADIENT_4:
        case ID_GRADIENT_5:
        case ID_GRADIENT_6:
        case ID_GRADIENT_7:
        {
            HMENU hMenu = GetMenu(hWnd);

            CheckMenuItem(hMenu, m_menuOptionsOn.m_gradient, MF_UNCHECKED);
            CheckMenuItem(hMenu, param, MF_CHECKED);

            m_menuOptionsOn.m_gradient = param;

            break;
        }
        } // Switch

        break;
    }
    case WM_LBUTTONDOWN:
    {
        if (m_bCanZoom)
        {
            GetCursorPos(&m_clickPoint);       // Retrieve screen coordinates
            ScreenToClient(hWnd, &m_clickPoint); // Convert to client coordinates

            m_fractal->MoveScreen(&m_clickPoint);

            // Rendering the mandelbrot to the pixel buffer
            m_fractal->Render(m_pixelBuffer);

            // Painting to the window
            // Force a repaint to transfer the bitmap buffer to the window
            m_bRender = true;
            InvalidateRect(hWnd, NULL, TRUE);

        }

        break;
    }
    case WM_MOUSEWHEEL:
    {
        if (m_bCanZoom)
        {
            int wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);

            // Scrolling up/down (zoomin in/out)
            m_fractal->ZoomScreen(wheelDelta > 0 ? Fractal::ZoomType::ZOOM_IN : Fractal::ZoomType::ZOOM_OUT);

            // Rendering the mandelbrot to the pixel buffer
            m_fractal->Render(m_pixelBuffer);

            // Painting to the window
            // Force a repaint to transfer the bitmap buffer to the window
            m_bRender = true;
            InvalidateRect(hWnd, NULL, TRUE);
        }

        break;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }
    default:
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    } // Switch

    return 0;
}

// Entry
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    auto app = std::make_shared<App>();

    return app->Run(hInstance, nCmdShow);
}