
# Add source to this project's executable.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable (vulkan_guide 
"main.cpp"
"vk_engine.cpp"
 "vk_engine.h" 
 "vk_types.h" 
 "vk_initializers.cpp" 
 "vk_initializers.h"
 "VkBootstrap.cpp"
 "vk_mem_alloc.cpp"
 "vk_buffers.cpp"
 "stb_image.cpp")



set_property(TARGET vulkan_guide PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:vulkan_guide>")

target_include_directories(vulkan_guide PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(vulkan_guide  vkbootstrap vma glm tinyobjloader imgui stb_image dl)

target_link_libraries(vulkan_guide Vulkan::Vulkan sdl2)
