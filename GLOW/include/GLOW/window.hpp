#ifndef __window_manager_h__
#define __window_manager_h__

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
#endif // WIN32

#include <stdio.h>  // for printf()
#include <stdlib.h> // for exit()

#define UNICODE
#define MYICON 101

typedef void (*InitializeCallback)      (void);
typedef void (*UninitializeCallback)    (void);

typedef void (*KeyboardCallback)        (unsigned int key);
typedef void (*MouseMoveCallback)       (int x, int y);
typedef void (*MouseClickCallback)      (unsigned int key, int x, int y);

typedef void (*DisplayCallback)         (void);
typedef void (*UpdateCallback)          (void);

typedef void (*ReshapeCallback)         (int width, int height);

// function declarations
#ifdef _WIN32
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
#else
typedef GLXContext (*glXCreateContextAttribsARBproc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
#endif // WIN32

void ToggleFullscreen(void);
void uninitialize(void);

void acewmCreateWindow(const char *title, int x, int y, int width, int height);
void acewmDestroyWindow(void);

void acewmInitializeCallback(InitializeCallback callback);
void acewmUninitiiizeCallback(UninitializeCallback callback); 

void acewmFullScreen();
void acewmExitFullScreen();

void acewmEventLoop();
void acewmExitEventLoop();

void acewmKeyboardCallback(KeyboardCallback callback);
void acewmMouseMoveCallback(MouseMoveCallback callback);
void acewmMouseClickCallback(MouseClickCallback callback);

void acewmDisplayCallback(DisplayCallback callback);
void acewmUpdateCallback(UpdateCallback callback);

void acewmReshapeCallback(ReshapeCallback callback);

void acewmSwapBuffers(void);

void printGLinfo();

#endif
