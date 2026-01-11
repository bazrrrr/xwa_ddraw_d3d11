set(ddraw_sources
    ddraw.cfg
    ddraw.def
    BackbufferSurface.cpp
    config.cpp
    ddraw.cpp
    DepthSurface.cpp
    DeviceResources.cpp
    Direct3D.cpp
    Direct3DDevice.cpp
    Direct3DExecuteBuffer.cpp
    Direct3DMaterial.cpp
    Direct3DTexture.cpp
    Direct3DViewport.cpp
    DirectDraw.cpp
    DirectDraw2.cpp
    DirectDrawPalette.cpp
    dllmain.cpp
    ExecuteBufferDumper.cpp
    FrontbufferSurface.cpp
    joystick.cpp
    logger.cpp
    MipmapSurface.cpp
    OffscreenSurface.cpp
    PrimarySurface.cpp
    TextureSurface.cpp
    utils.cpp
    XwaDrawBracketHook.cpp
    XwaDrawRadarHook.cpp
    XwaDrawTextHook.cpp
)
set(ddraw_headers
    BackbufferSurface.h
    common.h
    ComPtr.h
    config.h
    DepthSurface.h
    DeviceResources.h
    Direct3D.h
    Direct3DDevice.h
    Direct3DExecuteBuffer.h
    Direct3DMaterial.h
    Direct3DTexture.h
    Direct3DViewport.h
    DirectDraw.h
    DirectDraw2.h
)

add_library(ddraw SHARED ${ddraw_sources})
set_property(TARGET ddraw PROPERTY CXX_STANDARD 20)
set_property(TARGET ddraw PROPERTY PREFIX "")
add_dependencies(ddraw shaders)
target_include_directories(ddraw PRIVATE ${CMAKE_BINARY_DIR})
if (NOT MSVC)
  target_link_libraries(ddraw gdiplus d3d11 dxgi dxguid winmm xinput9_1_0 -Bsymbolic -static-libgcc -static-libstdc++)
endif()
install(TARGETS ddraw DESTINATION .)
install(FILES ddraw.cfg nopowervr.txt DESTINATION .)
