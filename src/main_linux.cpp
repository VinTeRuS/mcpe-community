#ifndef MAIN_LINUX_H__
#define MAIN_LINUX_H__

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#include <SDL2/SDL.h>

#ifdef OPENGL_ES
#include <EGL/egl.h>
#include <GLES/gl.h>
#else
#include <GL/glew.h>
#include <GL/gl.h>
#endif

#include <fstream>
#include <png.h>

#include "NinecraftApp.h"
#include "platform/input/Multitouch.h"
#include "platform/log.h"

FILE* g_logFile = nullptr;

int width = 854;
int height = 480;
bool mouseCaptured = false;
bool wasInGame = false;

static void png_funcReadFile(png_structp pngPtr, png_bytep data, png_size_t length) {
    ((std::istream*)png_get_io_ptr(pngPtr))->read((char*)data, length);
}

class AppPlatform_linux: public AppPlatform
{
public:
    bool isTouchscreen() { return false; }
    bool supportsTouchscreen() { return false; }

    void createUserInput() {
        _userInputStatus = 1;
        _userInput.clear();
        _userInput.push_back("New World");
        char seedStr[16];
        srand((unsigned int)time(nullptr));
        snprintf(seedStr, sizeof(seedStr), "%d", rand());
        _userInput.push_back(seedStr);
        _userInput.push_back("survival");
        LOGVV("createUserInput called - using default values");
    }

    int getUserInputStatus() {
        int status = _userInputStatus;
        _userInputStatus = -1;
        return status;
    }

    StringVector getUserInput() {
        return _userInput;
    }

    TextureData loadTexture(const std::string& filename_, bool textureFolder)
    {
        TextureData out;

        std::string filename;
        if (textureFolder) {
            filename = "data/images/" + filename_;
        } else {
            filename = filename_;
        }

        LOGVV("Loading texture: %s", filename.c_str());

        std::ifstream source(filename.c_str(), std::ios::binary);

        if (!source && textureFolder) {
            LOGW("Trying handheld/data path for: %s", filename.c_str());
            filename = "handheld/data/images/" + filename_;
            source.open(filename.c_str(), std::ios::binary);
        }

        if (!source && textureFolder) {
            LOGW("Trying alternate path for: %s", filename.c_str());
            filename = "../data/images/" + filename_;
            source.open(filename.c_str(), std::ios::binary);
        }

        if (source) {
            LOGVV("Found texture file: %s", filename.c_str());
            png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

            if (!pngPtr) {
                LOGE("Failed to create PNG read struct for: %s", filename.c_str());
                return out;
            }

            png_infop infoPtr = png_create_info_struct(pngPtr);

            if (!infoPtr) {
                png_destroy_read_struct(&pngPtr, NULL, NULL);
                LOGE("Failed to create PNG info struct for: %s", filename.c_str());
                return out;
            }

            png_set_read_fn(pngPtr, (void*)&source, png_funcReadFile);

            png_read_info(pngPtr, infoPtr);

            out.w = png_get_image_width(pngPtr, infoPtr);
            out.h = png_get_image_height(pngPtr, infoPtr);

            LOGVV("Texture dimensions: %dx%d", out.w, out.h);

            png_bytep* rowPtrs = new png_bytep[out.h];
            out.data = new unsigned char[4 * out.w * out.h];
            out.memoryHandledExternally = false;

            int rowStrideBytes = 4 * out.w;
            for (int i = 0; i < out.h; i++) {
                rowPtrs[i] = (png_bytep)&out.data[i*rowStrideBytes];
            }
            png_read_image(pngPtr, rowPtrs);

            png_destroy_read_struct(&pngPtr, &infoPtr, (png_infopp)0);
            delete[] (png_bytep)rowPtrs;
            source.close();

            LOGVV("Successfully loaded texture: %s (%dx%d)", filename_.c_str(), out.w, out.h);
            return out;
        }
        else
        {
            LOGE("Couldn't find texture file: %s", filename.c_str());
            return out;
        }
    }

