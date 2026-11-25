#pragma once
#include "Base/Export.hpp"
#include <cstdarg>
// Engine values and opaque resources. No backend headers are needed by consumers.
namespace Sonar {
struct Vector2 {
    float x;                
    float y;                
};
struct Vector3 {
    float x;                
    float y;                
    float z;                
};
struct Vector4 {
    float x;                
    float y;                
    float z;                
    float w;                
};
struct Matrix {
    float m0, m4, m8, m12;  
    float m1, m5, m9, m13;  
    float m2, m6, m10, m14; 
    float m3, m7, m11, m15; 
};
struct Color {
    unsigned char r;        
    unsigned char g;        
    unsigned char b;        
    unsigned char a;        
};
struct Rectangle {
    float x;                
    float y;                
    float width;            
    float height;           
};
struct NPatchInfo {
    Rectangle source;       
    int left;               
    int top;                
    int right;              
    int bottom;             
    int layout;             
};
enum KeyboardKey {
    KEY_NULL            = 0,        
    
    KEY_APOSTROPHE      = 39,       
    KEY_COMMA           = 44,       
    KEY_MINUS           = 45,       
    KEY_PERIOD          = 46,       
    KEY_SLASH           = 47,       
    KEY_ZERO            = 48,       
    KEY_ONE             = 49,       
    KEY_TWO             = 50,       
    KEY_THREE           = 51,       
    KEY_FOUR            = 52,       
    KEY_FIVE            = 53,       
    KEY_SIX             = 54,       
    KEY_SEVEN           = 55,       
    KEY_EIGHT           = 56,       
    KEY_NINE            = 57,       
    KEY_SEMICOLON       = 59,       
    KEY_EQUAL           = 61,       
    KEY_A               = 65,       
    KEY_B               = 66,       
    KEY_C               = 67,       
    KEY_D               = 68,       
    KEY_E               = 69,       
    KEY_F               = 70,       
    KEY_G               = 71,       
    KEY_H               = 72,       
    KEY_I               = 73,       
    KEY_J               = 74,       
    KEY_K               = 75,       
    KEY_L               = 76,       
    KEY_M               = 77,       
    KEY_N               = 78,       
    KEY_O               = 79,       
    KEY_P               = 80,       
    KEY_Q               = 81,       
    KEY_R               = 82,       
    KEY_S               = 83,       
    KEY_T               = 84,       
    KEY_U               = 85,       
    KEY_V               = 86,       
    KEY_W               = 87,       
    KEY_X               = 88,       
    KEY_Y               = 89,       
    KEY_Z               = 90,       
    KEY_LEFT_BRACKET    = 91,       
    KEY_BACKSLASH       = 92,       
    KEY_RIGHT_BRACKET   = 93,       
    KEY_GRAVE           = 96,       
    
    KEY_SPACE           = 32,       
    KEY_ESCAPE          = 256,      
    KEY_ENTER           = 257,      
    KEY_TAB             = 258,      
    KEY_BACKSPACE       = 259,      
    KEY_INSERT          = 260,      
    KEY_DELETE          = 261,      
    KEY_RIGHT           = 262,      
    KEY_LEFT            = 263,      
    KEY_DOWN            = 264,      
    KEY_UP              = 265,      
    KEY_PAGE_UP         = 266,      
    KEY_PAGE_DOWN       = 267,      
    KEY_HOME            = 268,      
    KEY_END             = 269,      
    KEY_CAPS_LOCK       = 280,      
    KEY_SCROLL_LOCK     = 281,      
    KEY_NUM_LOCK        = 282,      
    KEY_PRINT_SCREEN    = 283,      
    KEY_PAUSE           = 284,      
    KEY_F1              = 290,      
    KEY_F2              = 291,      
    KEY_F3              = 292,      
    KEY_F4              = 293,      
    KEY_F5              = 294,      
    KEY_F6              = 295,      
    KEY_F7              = 296,      
    KEY_F8              = 297,      
    KEY_F9              = 298,      
    KEY_F10             = 299,      
    KEY_F11             = 300,      
    KEY_F12             = 301,      
    KEY_LEFT_SHIFT      = 340,      
    KEY_LEFT_CONTROL    = 341,      
    KEY_LEFT_ALT        = 342,      
    KEY_LEFT_SUPER      = 343,      
    KEY_RIGHT_SHIFT     = 344,      
    KEY_RIGHT_CONTROL   = 345,      
    KEY_RIGHT_ALT       = 346,      
    KEY_RIGHT_SUPER     = 347,      
    KEY_KB_MENU         = 348,      
    
