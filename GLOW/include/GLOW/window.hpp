#pragma once

#include <stdio.h>  // for printf()
#include <stdlib.h> // for exit()

#ifdef _WIN32
    #include <windows.h>
#else
    // X11 header files
    #include <X11/Xlib.h>   // for all Xlib APIs
    #include <X11/Xutil.h>  // for struct XVisualInfo and related APIs
    #include <X11/XKBlib.h>

    // OpenGL header files
    #include <GL/glx.h>

    #include <memory.h> // for memset()
#endif

#define UNICODE
#define MYICON 101

typedef void (*KeyboardCallback)        (unsigned int key);
typedef void (*MouseMoveCallback)       (int x, int y);
typedef void (*MouseClickCallback)      (unsigned int key, int x, int y);

typedef void (*ReshapeCallback)         (int width, int height);

// function declarations
#ifdef _WIN32
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
#else
typedef GLXContext (*glXCreateContextAttribsARBproc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
#endif // WIN32

#ifdef _WIN32
class GLOWwindow {
	private:
        HWND _ghwnd = NULL;
        DWORD _dwStyle = 0;
        WINDOWPLACEMENT _wpPrev = { sizeof(WINDOWPLACEMENT) };

        BOOL _gbFullscreen = FALSE;
        BOOL _gbActive = FALSE;

        int _gWidth = 800;
        int _gHeight = 600;

        // OpenGL related global variables
        HDC ghdc = NULL;
        HGLRC ghrc = NULL;

        void initialize(void);
        void ToggleFullscreen(void);
        void uninitialize(void);

        // global callback declarations
        InitializeCallback   _initializeCallback  = NULL;
        KeyboardCallback     _keyboardCallback    = NULL;  
        MouseMoveCallback    _mouseMoveCallback   = NULL; 
        MouseClickCallback   _mouseClickCallback  = NULL;
        DisplayCallback      _displayCallback     = NULL;   
        UpdateCallback       _updateCallback      = NULL;    
        ReshapeCallback      _reshapeCallback     = NULL;   
        UninitializeCallback _uninitializeCallback = NULL;

    public:

        // void glowDestroyWindow(void);

        // void glowInitializeCallback(InitializeCallback callback);
        // void glowUninitiiizeCallback(UninitializeCallback callback); 

        void glowFullScreen();
        void glowExitFullScreen();

        void glowEventLoop();
        void glowExitEventLoop();

        void glowKeyboardCallback(KeyboardCallback callback);
        void glowMouseMoveCallback(MouseMoveCallback callback);
        void glowMouseClickCallback(MouseClickCallback callback);

        void glowDisplayCallback(DisplayCallback callback);
        void glowUpdateCallback(UpdateCallback callback);

        void glowReshapeCallback(ReshapeCallback callback);

        void glowSwapBuffers(void);

        friend GLOWwindow* glowCreateWindow(const char *title, int x, int y, int width, int height);   
};

#else

class GLOWwindow {
    private:
        Display *display = NULL;
        Colormap colormap;
        Window window;
        XVisualInfo *visualInfo;

        XEvent event;
        KeySym keySym;
        char keys[26];

        Bool _gbFullscreen = False;
        Bool _gbActive = False;

        Bool bDone = False;

        int _gWidth = 800;
        int _gHeight = 600;

        // OpenGL related global variable
        glXCreateContextAttribsARBproc glXCreateContextAttribsARB = NULL;
        GLXFBConfig glxFBConfig;
        GLXContext glxContext = NULL;

        int initialize(void);
        void ToggleFullscreen(void);
        void uninitialize(void);

        // global callback declarations
        KeyboardCallback     _keyboardCallback    = NULL;  
        MouseMoveCallback    _mouseMoveCallback   = NULL; 
        MouseClickCallback   _mouseClickCallback  = NULL;
        ReshapeCallback      _reshapeCallback     = NULL;   

    public:
        void glowFullScreen();
        void glowExitFullScreen();

        void glowEventLoop();
        void glowExitEventLoop();

        void glowKeyboardCallback(KeyboardCallback callback);
        void glowMouseMoveCallback(MouseMoveCallback callback);
        void glowMouseClickCallback(MouseClickCallback callback);

        void glowReshapeCallback(ReshapeCallback callback);

        void glowSwapBuffers(void);

        friend GLOWwindow* glowCreateWindow(const char *title, int x, int y, int width, int height);
        bool glowWindowShouldClose(void);
        void glowDestroyWindow(void);
};

GLOWwindow* glowCreateWindow(const char *title, int x, int y, int width, int height);
GLOWwindow* glowCreateWindow(const char *title, int width, int height);

#endif