    BinaryBlob readAssetFile(const std::string& filename) {
        std::string path = "data/" + filename;
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        
        if (!file) {
            LOGW("Couldn't find asset file: %s", path.c_str());
            return BinaryBlob();
        }
        
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        
        unsigned char* buffer = new unsigned char[size];
        if (!file.read((char*)buffer, size)) {
            delete[] buffer;
            LOGW("Failed to read asset file: %s", path.c_str());
            return BinaryBlob();
        }
        
        LOGI("Loaded asset file: %s (%d bytes)", path.c_str(), (int)size);
        return BinaryBlob(buffer, (unsigned int)size);
    }

private:
    int _userInputStatus = -1;
    StringVector _userInput;
};

static bool _inited_gl = false;
static bool _app_inited = false;

bool AppPlatform::glInitialized = false;
bool AppPlatform::captureMouse = false;

static SDL_Window* _window = NULL;
static SDL_GLContext _glContext = NULL;
static App* _app = NULL;
static AppContext* _state = NULL;

static void initGL(App* app, AppContext* state);
static void deinitGL();

static unsigned char transformKey(int key) {
    if (key == SDLK_LSHIFT) return Keyboard::KEY_LSHIFT;
    if (key == SDLK_DOWN) return 40;
    if (key == SDLK_UP)   return 38;
    if (key == SDLK_SPACE) return Keyboard::KEY_SPACE;
    if (key == SDLK_RETURN) return 13;
    if (key == SDLK_ESCAPE) return Keyboard::KEY_ESCAPE;
    if (key == SDLK_TAB) return 250;
    if (key == SDLK_F3) return Keyboard::KEY_F3;
    if (key >= 'a' && key <= 'z') return key - 32;
    if (key >= SDLK_0 && key <= SDLK_9) return '0' + (key - SDLK_0);
    if (key >= SDLK_F1 && key <= SDLK_F12) return Keyboard::KEY_F1 + (key - SDLK_F1);
    return 0;
}

int handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (SDL_QUIT == event.type) {
            return -1;
        }
        if (SDL_TEXTINPUT == event.type) {
            const char* text = event.text.text;
            if (text && text[0]) {
                for (int i = 0; text[i]; i++) {
                    Keyboard::feedText(text[i]);
                }
            }
        }
        if (SDL_KEYDOWN == event.type) {
            int key = event.key.keysym.sym;
            unsigned char transformed = transformKey(key);
            if (transformed) Keyboard::feed(transformed, 1);
            
            if (!mouseCaptured) {
                if (key == SDLK_BACKSPACE) {
                    Keyboard::feed(8, 1); // Backspace key code
                } else if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
                    Keyboard::feed(13, 1); // Enter key code
                }
            }
        }
        if (SDL_KEYUP == event.type) {
            int key = event.key.keysym.sym;
            unsigned char transformed = transformKey(key);
            if (transformed) Keyboard::feed(transformed, 0);
            
            if (key == SDLK_ESCAPE && mouseCaptured) {
                SDL_CaptureMouse(SDL_FALSE);
                SDL_SetWindowGrab(_window, SDL_FALSE);
                mouseCaptured = false;
                SDL_ShowCursor(SDL_ENABLE);
            }
        }
        if (SDL_MOUSEWHEEL == event.type) {
            int x, y;
            SDL_GetGlobalMouseState(&x, &y);
            int wx, wy;
            SDL_GetWindowPosition(_window, &wx, &wy);
            int localX = x - wx;
            int localY = y - wy;
            if (event.wheel.y != 0) {
                Mouse::feed(MouseAction::ACTION_WHEEL, 0, localX, localY, 0, event.wheel.y);
            }
        }
        if (SDL_MOUSEBUTTONDOWN == event.type) {
            char button;
            if (SDL_BUTTON_LEFT == event.button.button) {
                button = MouseAction::ACTION_LEFT;
            } else if (SDL_BUTTON_RIGHT == event.button.button) {
                button = MouseAction::ACTION_RIGHT;
            } else if (SDL_BUTTON_MIDDLE == event.button.button) {
                button = MouseAction::ACTION_MIDDLE;
            } else {
                continue;
            }
            Mouse::feed(button, 1, event.button.x, event.button.y);
            Multitouch::feed(button, 1, event.button.x, event.button.y, 0);
            
            if (SDL_BUTTON_LEFT == event.button.button && !mouseCaptured && AppPlatform::captureMouse) {
                SDL_CaptureMouse(SDL_TRUE);
                SDL_SetWindowGrab(_window, SDL_TRUE);
                mouseCaptured = true;
                SDL_ShowCursor(SDL_DISABLE);
            }
        }
        if (SDL_MOUSEBUTTONUP == event.type) {
            char button;
            if (SDL_BUTTON_LEFT == event.button.button) {
                button = MouseAction::ACTION_LEFT;
            } else if (SDL_BUTTON_RIGHT == event.button.button) {
                button = MouseAction::ACTION_RIGHT;
            } else if (SDL_BUTTON_MIDDLE == event.button.button) {
                button = MouseAction::ACTION_MIDDLE;
            } else {
                continue;
            }
            Mouse::feed(button, 0, event.button.x, event.button.y);
            Multitouch::feed(button, 0, event.button.x, event.button.y, 0);
        }
        if (SDL_MOUSEMOTION == event.type) {
            int x, y;
            SDL_GetGlobalMouseState(&x, &y);
            int wx, wy;
            SDL_GetWindowPosition(_window, &wx, &wy);
            int localX = x - wx;
            int localY = y - wy;
            Mouse::feed(0, 0, localX, localY, event.motion.xrel, event.motion.yrel);
            Multitouch::feed(0, 0, localX, localY, 0);
            
            if (mouseCaptured && AppPlatform::captureMouse) {
                SDL_WarpMouseInWindow(_window, width / 2, height / 2);
            }
        }
        if (SDL_WINDOWEVENT == event.type) {
            if (event.window.event == SDL_WINDOWEVENT_RESIZED || 
                event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                width = event.window.data1;
                height = event.window.data2;
                if (_app) {
                    _app->setSize(width, height);
                }
                glViewport(0, 0, width, height);
            }
            if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
                SDL_StartTextInput();
            }
            if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                SDL_StopTextInput();
            }
        }
    }
    return 0;
}

