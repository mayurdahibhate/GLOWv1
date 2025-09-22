#include <glad/glad.h>
#include <GLOW/window.hpp>

#ifdef WIN32
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// code
	switch (iMsg)
	{
	case WM_SETFOCUS:
		_gbActive = TRUE;
		break;
	case WM_KILLFOCUS:
		_gbActive = FALSE;
		break;
	case WM_SIZE:
        if (_reshapeCallback)
        {
		    _reshapeCallback(LOWORD(lParam), HIWORD(lParam));
        }
		break;
	case WM_ERASEBKGND:
		return(0);
	case WM_KEYDOWN:
        if(_keyboardCallback) 
		{
            _keyboardCallback(wParam);
		}
        break;
	case WM_CHAR:
		switch (LOWORD(wParam))
		{
		case 'F':
		case 'f':
			if (_gbFullscreen == FALSE)
			{
				ToggleFullscreen();
				_gbFullscreen = TRUE;
			}
			else
			{
				ToggleFullscreen();
				_gbFullscreen = FALSE;
			}
			break;
		case 27:
			DestroyWindow(hwnd);
			break;
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}
 
void ToggleFullscreen(void)
{
	// local variable declarations
	MONITORINFO mi = { sizeof(MONITORINFO) };

	// code
	if (_gbFullscreen == FALSE)
	{
		_dwStyle = GetWindowLong(_ghwnd, GWL_STYLE);

		if (_dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(_ghwnd, &_wpPrev) && GetMonitorInfo(MonitorFromWindow(_ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(_ghwnd, GWL_STYLE, _dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(_ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}

		ShowCursor(FALSE);
	}
	else
	{
		SetWindowPlacement(_ghwnd, &_wpPrev);
		SetWindowLong(_ghwnd, GWL_STYLE, _dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPos(_ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
}

int initialize(void)
{
	// variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;

	// code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	// initialization of PI
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	// get the DC
	ghdc = GetDC(_ghwnd);
	if (ghdc == NULL)
	{
		// fprintf(gpFile, "GetDC() failed.\n");
		return(-1);
	}

	// get pixel format
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);

	if (iPixelFormatIndex == 0)
	{
		// fprintf(gpFile, "ChoosePixelFormat() failed.\n");
		return(-2);
	}

	// set obtained pixel format
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE) {
		// fprintf(gpFile, "SetPixelFormat() failed.\n");
		return(-3);
	}

	// create OpenGL context from Device Context
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		// fprintf(gpFile, "wglCreateContext() failed.\n");
		return(-4);
	}

	// make rendering context current
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		// fprintf(gpFile, "wglMakeCurrent() failed.\n");
		return(-5);
	}

    if(_initializeCallback)
    {
        _initializeCallback();
    }

	// warm up resize
    if(_reshapeCallback) 
    {
        _reshapeCallback(_gWidth, _gHeight);
	}

	return(0);
}

void glowEventLoop()
{
    // variables
    BOOL bDone = FALSE;
    MSG msg;

    // code
    while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (_gbActive == TRUE) 
            {
                if (_updateCallback)
                    _updateCallback();
            }

            if (_displayCallback)
			{
			    _displayCallback();
			}
		}
    }    
}


void _uninitialize(void)
{
    if (_uninitializeCallback)
        _uninitializeCallback();

    	// fullscreen check
	if (_gbFullscreen == TRUE)
	{
		SetWindowLong(_ghwnd, GWL_STYLE, _dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(_ghwnd, &_wpPrev);
		SetWindowPos(_ghwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);

		ShowCursor(TRUE);
	}

	// break the current context
	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (ghrc)
	{
		wglDeleteContext(ghrc);
	}

	if (ghdc)
	{
		ReleaseDC(_ghwnd, ghdc);
		ghdc = NULL;
	}

}

GLOWwindow* glowCreateWindow(const char *title, int x, int y, int width, int height) {
    // variables 
	int iRet = 0;
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;

    _gWidth = width;
    _gHeight = height;

    HINSTANCE hInstance = (HINSTANCE) GetModuleHandle(NULL);

    size_t size = strlen(title) + 1;
	TCHAR *szClassName = new TCHAR[size];
    szClassName = (TCHAR*)title;

	// code

	// WNDCLASSEX initialization
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	// register WNDCLASSEX
	RegisterClassEx(&wndclass);

	// create window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		szClassName,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		(GetSystemMetrics(SM_CXSCREEN) - _gWidth) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - _gHeight) / 2,
		_gWidth,
		_gHeight,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	_ghwnd = hwnd;

	iRet = initialize();
	if (iRet == -1)
	{
		// fprintf(_gpFile, "ChoosePixelFormat failed...\n");
		DestroyWindow(hwnd);
	}
	else if (iRet == -2)
	{
		// fprintf(_gpFile, "SetPixelFormat failed...\n");
		DestroyWindow(hwnd);
	}
	else if (iRet == -3)
	{
		// fprintf(_gpFile, "wglCreateContext failed...\n");
		DestroyWindow(hwnd);
	}
	else if (iRet == -4)
	{
		// fprintf(_gpFile, "wglMakeCurrent failed...\n");
		DestroyWindow(hwnd);
	}
	else
	{
		// fprintf(_gpFile, "initialize() successful...\n");
	}

	// show the window
	ShowWindow(hwnd, SW_NORMAL);

	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
}

void glowSwapBuffers(void)
{
	SwapBuffers(ghdc);
}

#else

GLOWwindow* glowCreateWindow(const char *title, int x, int y, int width, int height) {
   	
    GLOWwindow *newWindow = new GLOWwindow();
    
    // local variable declarations
    int defaultScreen;
    XSetWindowAttributes windowAtrributes;
    int styleMask;
    Atom windowMangerDelete;

    int screenWidth, screenHeight;
    static int winWidth, winHeight;
    char keys[26];

    int frameBufferAtrributes[] = {
        GLX_DOUBLEBUFFER, True,
        GLX_X_RENDERABLE, True,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_STENCIL_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        None
    };
    
    Bool bDone = False;
    int result = 0;

    // programmable pipeline related variables
    GLXFBConfig bestGLXconfig;
    GLXFBConfig *glxFBConfigs = NULL;
    XVisualInfo *tempXVisualInfo = NULL;
    int numFBConfigs = 0;

    int bestFrameBufferConfig = -1;
    int bestNumOfSamples = -1;
    int worstFrameBufferConfig = -1;
    int worstNumOfSamples = 999;
    
    int sampleBuffers = 0;
    int samples = 0;
    int i = 0;

    // code
    // open a connection with X server and get display interface
    newWindow->display = XOpenDisplay(NULL);

    newWindow->_gWidth = width;
    newWindow->_gHeight = height;

    if (newWindow->display == NULL)
    {
        exit(1);
    }

    // get default screen from above display
    defaultScreen = XDefaultScreen(newWindow->display);

    // get available FBconfigs
    glxFBConfigs = glXChooseFBConfig(newWindow->display, defaultScreen, frameBufferAtrributes, &numFBConfigs);

    if (glxFBConfigs == NULL)
    {
        exit(-1);
    }

    // find best matching FBConfig from array obtained from above
    for (i = 0; i < numFBConfigs; i++)
    {
        tempXVisualInfo = glXGetVisualFromFBConfig(newWindow->display, glxFBConfigs[i]);

        if (tempXVisualInfo != NULL)
        {
            // get sample buffers
            glXGetFBConfigAttrib(newWindow->display, glxFBConfigs[i], GLX_SAMPLE_BUFFERS, &sampleBuffers);

            glXGetFBConfigAttrib(newWindow->display, glxFBConfigs[i], GLX_SAMPLES, &samples);

            if (bestFrameBufferConfig < 0 || sampleBuffers && samples > bestNumOfSamples)
            {
                bestFrameBufferConfig = i;
                bestNumOfSamples = samples;
            }

            if (worstFrameBufferConfig < 0 || !sampleBuffers || samples < worstNumOfSamples)
            {
                worstFrameBufferConfig = i;
                worstNumOfSamples = samples;
            }

            XFree(tempXVisualInfo);
            tempXVisualInfo = NULL;
        }
    }

    // accordingly get best GLXFBconfig
    bestGLXconfig = glxFBConfigs[bestFrameBufferConfig];

    // assign to global glxConfig
    newWindow->glxFBConfig = bestGLXconfig;
    
    XFree(glxFBConfigs);

    // now get best visual bestFBConfig
    newWindow->visualInfo = glXGetVisualFromFBConfig(newWindow->display, newWindow->glxFBConfig);

    // set window attriubtes / properties
    memset((void*)&windowAtrributes, 0, sizeof(XWindowAttributes));
    windowAtrributes.border_pixel = 0;
    windowAtrributes.background_pixel = XBlackPixel(newWindow->display, newWindow->visualInfo->screen);
    windowAtrributes.background_pixmap = 0;
    windowAtrributes.colormap = XCreateColormap(newWindow->display,
                                    XRootWindow(newWindow->display, newWindow->visualInfo->screen),
                                    newWindow->visualInfo->visual,
                                    AllocNone
                                );

    // assign this colormap to global colormap
    newWindow->colormap = windowAtrributes.colormap;

    // set the style of the window CW = create window
    styleMask = CWBorderPixel | CWBackPixel | CWColormap | CWEventMask;

    // now finally create a window
    newWindow->window = XCreateWindow(newWindow->display,
                            XRootWindow(newWindow->display, newWindow->visualInfo->screen),
                            0,
                            0,
                            newWindow->_gWidth,
                            newWindow->_gHeight,
                            0,
                            newWindow->visualInfo->depth,
                            InputOutput,
                            newWindow->visualInfo->visual,
                            styleMask,
                            &windowAtrributes
                        );

    if (!newWindow->window)
    {
        exit(1);        
    }

    // specify to which events this window should respond
    XSelectInput(newWindow->display, newWindow->window,
                ExposureMask | VisibilityChangeMask | StructureNotifyMask | KeyPressMask | ButtonPressMask | PointerMotionMask | FocusChangeMask
            );

    // specify window manager delete atom
    windowMangerDelete = XInternAtom(newWindow->display, "WM_DELETE_WINDOW", True);

    // add/set above atom as a protocol for window manager
    XSetWMProtocols(newWindow->display, newWindow->window, &windowMangerDelete, 1);

    // give caption to window
    XStoreName(newWindow->display, newWindow->window, title);

    // show / map the window
    XMapWindow(newWindow->display, newWindow->window);

    // center the window
    if (x == -1 && y == -1) {
        screenWidth = XWidthOfScreen(XScreenOfDisplay(newWindow->display, newWindow->visualInfo->screen));
        screenHeight = XHeightOfScreen(XScreenOfDisplay(newWindow->display, newWindow->visualInfo->screen));

        XMoveWindow(newWindow->display, newWindow->window, (screenWidth - width)/2 , (screenHeight - height)/2);
    }

    // OpenGL initialization
	result = newWindow->initialize();

	if (result != 0) {
        exit(1);
	}
    
    return newWindow;
}

GLOWwindow* glowCreateWindow(const char *title, int width, int height) {
    return glowCreateWindow(title, -1, -1, width, height);
}

int GLOWwindow::initialize() {
	// local variable declarations
    int attribs_new[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
        GLX_CONTEXT_MINOR_VERSION_ARB, 6,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None
    };

    int attribs_old[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
        GLX_CONTEXT_MINOR_VERSION_ARB, 0,
        None
    };

    // code
    // get the address of function in function pointer
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBproc)glXGetProcAddress((GLubyte*)"glXCreateContextAttribsARB");

    if (glXCreateContextAttribsARB == NULL) {
        return -3;
    }

    // create OpenGL context programmable pipeline compatible
    glxContext = glXCreateContextAttribsARB(display, glxFBConfig, 0, True, attribs_new);

    if (!glxContext) {
        glxContext = glXCreateContextAttribsARB(display, glxFBConfig, 0, True, attribs_old);

        if (!glxContext)
        {
            uninitialize();
        }

        if (!glXIsDirect(display, glxContext)) {

        }
        else {

        }
    }
    else {

    }

    // make this context as current context
    if (glXMakeCurrent(display, window, glxContext) == False) {
        return -2;
    }

	return 0;     
}

void GLOWwindow::uninitialize() {
	// local variable declarations
    GLXContext currentGLXContext = NULL;

    // uncurrent the context
    currentGLXContext = glXGetCurrentContext();

    if (currentGLXContext != NULL && currentGLXContext == glxContext)
    {
        glXMakeCurrent(display, 0, 0);
    }

    if (glxContext != NULL)
    {
        glXDestroyContext(display, glxContext);
        glxContext = NULL;
    }

    if (window)
    {
        XDestroyWindow(display, window);
    }

    if (colormap)
    {
        XFreeColormap(display, colormap);
    }

    if (display)
    {
        XCloseDisplay(display);
        display = NULL;
    }
}

void GLOWwindow::ToggleFullscreen(void) {
    Atom windowManagerStateNormal;
    Atom windowManagerStateFullscreen;
    XEvent event;

    // code
    windowManagerStateNormal = XInternAtom(display, "_NET_WM_STATE", False);
    windowManagerStateFullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);

    // memset the event structure and fill it with above two atoms 
    memset((void*)&event, 0, sizeof(XEvent));

    event.type = ClientMessage;
    event.xclient.window = window;
    event.xclient.message_type = windowManagerStateNormal;
    event.xclient.format = 32;
    event.xclient.data.l[0] = _gbFullscreen ? 0 : 1;
    event.xclient.data.l[1] = windowManagerStateFullscreen;

    // send the event
    XSendEvent(display, XRootWindow(display, visualInfo->screen), False, SubstructureNotifyMask, &event);
}

bool GLOWwindow::glowWindowShouldClose() {

    // game loop
    while (XPending(this->display)) {
        memset((void*)&event, 0, sizeof(XEvent));
        XNextEvent(this->display, &event);

        switch (event.type) {
        case MapNotify:
            break;

        case FocusIn:
            _gbActive = True;
            break;

        case FocusOut:
            _gbActive = False;
            break;

        case ConfigureNotify:
            _gWidth = event.xconfigure.width;
            _gHeight = event.xconfigure.height;
        
            if(_reshapeCallback) {
                _reshapeCallback(_gWidth, _gHeight);
            }
            break;

        case KeyPress:
            keySym = XkbKeycodeToKeysym(display, event.xkey.keycode, 0, 0);

            switch (keySym) {
                case XK_Escape:
                    bDone = True;
                    break;
                
                default:
                    break;
            }

            XLookupString(&event.xkey, keys, sizeof(keys), NULL, NULL);

            switch (keys[0])
            {
            case 'F':
            case 'f':
                if (_gbFullscreen == False)
                {
                    ToggleFullscreen();
                    _gbFullscreen = True;
                }
                else
                {
                    ToggleFullscreen();
                    _gbFullscreen = False;
                }
                break;
            
            default:
                break;
            }

            break; // keypress
        
        case ButtonPress:
            switch (event.xbutton.button)
            {
            case 1: // LEFT MOUSE BUTTON
                break;
            case 2: // MIDDLE MOUSE BUTTON
                break;
            case 3: // RIGHT MOUSE BUTTON
                break;                            
            default:
                break;
            }
            break;
        case 33:
            bDone = True;
            break;

        default:
            break;
        }
    }

    return bDone;
}
#endif

// input callbacks
void GLOWwindow::glowKeyboardCallback(KeyboardCallback callback)
{
    _keyboardCallback = callback;
}

void GLOWwindow::glowMouseMoveCallback(MouseMoveCallback callback)
{
    _mouseMoveCallback = callback;
}

void GLOWwindow::glowMouseClickCallback(MouseClickCallback callback)
{
    _mouseClickCallback = callback;
}

// reshape and swap buffers
void GLOWwindow::glowReshapeCallback(ReshapeCallback callback)
{
    _reshapeCallback = callback;
}

void GLOWwindow::glowFullScreen()
{
    if (!_gbFullscreen)
        ToggleFullscreen();
}

void GLOWwindow::glowExitFullScreen()
{
    if (_gbFullscreen)
        ToggleFullscreen();
}

void GLOWwindow::glowSwapBuffers(void) {
	glXSwapBuffers(this->display, this->window);
}

void GLOWwindow::glowDestroyWindow(void) {
    this->uninitialize();
}
