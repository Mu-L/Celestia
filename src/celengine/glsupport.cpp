#include "glsupport.h"
#include <algorithm>
#include <cstring>

namespace celestia::gl
{

#ifdef GL_ES
CELAPI bool OES_vertex_array_object        = false;
CELAPI bool OES_texture_border_clamp       = false;
CELAPI bool OES_geometry_shader            = false;
CELAPI bool APPLE_framebuffer_multisample  = false;
CELAPI bool EXT_discard_framebuffer        = false;
#else
CELAPI bool ARB_vertex_array_object        = false;
CELAPI bool ARB_framebuffer_object         = false;
CELAPI bool EXT_framebuffer_multisample    = false;
CELAPI bool EXT_framebuffer_blit           = false;
CELAPI bool ARB_invalidate_subdata         = false;
#endif
CELAPI bool ARB_shader_texture_lod         = false;
CELAPI bool EXT_texture_compression_s3tc   = false;
CELAPI bool EXT_texture_filter_anisotropic = false;
CELAPI bool MESA_pack_invert               = false;
CELAPI GLint maxPointSize                  = 0;
CELAPI GLint maxTextureSize                = 0;
CELAPI GLfloat maxLineWidth                = 0.0f;
CELAPI GLint maxTextureAnisotropy          = 0;

namespace
{

bool EnableGeomShaders = true;

inline bool has_extension(const char *name) noexcept
{
    return epoxy_has_gl_extension(name);
}

bool check_extension(util::array_view<std::string> list, const char *name) noexcept
{
    return std::find(list.begin(), list.end(), std::string(name)) == list.end()
           && has_extension(name);
}

void enable_workarounds()
{
    bool isMesa = false;
    bool isAMD = false;
    bool isNavi = false;

    const char* s;
    s = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    // "4.6 (Compatibility Profile) Mesa 22.3.6"
    // "OpenGL ES 3.2 Mesa 22.3.6"
    if (s != nullptr)
        isMesa = std::strstr(s, "Mesa") != nullptr;

    s = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    // "AMD" for radeonsi
    // "Mesa/X.org" for llvmpipe
    // "Collabora Ltd" for zink
    if (s != nullptr)
        isAMD = std::strcmp(s, "AMD") == 0;

    s = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    // "AMD Radeon RX 6600 (navi23, LLVM 15.0.6, DRM 3.52, 6.4.0-0.deb12.2-amd64)"" for radeonsi
    // "llvmpipe (LLVM 15.0.6, 256 bits)""
    // "zink (llvmpipe (LLVM 15.0.6, 256 bits))""
    // "zink (AMD Radeon RX 6600 (RADV NAVI23))""
    if (s != nullptr)
        isNavi = std::strstr(s, "navi") != nullptr;

    // https://gitlab.freedesktop.org/mesa/mesa/-/issues/9971
    if (isMesa && isAMD && isNavi)
        EnableGeomShaders = false;
}

} // namespace

bool init(util::array_view<std::string> ignore) noexcept
{
#ifdef GL_ES
    OES_vertex_array_object        = check_extension(ignore, "GL_OES_vertex_array_object");
    OES_texture_border_clamp       = check_extension(ignore, "GL_OES_texture_border_clamp") || check_extension(ignore, "GL_EXT_texture_border_clamp");
    OES_geometry_shader            = check_extension(ignore, "GL_OES_geometry_shader") || check_extension(ignore, "GL_EXT_geometry_shader");
    APPLE_framebuffer_multisample  = check_extension(ignore, "GL_APPLE_framebuffer_multisample");
    EXT_discard_framebuffer        = check_extension(ignore, "GL_EXT_discard_framebuffer");
#else
    ARB_vertex_array_object        = check_extension(ignore, "GL_ARB_vertex_array_object");
    ARB_framebuffer_object         = check_extension(ignore, "GL_ARB_framebuffer_object") || check_extension(ignore, "GL_EXT_framebuffer_object");
    EXT_framebuffer_multisample    = check_extension(ignore, "GL_EXT_framebuffer_multisample");
    EXT_framebuffer_blit           = check_extension(ignore, "GL_EXT_framebuffer_blit");
    ARB_invalidate_subdata         = check_extension(ignore, "GL_ARB_invalidate_subdata");
#endif
    ARB_shader_texture_lod         = check_extension(ignore, "GL_ARB_shader_texture_lod");
    EXT_texture_compression_s3tc   = check_extension(ignore, "GL_EXT_texture_compression_s3tc");
    EXT_texture_filter_anisotropic = check_extension(ignore, "GL_EXT_texture_filter_anisotropic") || check_extension(ignore, "GL_ARB_texture_filter_anisotropic");
    MESA_pack_invert               = check_extension(ignore, "GL_MESA_pack_invert");

    GLint pointSizeRange[2];
    GLfloat lineWidthRange[2];
#ifdef GL_ES
    glGetIntegerv(GL_ALIASED_POINT_SIZE_RANGE, pointSizeRange);
    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidthRange);
#else
    glGetIntegerv(GL_SMOOTH_POINT_SIZE_RANGE, pointSizeRange);
    glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, lineWidthRange);
#endif
    maxPointSize = pointSizeRange[1];
    maxLineWidth = lineWidthRange[1];

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

    if (gl::EXT_texture_filter_anisotropic)
        glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxTextureAnisotropy);

    enable_workarounds();

    return true;
}

bool checkVersion(int v) noexcept
{
    static int version = 0;
    if (version == 0)
        version = epoxy_gl_version(); // this function always queries GL
    return version >= v;
}

bool hasGeomShader() noexcept
{
#ifdef GL_ES
    return EnableGeomShaders && checkVersion(celestia::gl::GLES_3_2);
#else
    return EnableGeomShaders && checkVersion(celestia::gl::GL_3_2);
#endif
}

void enableGeomShaders() noexcept
{
    EnableGeomShaders = true;
}

void disableGeomShaders() noexcept
{
    EnableGeomShaders = false;
}

} // end namespace celestia::gl