void initGL(App* app, AppContext* state) {
    if (_inited_gl) return;

#ifdef OPENGL_ES
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOGI("Error: eglGetDisplay failed\n");
        return;
    }

    if (!eglInitialize(display, NULL, NULL)) {
        LOGI("Error: eglInitialize failed\n");
        return;
    }

    EGLint numConfigs;
    EGLConfig config;
    static const EGLint attributeList[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_NONE
    };

    if (!eglChooseConfig(display, attributeList, &config, 1, &numConfigs)) {
        LOGI("Error: eglChooseConfig failed\n");
        return;
    }

    if (!eglBindAPI(EGL_OPENGL_ES_API)) {
        LOGI("Error: eglBindAPI failed\n");
        return;
    }

    static const EGLint contextAttributes[] = {
        EGL_CONTEXT_CLIENT_VERSION, 1,
        EGL_NONE
    };

    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttributes);
    if (context == EGL_NO_CONTEXT) {
        LOGI("Error: eglCreateContext failed\n");
        return;
    }

    EGLSurface surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)_window, NULL);
    if (surface == EGL_NO_SURFACE) {
        LOGI("Error: eglCreateWindowSurface failed\n");
        return;
    }

    if (!eglMakeCurrent(display, surface, surface, context)) {
        LOGI("Error: eglMakeCurrent failed\n");
        return;
    }

    state->display = display;
    state->context = context;
    state->surface = surface;
#else
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        LOGI("Error: glewInit failed: %s\n", glewGetErrorString(err));
        return;
    }
#endif

    _inited_gl = true;
    AppPlatform::glInitialized = true;

    if (!_app_inited) {
        _app_inited = true;
        LOGI("Calling app->init()...");
        app->init(*state);
        LOGI("app->init() returned");
    } else {
        app->onGraphicsReset(*state);
    }
    app->setSize(width, height);
}