    KEY_KP_0            = 320,      
    KEY_KP_1            = 321,      
    KEY_KP_2            = 322,      
    KEY_KP_3            = 323,      
    KEY_KP_4            = 324,      
    KEY_KP_5            = 325,      
    KEY_KP_6            = 326,      
    KEY_KP_7            = 327,      
    KEY_KP_8            = 328,      
    KEY_KP_9            = 329,      
    KEY_KP_DECIMAL      = 330,      
    KEY_KP_DIVIDE       = 331,      
    KEY_KP_MULTIPLY     = 332,      
    KEY_KP_SUBTRACT     = 333,      
    KEY_KP_ADD          = 334,      
    KEY_KP_ENTER        = 335,      
    KEY_KP_EQUAL        = 336,      
    
    KEY_BACK            = 4,        
    KEY_MENU            = 5,        
    KEY_VOLUME_UP       = 24,       
    KEY_VOLUME_DOWN     = 25        
};
enum MouseButton {
    MOUSE_BUTTON_LEFT    = 0,       
    MOUSE_BUTTON_RIGHT   = 1,       
    MOUSE_BUTTON_MIDDLE  = 2,       
    MOUSE_BUTTON_SIDE    = 3,       
    MOUSE_BUTTON_EXTRA   = 4,       
    MOUSE_BUTTON_FORWARD = 5,       
    MOUSE_BUTTON_BACK    = 6,       
};
enum MouseCursor {
    MOUSE_CURSOR_DEFAULT       = 0,     
    MOUSE_CURSOR_ARROW         = 1,     
    MOUSE_CURSOR_IBEAM         = 2,     
    MOUSE_CURSOR_CROSSHAIR     = 3,     
    MOUSE_CURSOR_POINTING_HAND = 4,     
    MOUSE_CURSOR_RESIZE_EW     = 5,     
    MOUSE_CURSOR_RESIZE_NS     = 6,     
    MOUSE_CURSOR_RESIZE_NWSE   = 7,     
    MOUSE_CURSOR_RESIZE_NESW   = 8,     
    MOUSE_CURSOR_RESIZE_ALL    = 9,     
    MOUSE_CURSOR_NOT_ALLOWED   = 10     
};
enum TextureFilter {
    TEXTURE_FILTER_POINT = 0,               
    TEXTURE_FILTER_BILINEAR,                
    TEXTURE_FILTER_TRILINEAR,               
    TEXTURE_FILTER_ANISOTROPIC_4X,          
    TEXTURE_FILTER_ANISOTROPIC_8X,          
    TEXTURE_FILTER_ANISOTROPIC_16X,         
};
enum TextureWrap {
    TEXTURE_WRAP_REPEAT = 0,                
    TEXTURE_WRAP_CLAMP,                     
    TEXTURE_WRAP_MIRROR_REPEAT,             
    TEXTURE_WRAP_MIRROR_CLAMP               
};
enum ConfigFlags {
    FLAG_VSYNC_HINT         = 0x00000040,   
    FLAG_FULLSCREEN_MODE    = 0x00000002,   
    FLAG_WINDOW_RESIZABLE   = 0x00000004,   
    FLAG_WINDOW_UNDECORATED = 0x00000008,   
    FLAG_WINDOW_HIDDEN      = 0x00000080,   
    FLAG_WINDOW_MINIMIZED   = 0x00000200,   
    FLAG_WINDOW_MAXIMIZED   = 0x00000400,   
    FLAG_WINDOW_UNFOCUSED   = 0x00000800,   
    FLAG_WINDOW_TOPMOST     = 0x00001000,   
    FLAG_WINDOW_ALWAYS_RUN  = 0x00000100,   
    FLAG_WINDOW_TRANSPARENT = 0x00000010,   
    FLAG_WINDOW_HIGHDPI     = 0x00002000,   
    FLAG_WINDOW_MOUSE_PASSTHROUGH = 0x00004000, 
    FLAG_BORDERLESS_WINDOWED_MODE = 0x00008000, 
    FLAG_MSAA_4X_HINT       = 0x00000020,   
    FLAG_INTERLACED_HINT    = 0x00010000    
};
enum TraceLogLevel {
    LOG_ALL = 0,        
    LOG_TRACE,          
    LOG_DEBUG,          
    LOG_INFO,           
    LOG_WARNING,        
    LOG_ERROR,          
    LOG_FATAL,          
    LOG_NONE            
};
enum PixelFormat {
    PIXELFORMAT_UNCOMPRESSED_GRAYSCALE = 1, 
    PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA,    
    PIXELFORMAT_UNCOMPRESSED_R5G6B5,        
    PIXELFORMAT_UNCOMPRESSED_R8G8B8,        
    PIXELFORMAT_UNCOMPRESSED_R5G5B5A1,      
    PIXELFORMAT_UNCOMPRESSED_R4G4B4A4,      
    PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,      
    PIXELFORMAT_UNCOMPRESSED_R32,           
    PIXELFORMAT_UNCOMPRESSED_R32G32B32,     
    PIXELFORMAT_UNCOMPRESSED_R32G32B32A32,  
    PIXELFORMAT_UNCOMPRESSED_R16,           
    PIXELFORMAT_UNCOMPRESSED_R16G16B16,     
    PIXELFORMAT_UNCOMPRESSED_R16G16B16A16,  
    PIXELFORMAT_COMPRESSED_DXT1_RGB,        
    PIXELFORMAT_COMPRESSED_DXT1_RGBA,       
    PIXELFORMAT_COMPRESSED_DXT3_RGBA,       
    PIXELFORMAT_COMPRESSED_DXT5_RGBA,       
    PIXELFORMAT_COMPRESSED_ETC1_RGB,        
    PIXELFORMAT_COMPRESSED_ETC2_RGB,        
    PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA,   
    PIXELFORMAT_COMPRESSED_PVRT_RGB,        
    PIXELFORMAT_COMPRESSED_PVRT_RGBA,       
    PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA,   
    PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA    
};
enum NPatchLayout {
    NPATCH_NINE_PATCH = 0,          
    NPATCH_THREE_PATCH_VERTICAL,    
    NPATCH_THREE_PATCH_HORIZONTAL   
};
inline constexpr Color LIGHTGRAY{200,200,200,255};
inline constexpr Color GRAY{130,130,130,255};
inline constexpr Color DARKGRAY{80,80,80,255};
inline constexpr Color YELLOW{253,249,0,255};
inline constexpr Color GOLD{255,203,0,255};
inline constexpr Color ORANGE{255,161,0,255};
inline constexpr Color PINK{255,109,194,255};
inline constexpr Color RED{230,41,55,255};
inline constexpr Color MAROON{190,33,55,255};
inline constexpr Color GREEN{0,228,48,255};
inline constexpr Color LIME{0,158,47,255};
inline constexpr Color DARKGREEN{0,117,44,255};
inline constexpr Color SKYBLUE{102,191,255,255};
inline constexpr Color BLUE{0,121,241,255};
inline constexpr Color DARKBLUE{0,82,172,255};
inline constexpr Color PURPLE{200,122,255,255};
inline constexpr Color VIOLET{135,60,190,255};
inline constexpr Color DARKPURPLE{112,31,126,255};
inline constexpr Color BEIGE{211,176,131,255};
inline constexpr Color BROWN{127,106,79,255};
inline constexpr Color DARKBROWN{76,63,47,255};
inline constexpr Color WHITE{255,255,255,255};
inline constexpr Color BLACK{0,0,0,255};
inline constexpr Color BLANK{0,0,0,0};
inline constexpr Color MAGENTA{255,0,255,255};
inline constexpr Color RAYWHITE{245,245,245,255};
inline constexpr float PI=3.14159265358979323846f, DEG2RAD=PI/180.0f, RAD2DEG=180.0f/PI;
// Non-owning resource descriptors. Copies refer to the same underlying resource.
// The caller/resource manager must unload each resource once, after its last use.
// IsValid checks descriptor fields; it cannot detect resources freed through another copy.
struct Texture2D {
    unsigned int id = 0;
    int width = 0;
    int height = 0;
    int mipmaps = 0;
    int format = 0;

    bool IsValid() const noexcept { return id != 0; }
};
using Texture = Texture2D;
using TextureCubemap = Texture2D;

struct Shader {
    unsigned int id = 0;
    int* locs = nullptr;

    bool IsValid() const noexcept { return id != 0; }
};

struct AudioStream {
    void* buffer = nullptr;
    void* processor = nullptr;
    unsigned int sampleRate = 0;
    unsigned int sampleSize = 0;
    unsigned int channels = 0;

    bool IsValid() const noexcept { return buffer != nullptr; }
};

struct Music {
    AudioStream stream{};
    unsigned int frameCount = 0;
    bool looping = false;
    int ctxType = 0;
    void* ctxData = nullptr;

    bool IsValid() const noexcept { return stream.IsValid(); }
};

struct RenderTexture2D {
    unsigned int id = 0;
    Texture2D texture{};
    Texture2D depth{};

    bool IsValid() const noexcept { return id != 0; }
};
using RenderTexture = RenderTexture2D;

struct Image {
    void* data = nullptr;
    int width = 0;
    int height = 0;
    int mipmaps = 0;
    int format = 0;

    bool IsValid() const noexcept { return data != nullptr; }
};

using TraceLogCallback = void (*)(int, const char*, va_list);
}
