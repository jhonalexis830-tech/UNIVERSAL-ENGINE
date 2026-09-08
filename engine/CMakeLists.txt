# ============================================================
#  UNIVERSAL ENGINE — biblioteca del motor (núcleo SOLID)
# ============================================================

# ImGui como biblioteca estática del motor (solo el núcleo; los backends de
# GLFW/OpenGL son responsabilidad de la aplicación, que también los enlaza).
add_library(ue_imgui STATIC
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui_SOURCE_DIR}/imgui_tables.cpp"
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
)
target_include_directories(ue_imgui PUBLIC "${imgui_SOURCE_DIR}")
target_compile_features(ue_imgui PUBLIC cxx_std_17)

# ---- Motor --------------------------------------------------
file(GLOB_RECURSE UE_ENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp"
)

add_library(engine STATIC ${UE_ENGINE_SOURCES})
target_include_directories(engine PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include"
)
target_compile_features(engine PUBLIC cxx_std_17)
target_link_libraries(engine PUBLIC ue_imgui glfw)

# ---- Compilador ---------------------------------------------
if(MSVC)
    target_compile_options(engine PRIVATE /W4 /permissive-)
else()
    target_compile_options(engine PRIVATE -Wall -Wextra -Wpedantic)
endif()

# El editor usa OpenGL 4.3+; declaramos los tipos GL en Gl.hpp (sin gl.h).
target_compile_definitions(engine PRIVATE GLFW_INCLUDE_NONE)