// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#include "vk_buffers.h"
#include <iostream>
#include <vk_types.h>
#include "VkBootstrap.h"
#include "vk_mem_alloc.h"
#include <vector>
#include <vulkan/vulkan_core.h>

struct DescData{
	glm::vec4 color;
};
struct FrameData{
	VkSemaphore render_semaphore,present_semaphore;
	VkFence render_fence;
	VkCommandPool command_pool;
	VkCommandBuffer command_buffer;
	AllocatedBuffer data;
	VkDescriptorSet desc_set;
	VkSampler blocky_sampler;
};
class VulkanEngine {
public:

	bool _isInitialized{ false };
	int _frameNumber {0};

	VkExtent2D _windowExtent{ 1700 , 900 };

	struct SDL_Window* _window{ nullptr };

	//initializes everything in the engine
	void init();
	//shuts down the engine
	void cleanup();

	//draw loop
	void draw();

	//run main loop
	void run();
public:
	void init_vulkan();
	vkb::Instance vkb_instance;
	vkb::PhysicalDevice vkb_physicaldevice;
	vkb::Device vkb_device;
	VkSurfaceKHR surface;
	VkQueue graphics_queue;
	uint32_t graphics_queue_index;
public:
	vkb::Swapchain vkb_swapchain;
	VkFormat swapchain_format;
	std::vector<VkImage> swapchain_images;
	std::vector<VkImageView> swapchain_images_views;
	void init_swapchain();
	std::vector<VkFramebuffer> framebuffers;
	void create_default_render_pass();
	void create_frame_buffers();
	void init_sync_structures();
	VkRenderPass main_render_pass;
	FrameData frames_data[2];
public:
	void init_command_buffer();
public:
	VkShaderModule vertex_shader,fragment_shader;
	void init_shaders();
	VkPipeline triangle_pipeline;
	void init_pipeline();
	VBO<Vertex> vbo;
	VmaAllocator allocator;
	VkPipelineLayout triangle_pipeline_layout;
	inline FrameData& get_current_frame(){
		return frames_data[_frameNumber % 2];
	};
	VkDescriptorPool desc_pool;
	VkDescriptorSetLayout global_set_layout;
public:
	void init_descriptors();
	AllocatedImage test_image;
};
struct PushConstantData{
	glm::vec4 color;
};
