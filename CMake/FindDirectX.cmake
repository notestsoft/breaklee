find_path(DirectX_INCLUDE_DIRS d3d9.h PATHS
    "$ENV{DXSDK_DIR}/Include"
    "$ENV{PROGRAMFILES}/Microsoft DirectX SDK*/Include"
)

get_filename_component(DirectX_ROOT_DIR "${DirectX_INCLUDE_DIRS}/.." ABSOLUTE)

if (CMAKE_CL_64)
    set(DirectX_LIBRARY_PATHS "${DirectX_ROOT_DIR}/Lib/x64")
else ()
    set(DirectX_LIBRARY_PATHS "${DirectX_ROOT_DIR}/Lib/x86" "${DirectX_ROOT_DIR}/Lib")
endif ()

find_library(DirectX_D3D9_LIBRARY d3d9 ${DirectX_LIBRARY_PATHS} NO_DEFAULT_PATH)
find_library(DirectX_D3DX9_LIBRARY d3dx9 ${DirectX_LIBRARY_PATHS} NO_DEFAULT_PATH)
set(DirectX_LIBRARIES ${DirectX_D3D9_LIBRARY} ${DirectX_D3DX9_LIBRARY})

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DirectX DEFAULT_MSG DirectX_ROOT_DIR DirectX_LIBRARIES DirectX_INCLUDE_DIRS)
mark_as_advanced(DirectX_INCLUDE_DIRS DirectX_D3D9_LIBRARY DirectX_D3DX9_LIBRARY)