void deinitGL() {
    if (!_inited_gl) return;

#ifdef OPENGL_ES
    if (_state) {
        eglSwapBuffers(_state->display, _state->surface);
        eglMakeCurrent(_state->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(_state->display, _state->surface);
        eglDestroyContext(_state->display, _state->context);
        eglTerminate(_state->display);
    }
#else
    if (_glContext) {
        SDL_GL_DeleteContext(_glContext);
        _glContext = NULL;
    }
#endif

    _inited_gl = false;
}

void teardown() {
    deinitGL();
    if (_window) {
        SDL_DestroyWindow(_window);
        _window = NULL;
    }
    if (g_logFile) {
        fclose(g_logFile);
        g_logFile = nullptr;
    }
    SDL_Quit();
}

int main(int argc, char** argv) {
    std::string logDir = getenv("HOME");
    logDir += "/.minecraft/mcpe/";
    
    mkdir(logDir.c_str(), 0755);
    
    std::string logPath = logDir + "game.log";
    g_logFile = fopen(logPath.c_str(), "w");
    
    time_t now = time(nullptr);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    if (g_logFile) {
        fprintf(g_logFile, "[%s] Opening game.log\n", timestamp);
        fflush(g_logFile);
    }
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        return -1;
    }

#ifdef OPENGL_ES
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
#endif

    _window = SDL_CreateWindow(
        "Minecraft PE",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
    );

    if (!_window) {
        printf("Couldn't create SDL window: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_GetWindowSize(_window, &width, &height);
    LOGI("Window created with size: %dx%d", width, height);

    SDL_StartTextInput();

#ifdef OPENGL_ES
    _glContext = NULL;
#else
    _glContext = SDL_GL_CreateContext(_window);
    if (!_glContext) {
        printf("Couldn't create GL context: %s\n", SDL_GetError());
        SDL_DestroyWindow(_window);
        SDL_Quit();
        return -1;
    }
    SDL_GL_SetSwapInterval(0);
#endif

    std::string path = argv[0];
    int e = path.rfind('/');
    if (e != std::string::npos) {
        path = path.substr(0, e);
        LOGI("Changing directory to executable location: %s", path.c_str());
        chdir(path.c_str());
    }

    char cwd[256];
    if (getcwd(cwd, sizeof(cwd))) {
        LOGI("Current working directory: %s", cwd);
    }

    std::string dataPath;
    if (opendir("data")) {
        dataPath = "data";
        LOGI("'data' directory found in current directory!");
    } else if (opendir("handheld/data")) {
        dataPath = "handheld/data";
        LOGI("'data' directory found in handheld/data!");
        chdir("handheld");
    } else {
        LOGE("'data' directory NOT found!");
    }

    atexit(teardown);
    SDL_SetWindowGrab(_window, SDL_FALSE);
    SDL_ShowCursor(1);

    NinecraftApp* app = new NinecraftApp();
    _app = app;
    std::string storagePath = getenv("HOME");
    storagePath += "/.minecraft/mcpe/";
    app->externalStoragePath = storagePath;
    app->externalCacheStoragePath = storagePath;
    LOGI("Storage path set to: %s", storagePath.c_str());

    AppContext context;
    AppPlatform_linux platform;
    context.doRender = true;
    context.platform = &platform;
    _state = &context;

    initGL(app, &context);

    bool running = true;
    Uint32 lastFrameTime = SDL_GetTicks();
    int targetFrameTime = 16;

    while (running) {
        running = (handleEvents() == 0);
        if (!running) break;

        int maxFps = app->options.maxFps;
        if (maxFps > 0) {
            targetFrameTime = 1000 / maxFps;
        } else {
            targetFrameTime = 1;
        }

        app->update();

        if (AppPlatform::captureMouse && !mouseCaptured) {
            SDL_CaptureMouse(SDL_TRUE);
            SDL_SetWindowGrab(_window, SDL_TRUE);
            mouseCaptured = true;
            SDL_ShowCursor(SDL_DISABLE);
        } else if (!AppPlatform::captureMouse && mouseCaptured) {
            SDL_CaptureMouse(SDL_FALSE);
            SDL_SetWindowGrab(_window, SDL_FALSE);
            mouseCaptured = false;
            SDL_ShowCursor(SDL_ENABLE);
        }

#ifdef OPENGL_ES
        if (_state) {
            eglSwapBuffers(_state->display, _state->surface);
        }
#else
        SDL_GL_SwapWindow(_window);
#endif

        if (maxFps > 0) {
            Uint32 currentTime = SDL_GetTicks();
            Uint32 frameTime = currentTime - lastFrameTime;
            if (frameTime < (Uint32)targetFrameTime) {
                SDL_Delay(targetFrameTime - frameTime);
            }
            lastFrameTime = SDL_GetTicks();
        }
    }

    delete app;
    return 0;
}

#endif
