set(ddraw_ps_shaders
    LanczosScalePixelShader.hlsl
    MainPixelShader.hlsl
    MainPixelShaderBpp2ColorKey00.hlsl
    MainPixelShaderBpp2ColorKey20.hlsl
    MainPixelShaderBpp4ColorKey20.hlsl
    PixelShaderAtest565.hlsl
    PixelShaderAtestDiscardBlack.hlsl
    PixelShaderAtestTexture.hlsl
    PixelShaderAtestTextureNoAlpha.hlsl
    PixelShaderSolid.hlsl
    PixelShaderTexture.hlsl
    SmoothScalePixelShader.hlsl
)
set(ddraw_vs_shaders MainVertexShader.hlsl VertexShader.hlsl)

foreach(FILE ${ddraw_ps_shaders} ${ddraw_vs_shaders})
  get_filename_component(FILE_NOEXT ${FILE} NAME_WE)
  set(FILE_DST ${CMAKE_BINARY_DIR}/${FILE_NOEXT}.h)
  set(SHADER_TYPE ps)
  if (${FILE} IN_LIST ddraw_vs_shaders)
    set(SHADER_TYPE vs)
  endif()
  add_custom_command(OUTPUT ${FILE_DST}
                     COMMAND fxc /nologo /T${SHADER_TYPE}_4_0 /O1 /Vn g_${FILE_NOEXT} /Fh ${FILE_DST} ${FILE}
                     COMMENT "HLSL ${FILE}"
                     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                     DEPENDS ${FILE}
                     VERBATIM)
   list(APPEND ddraw_shader_headers ${FILE_DST})
endforeach(FILE)

add_library(shaders INTERFACE ${ddraw_shader_headers})
