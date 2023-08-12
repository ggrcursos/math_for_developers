# Defina uma variável com os caminhos das pastas de inclusão que serão usadas para "mfgd"
set(MFGD_INCLUDE_DIRS
    common
    datamanager
    game
    math
    renderer 
)

# Obtenha a lista de arquivos de origem
file(GLOB_RECURSE HEADERS "*.h")

file(GLOB_RECURSE SRCS_CPP "*.cpp")

file(GLOB SRCS_C "renderer/*.c")

# Crie a biblioteca mfgd
add_library(mfgd ${SRCS_CPP} ${SRCS_C} ${HEADERS})

# Adicione as pastas de inclusão aos alvos apropriados
target_include_directories(mfgd PUBLIC ${MFGD_INCLUDE_DIRS})

# target_include_directories(mfgd PUBLIC src)
target_link_libraries(mfgd glfw ${OPENGL_gl_LIBRARY})
if(APPLE)
  target_link_libraries(mfgd ${FREETYPE_LIBRARIES})
endif(APPLE)

# Crie o executável
add_executable(mfgd_exec game/main.cpp ${HEADERS})
target_include_directories(mfgd_exec PRIVATE ${MFGD_INCLUDE_DIRS})
target_link_libraries(mfgd_exec mfgd)
