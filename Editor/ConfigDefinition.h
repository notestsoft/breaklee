#ifndef CONFIG_BEGIN
#define CONFIG_BEGIN(__NAMESPACE__)
#endif

#ifndef CONFIG_PARAMETER
#define CONFIG_PARAMETER(__TYPE__, __NAME__, __PATH__, __DEFAULT__)
#endif

#ifndef CONFIG_PARAMETER_ARRAY
#define CONFIG_PARAMETER_ARRAY(__TYPE__, __LENGTH__, __NAME__, __PATH__, __DEFAULT__)
#endif

#ifndef CONFIG_END
#define CONFIG_END(__NAMESPACE__)
#endif

CONFIG_BEGIN(Editor)
CONFIG_PARAMETER_ARRAY(Char, PLATFORM_PATH_MAX, ServerDataPath, "Editor.ServerDataPath", ServerData)
CONFIG_PARAMETER_ARRAY(Char, PLATFORM_PATH_MAX, ClientDataPath, "Editor.ClientDataPath", ClientData)
CONFIG_PARAMETER_ARRAY(Char, PLATFORM_PATH_MAX, ShaderDataPath, "Editor.ShaderDataPath", Shaders)
CONFIG_PARAMETER_ARRAY(Char, PLATFORM_PATH_MAX, ClientLanguage, "Editor.ClientLanguage", Korean)
CONFIG_END(Editor)

CONFIG_BEGIN(Screen)
CONFIG_PARAMETER(Int32, Width, "Screen.Width", 1280)
CONFIG_PARAMETER(Int32, Height, "Screen.Height", 720)
CONFIG_PARAMETER(Int32, TargetFPS, "Screen.TargetFPS", 60)
CONFIG_END(Screen)

CONFIG_BEGIN(Diagnostic)
CONFIG_PARAMETER(Int32, LogLevel, "Diagnostic.LogLevel", 5)
CONFIG_END(Diagnostic)

#undef CONFIG_BEGIN
#undef CONFIG_PARAMETER
#undef CONFIG_PARAMETER_ARRAY
#undef CONFIG_END
