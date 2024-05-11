
cmake_minimum_required(VERSION 3.24)

project(VulkanRahmenprogrammStudi VERSION 1.0)


find_package(Vulkan REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

message("Cmake Vulkan Module Path: " ${Vulkan_INCLUDE_DIRS})
message("Cmake Vulkan Include Dir: " ${Vulkan_INCLUDE_DIR})

add_subdirectory(dependencies/vkal)
add_subdirectory(dependencies/imageloader)
add_subdirectory(dependencies/tinyobjloader)
add_subdirectory(dependencies/assimp)

add_subdirectory(A1_Versuch1a)
add_subdirectory(A1_Versuch1c)
add_subdirectory(A1_Versuch1.2)
add_subdirectory(A2_Versuch2)
add_subdirectory(A3_Normalenvektoren)
add_subdirectory(A4_Cubemapping)
add_subdirectory(A5_PCSS)
add_subdirectory(A5_Raytracing)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set_property(TARGET vkal            PROPERTY FOLDER "Libraries")
set_property(TARGET tinyobjloader   PROPERTY FOLDER "Libraries")
set_property(TARGET assimp          PROPERTY FOLDER "Libraries")

set_property(TARGET A1_Versuch1a        PROPERTY FOLDER "Aufgaben")
set_property(TARGET A1_Versuch1c        PROPERTY FOLDER "Aufgaben")
set_property(TARGET A1_Versuch1.2       PROPERTY FOLDER "Aufgaben")
set_property(TARGET A2_Versuch2         PROPERTY FOLDER "Aufgaben")
set_property(TARGET A3_Normalenvektoren PROPERTY FOLDER "Aufgaben")
set_property(TARGET A4_Cubemapping      PROPERTY FOLDER "Aufgaben")
set_property(TARGET A5_PCSS             PROPERTY FOLDER "Aufgaben")
set_property(TARGET A5_Raytracing       PROPERTY FOLDER "Aufgaben")



