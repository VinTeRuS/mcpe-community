#ifndef NET_MINECRAFT_CLIENT_PLATFORM_H
#define NET_MINECRAFT_CLIENT_PLATFORM_H

#include <cstddef>

enum class PlatformOS {
    Unknown,
    Linux,
    Windows,
    macOS,
    Android,
    RPI
};

enum class PlatformGL {
    DesktopGL,
    GLES
};

class Platform {
public:
    static inline Platform& singleton() {
        static Platform instance;
        return instance;
    }

    PlatformOS os() const { return m_os; }
    PlatformGL gl() const { return m_gl; }

    bool isAndroid() const { return m_os == PlatformOS::Android; }
    bool isApple() const { return m_os == PlatformOS::macOS; }
    bool isLinux() const { return m_os == PlatformOS::Linux; }
    bool isWindows() const { return m_os == PlatformOS::Windows; }
    bool isRPI() const { return m_os == PlatformOS::RPI; }

    bool isGLES() const { return m_gl == PlatformGL::GLES; }
    bool useVBO() const { return m_useVBO; }
    bool isServer() const { return m_isServer; }
    bool isDemo() const { return m_isDemo; }
    bool isDebugGL() const { return m_debugGL; }
    bool isPreAndroid23() const { return m_preAndroid23; }
    bool isAppleDemoPromotion() const { return m_appleDemoPromotion; }

    bool supportsTouchscreen() const { return m_touchscreen; }
    bool supportsMouse() const { return m_mouse; }

    void setOS(PlatformOS os) { m_os = os; }
    void setGL(PlatformGL gl) { m_gl = gl; }
    void setUseVBO(bool v) { m_useVBO = v; }
    void setServer(bool s) { m_isServer = s; }
    void setDemo(bool d) { m_isDemo = d; }
    void setDebugGL(bool d) { m_debugGL = d; }
    void setPreAndroid23(bool p) { m_preAndroid23 = p; }
    void setAppleDemoPromotion(bool a) { m_appleDemoPromotion = a; }
    void setTouchscreen(bool t) { m_touchscreen = t; }
    void setMouse(bool m) { m_mouse = m; }

private:
    Platform() = default;

    PlatformOS m_os = PlatformOS::Unknown;
    PlatformGL m_gl = PlatformGL::DesktopGL;
    bool m_useVBO = true;
    bool m_isServer = false;
    bool m_isDemo = false;
    bool m_debugGL = false;
    bool m_preAndroid23 = false;
    bool m_appleDemoPromotion = false;
    bool m_touchscreen = false;
    bool m_mouse = true;
};

#endif
