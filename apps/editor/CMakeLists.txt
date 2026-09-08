# ============================================================
#  UNIVERSAL ENGINE — editor (aplicación)
# ============================================================

# Backends de Dear ImGui para GLFW + OpenGL3 (solo se usan aquí).
add_library(ue_imgui_backends STATIC
    "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
)
target_include_directories(ue_imgui_backends PUBLIC
    "${imgui_SOURCE_DIR}"
    "${imgui_SOURCE_DIR}/backends"
)
target_compile_features(ue_imgui_backends PUBLIC cxx_std_17)
target_link_libraries(ue_imgui_backends PUBLIC ue_imgui glfw)

# ---- Aplicación del editor ---------------------------------
add_executable(UE_Editor
    EditorApp.cpp
    main.cpp
)
target_compile_features(UE_Editor PRIVATE cxx_std_17)
target_link_libraries(UE_Editor PRIVATE engine ue_imgui_backends)

# Una vez compilado, copia los recursos junto al ejecutable para que la app
# sea portable (la plantillas y el branding se resuelven desde el exe).
add_custom_command(TARGET UE_Editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets/templates"
            "$<TARGET_FILE_DIR:UE_Editor>/assets/templates"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets/branding"
            "$<TARGET_FILE_DIR:UE_Editor>/assets/branding"
    COMMENT "Recursos copiados junto a UE_Editor.exe"
)

if(MSVC)
    target_compile_options(UE_Editor PRIVATE /W4 /permissive-)
else()
    target_compile_options(UE_Editor PRIVATE -Wall -Wextra -Wpedantic)
endif()