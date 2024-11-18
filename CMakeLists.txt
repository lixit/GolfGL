cmake_minimum_required (VERSION 3.8)

project ("GolfGL")


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
  message(STATUS "Defaulting configuration to Debug")
endif()


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_PROJECT_NAME})

if(UNIX)  
  cmake_policy(SET CMP0072 NEW)
  find_package(Freetype REQUIRED)
  find_package(fmt REQUIRED)
  message(STATUS "Found Freetype in ${FREETYPE_INCLUDE_DIR_ft2build}")
  
  set(GLAD_LIBRARY "${CMAKE_SOURCE_DIR}/External/GLAD/lib/unix/libglad.a")
  set(IMGUI_LIBRARY "${CMAKE_SOURCE_DIR}/External/imgui/lib/unix/libimgui.a")      

else() # WIN32
  link_directories(${CMAKE_SOURCE_DIR}/External/freetype2/win64 
                  ${CMAKE_SOURCE_DIR}/External/GLFW/lib 
                  ${CMAKE_SOURCE_DIR}/External/assimp/lib
                  ${CMAKE_SOURCE_DIR}/External/GLAD/lib/win64
                  ${CMAKE_SOURCE_DIR}/External/imgui/lib/win64
                  )

  set(FREETYPE_INCLUDE_DIR_ft2build "${CMAKE_SOURCE_DIR}/External/freetype2/include")    
endif(UNIX)

# Include paths for External libs
set(GLAD_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/External/GLAD")
set(IMGUI_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/External/imgui")
set(STB_IMAGE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/External/stb_image")

# find the required packages
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")
find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")



# Grouping libs
if(WIN32)
  set(LIBS glad glfw3 opengl32 assimp freetype imgui)

else() # UNIX
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
  find_package(OpenGL REQUIRED)
  add_definitions(${OPENGL_DEFINITIONS})
  find_package(X11 REQUIRED)  
  
  set(LIBS ${GLAD_LIBRARY} ${GLFW3_LIBRARY} X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl 
      pthread freetype fmt::fmt ${ASSIMP_LIBRARY} ${IMGUI_LIBRARY})
  set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")
endif(WIN32)


set(Source_Root "${CMAKE_SOURCE_DIR}/Game")
file(GLOB_RECURSE GameSrc
  "${Source_Root}/*.cpp"
  "${Source_Root}/*.hpp"
  "${Source_Root}/*.h"
  )

file(GLOB_RECURSE Shaders
  "${Source_Root}/Resources/*.vert"
  "${Source_Root}/Resources/*.frag"
  )

# Game files
set(NAME "GolfGL")

file(
    GLOB_RECURSE Source_List 
    LIST_DIRECTORIES false
    ${GameSrc}
	${Shaders}
)

# Setting clang tidy
option(Clang-Tidy OFF)
if(Clang-Tidy)
  set(CMAKE_CXX_CLANG_TIDY clang-tidy)
endif()

# Setting cmake executable
add_executable(${NAME} ${Source_List})

# Grouping and forming source tree
foreach(SRC_file IN ITEMS ${Source_List})
    get_filename_component(SRC_path "${SRC_file}" PATH)
    file(RELATIVE_PATH SRC_path_rel "${Source_Root}" "${SRC_path}")
    string(REPLACE "/" "\\" SRC_Group "${SRC_path_rel}")
    source_group("${SRC_Group}" FILES "${SRC_file}")
endforeach()


target_link_libraries(${NAME} PRIVATE ${LIBS})


if(MSVC)
                                                          # PCH Use   # inline exp
    set_target_properties(${NAME} PROPERTIES COMPILE_FLAGS "/Yuprecomp.h /Ob1")         
    set_source_files_properties("${Source_Root}/Source/precomp.cpp" PROPERTIES COMPILE_FLAGS "/Ycprecomp.h") # PCH create    
	
	
    set_target_properties(${NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${Source_Root}")	    
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "${NAME}")	        
	  
	       
    # CLANG-TIDY as default code analysis tool in VS
    if(Clang-Tidy)
      set_target_properties(${NAME} PROPERTIES VS_GLOBAL_RunCodeAnalysis "true")
      set_target_properties(${NAME} PROPERTIES VS_GLOBAL_EnableClangTidyCodeAnalysis "true")
      set_target_properties(${NAME} PROPERTIES VS_GLOBAL_ClangTidyChecks "clang-tidy")
      set_target_properties(${NAME} PROPERTIES VS_GLOBAL_EnableMicrosoftCodeAnalysis "false")
    endif()

else()
  target_precompile_headers(${NAME} INTERFACE ${Source_Root}/Source/precomp.h)
  target_compile_definitions(${NAME} PUBLIC $<IF:$<CONFIG:Debug>,_DEBUG,NDEBUG>)
endif(MSVC)


if(WIN32)									
  add_custom_command(TARGET ${NAME} PRE_LINK COMMAND 
              ${CMAKE_COMMAND} -E copy
              "${CMAKE_SOURCE_DIR}/External/assimp/lib/assimp-vc140-mt.dll" $<TARGET_FILE_DIR:${NAME}>)


# Making symbolic link on unix for Resources
else()
  add_custom_command(TARGET ${NAME} PRE_BUILD COMMAND 
        ${CMAKE_COMMAND} -E create_symlink "${Source_Root}/Resources" 
        $<TARGET_FILE_DIR:${NAME}>/Resources)        

endif(WIN32)


# Suppressing warnings from external libs
if(CMAKE_COMPILER_IS_GNUCXX)  
    file(GLOB_RECURSE externHeaders "${CMAKE_SOURCE_DIR}/External/*.h" "${Source_Root}/Source/precomp.cpp")
    set_source_files_properties(${externHeaders} PROPERTIES COMPILE_FLAGS 
    "-Wno-shift-negative-value -Wno-implicit-fallthrough -Wno-unused-but-set-variable -Wno-sign-compare")        
endif()

include_directories(${ASSIMP_INCLUDE_DIR}
                    ${FREETYPE_INCLUDE_DIR_ft2build}
                    ${GLAD_INCLUDE_DIR}
                    ${GLFW3_INCLUDE_DIR}
                    ${IMGUI_INCLUDE_DIR}
                    ${IMGUI_INCLUDE_DIR}/backends
					${STB_IMAGE_INCLUDE_DIR}
                    ${CMAKE_SOURCE_DIR}/External                    
                    ${CMAKE_SOURCE_DIR}/Game/Source
		            ${CMAKE_SOURCE_DIR}/Game/Source/Render
                   )


				
