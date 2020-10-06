
#include "vk_engine.h"
#include "VkBootstrap.h"
#include "vk_buffers.h"
#include "vk_mem_alloc.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <cstddef>
#include <cstring>
#include <tuple>
#include <vk_types.h>
#include <vk_initializers.h>
#include <vulkan/vulkan_core.h>
#define VK_CHECK(x) \
		do{  \
				VkResult err = x; \
					if(err){         \
								std::cout << " Detected Vulkan error : " << err << std::endl; \
						std::cout << __LINE__ << std::endl; \
								abort();}   \
				}while(0)
void VulkanEngine::init()
{
	// We initialize SDL and create a window with it. 
	SDL_Init(SDL_INIT_VIDEO);

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);
	
	_window = SDL_CreateWindow(
		"Vulkan Engine",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		_windowExtent.width,
		_windowExtent.height,
		window_flags
	);
	init_vulkan();	
	init_swapchain();
	create_default_render_pass();
	create_frame_buffers();
	init_command_buffer();
	init_sync_structures();
	init_shaders();
	test_image = allocate_image(vkb_device.device,graphics_queue,allocator,"../wall.jpg");	
	init_descriptors();
	init_pipeline();

	//everything went fine
	_isInitialized = true;
}
void VulkanEngine::cleanup()
{	
	if (_isInitialized) {
		
		SDL_DestroyWindow(_window);
	}
}
void VulkanEngine::init_vulkan(){
	vkb::InstanceBuilder instance_builder;
	auto inst_builder_ret_value = instance_builder.set_engine_name("Vulkan Engine")
		.set_app_name("Just an app")
		.desire_api_version(1, 1,0)
		.request_validation_layers(true)
		.enable_extension("VK_KHR_xlib_surface")
		.build();
	if(!inst_builder_ret_value.has_value()){
		std::cout <<"[*] failed to create vkdevice" << std::endl;
		abort();
	}
	vkb_instance = inst_builder_ret_value.value();	
	SDL_Vulkan_CreateSurface(_window, this->vkb_instance.instance, &surface);
	vkb::PhysicalDeviceSelector selector(vkb_instance);
	vkb_physicaldevice = selector
		.set_minimum_version(1,1)
		.set_surface(this->surface)
		.select().value();
	vkb::DeviceBuilder device_builder{vkb_physicaldevice};
	vkb_device = device_builder.build().value();
	graphics_queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
	graphics_queue_index = vkb_device.get_queue_index(vkb::QueueType::graphics).value();
		
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = vkb_physicaldevice.physical_device;
	allocatorInfo.device = vkb_device.device;
	allocatorInfo.instance = vkb_instance.instance;
	vmaCreateAllocator(&allocatorInfo, &allocator);
}
void VulkanEngine::init_swapchain(){
	vkb::SwapchainBuilder swapchain_builder{vkb_device};
	vkb_swapchain = swapchain_builder
		.use_default_format_selection()
		.set_desired_extent(_windowExtent.width,_windowExtent.height)
		.set_desired_present_mode(VK_PRESENT_MODE_IMMEDIATE_KHR)
		.build().value();
	swapchain_images = vkb_swapchain.get_images().value();
	swapchain_images_views = vkb_swapchain.get_image_views().value();
	swapchain_format = vkb_swapchain.image_format;
	//framebuffer_create_info.
}
void VulkanEngine::create_default_render_pass(){
	std::vector<VkAttachmentDescription> attachments_description= {};
	VkAttachmentDescription color_attachment = {};

	color_attachment.format = swapchain_format;
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	attachments_description.push_back(color_attachment);
	VkAttachmentReference color_attachment_ref = {};
	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	std::vector<VkSubpassDescription> subpass_description = {};
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;
	subpass_description.push_back(subpass);
	vkinit::create_render_pass(vkb_device.device, subpass, color_attachment,&main_render_pass);

}
void VulkanEngine::create_frame_buffers(){
	VkFramebufferCreateInfo framebuffer_create_info = {};
	framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebuffer_create_info.width = _windowExtent.width;
	framebuffer_create_info.height = _windowExtent.height;
	framebuffer_create_info.attachmentCount = 1;
	framebuffer_create_info.attachmentCount = 1;
	framebuffer_create_info.pNext = nullptr;
	framebuffer_create_info.renderPass = main_render_pass;
	framebuffer_create_info.layers = 1;
	framebuffers = std::vector<VkFramebuffer>(swapchain_images_views.size());	
	for(int i =0; i < swapchain_images.size();i++){
		framebuffer_create_info.pAttachments = &swapchain_images_views[i];
		vkCreateFramebuffer(vkb_device.device, &framebuffer_create_info, nullptr,&framebuffers[i]);
		std::cout << " FUCK THIS IS A FRAME BUFFER" << std::endl;
	}
}
void VulkanEngine::init_sync_structures(){
	VkSemaphoreCreateInfo semaphore_create_info = {};
	semaphore_create_info.flags = 0;
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphore_create_info.pNext = nullptr;
	for(int i = 0; i < 2; i++){
		vkCreateSemaphore(vkb_device.device, &semaphore_create_info, nullptr, &frames_data[i].render_semaphore);
		vkCreateSemaphore(vkb_device.device, &semaphore_create_info, nullptr, &frames_data[i].present_semaphore);
	}
	VkFenceCreateInfo fence_create_info = {};
	fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_create_info.pNext = nullptr;
	for(int i = 0; i < 2; i++){
		vkCreateFence(vkb_device.device, &fence_create_info, nullptr, &frames_data[i].render_fence);
	}
};
void VulkanEngine::init_command_buffer(){
	VkCommandPoolCreateInfo command_pool_create_info = {};
	command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	command_pool_create_info.pNext = nullptr;
	command_pool_create_info.queueFamilyIndex = graphics_queue_index;
	command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	for(int i = 0; i < 2; i++){
	vkCreateCommandPool(vkb_device.device, &command_pool_create_info, nullptr, &frames_data[i].command_pool);
	VkCommandBufferAllocateInfo cmd_buffer_allocate_info = {};
	cmd_buffer_allocate_info.commandPool = frames_data[i].command_pool;
	cmd_buffer_allocate_info.commandBufferCount = 1;
	cmd_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmd_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmd_buffer_allocate_info.pNext = nullptr;
	vkAllocateCommandBuffers(vkb_device.device, &cmd_buffer_allocate_info, &frames_data[i].command_buffer);
	}
};
void VulkanEngine::init_shaders(){
	vertex_shader = vkinit::compile_shader(vkb_device.device,"../shaders/triangle.vert.spv");
	fragment_shader = vkinit::compile_shader(vkb_device.device,"../shaders/triangle.frag.spv");
};
void VulkanEngine::init_pipeline(){
	vkinit::PipelineBuilder pb;
	pb.shader_stages.push_back({vertex_shader,VK_SHADER_STAGE_VERTEX_BIT});
	pb.shader_stages.push_back({fragment_shader,VK_SHADER_STAGE_FRAGMENT_BIT});
	pb.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	pb.poly_mode = VK_POLYGON_MODE_FILL;
	pb.scissors.extent = _windowExtent;
	pb.scissors.offset = {0,0};
	pb.viewport.width = _windowExtent.width;
	pb.viewport.height = _windowExtent.height;
	pb.color_blend_state.logic_op_enable = false;
	pb.color_blend_state.logic_op = VK_LOGIC_OP_COPY;
	pb.push_constant_ranges.push_back(vkinit::push_constant_range_create(0, sizeof(glm::vec4), VK_SHADER_STAGE_FRAGMENT_BIT));

	VkVertexInputBindingDescription binding_desc = {};
	binding_desc.stride = sizeof(Vertex);
	binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	binding_desc.binding = 0;
	
	VkVertexInputAttributeDescription attr_desc = {};
	attr_desc.binding = 0;
	attr_desc.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attr_desc.offset = offsetof(Vertex, position);
	attr_desc.location = 0;


	VkVertexInputAttributeDescription attr_desc2 = {};
	attr_desc2.binding = 0;
	attr_desc2.format = VK_FORMAT_R32G32_SFLOAT;
	attr_desc2.offset = offsetof(Vertex, uv);
	attr_desc2.location = 1;


	pb.bindings_description.push_back(binding_desc);

	pb.attr_description.push_back(attr_desc);
	pb.attr_description.push_back(attr_desc2);
	pb.descriptor_layout.push_back(global_set_layout);
	triangle_pipeline = pb.create_pipeline(vkb_device.device, main_render_pass);
	triangle_pipeline_layout = pb.layout;
	std::vector<Vertex> vertices;
	Vertex vertex;
	vertex.position = { 1.f, 1.f, 0.0f,0.0f };
	vertex.uv = {1.f,1.f};
	vertices.push_back(vertex);
	vertex.position = {-1.f, 1.f, 0.0f ,0.0f};
	vertex.uv = {0.f,1.f};
	vertices.push_back(vertex);
	vertex.position = { 0.f,-1.f, 0.0f,0.0f };
	vertex.uv = {0.5f,0.5f};
	vertices.push_back(vertex);
	vbo.initialize_buffer(allocator, 3 * sizeof(Vertex));
	vbo.push_data(allocator, vertices.data(), 3);
};
void VulkanEngine::init_descriptors(){
	for(int i =0;i < 2;i++){
		frames_data[i].data = allocate_buffer(allocator, sizeof(DescData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);	
	}
	VkDescriptorSetLayoutBinding desc_layout_binding = {};
	desc_layout_binding.binding = 0;
	desc_layout_binding.descriptorCount = 1;
	desc_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	desc_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	VkDescriptorSetLayoutBinding desc_layout_binding_image = {};
	desc_layout_binding_image.binding = 1;
	desc_layout_binding_image.descriptorCount = 1;
	desc_layout_binding_image.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	desc_layout_binding_image.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding set_array[] = {desc_layout_binding,desc_layout_binding_image};
	VkDescriptorSetLayoutCreateInfo desc_layout_create_info = {};
	desc_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	desc_layout_create_info.pNext = nullptr;
	desc_layout_create_info.bindingCount = 2;
	desc_layout_create_info.pBindings = set_array;
	desc_layout_create_info.flags = 0;
	vkCreateDescriptorSetLayout(vkb_device.device, &desc_layout_create_info,nullptr, &global_set_layout);
	VkDescriptorPoolSize pool_size[] = {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,10},{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,10}};
	VkDescriptorPoolCreateInfo pool_create_info = {};
	pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_create_info.pNext = nullptr;
	pool_create_info.poolSizeCount = 2;
	pool_create_info.pPoolSizes = pool_size;
	pool_create_info.maxSets = 10;
	pool_create_info.flags = 0;
	vkCreateDescriptorPool(vkb_device.device, &pool_create_info, nullptr, &desc_pool);
	for(int i =0;i < 2;i++){
		auto sampler_create_info = vkinit::sampler_create(VK_FILTER_NEAREST);
		vkCreateSampler(vkb_device.device,&sampler_create_info,nullptr,&frames_data[i].blocky_sampler);

		VkDescriptorSetAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		alloc_info.pNext = nullptr;
		alloc_info.descriptorPool = desc_pool;
		alloc_info.descriptorSetCount = 1;
		alloc_info.pSetLayouts = &global_set_layout;
		vkAllocateDescriptorSets(vkb_device.device, &alloc_info, &frames_data[i].desc_set);	
		{
			VkDescriptorBufferInfo buffer_info = {};
			buffer_info.buffer = frames_data[i].data.buffer;
			buffer_info.offset = 0;
			buffer_info.range = sizeof(DescData);
			VkWriteDescriptorSet write_set = {};
			write_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write_set.pNext = nullptr;
			write_set.descriptorCount = 1;
			write_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			write_set.pBufferInfo = &buffer_info;

			write_set.dstBinding = 0;
			write_set.dstSet = frames_data[i].desc_set;
			vkUpdateDescriptorSets(vkb_device.device, 1,&write_set,0, nullptr);
		}
		{
			VkDescriptorImageInfo image_info = {};
			image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			image_info.sampler = frames_data[i].blocky_sampler;
			image_info.imageView = test_image.view;
			
			VkWriteDescriptorSet write_set = {};
			write_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write_set.pNext = nullptr;
			write_set.descriptorCount = 1;
			write_set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			write_set.pImageInfo = &image_info;
			write_set.dstBinding = 1;
			write_set.dstSet = frames_data[i].desc_set;
			vkUpdateDescriptorSets(vkb_device.device, 1,&write_set,0, nullptr);
		}
	}

}
void VulkanEngine::draw()
{
	_frameNumber += 1;
	VK_CHECK(vkWaitForFences(vkb_device.device, 1, &get_current_frame().render_fence, true, 1000000000));	
	VK_CHECK(vkResetFences(vkb_device.device, 1, &get_current_frame().render_fence));
	vkResetCommandBuffer(get_current_frame().command_buffer, 0);
	uint32_t swapchain_image_index;
	vkAcquireNextImageKHR(vkb_device.device, vkb_swapchain.swapchain, 0, get_current_frame().present_semaphore, nullptr, &swapchain_image_index);
	VkCommandBufferBeginInfo cmd_begin_info = {};
	cmd_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmd_begin_info.pNext = nullptr;
	cmd_begin_info.pInheritanceInfo = 0;
	cmd_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	VK_CHECK(vkBeginCommandBuffer(get_current_frame().command_buffer, &cmd_begin_info));
	VkRenderPassBeginInfo render_pass_begin_info = {};
	render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_begin_info.pNext = nullptr;
	render_pass_begin_info.renderArea.offset = VkOffset2D{0,0};
	render_pass_begin_info.renderArea.extent = _windowExtent;
	render_pass_begin_info.framebuffer = framebuffers[swapchain_image_index];
	render_pass_begin_info.clearValueCount = 1;
	VkClearValue clear_value;
	clear_value.color = {1.0,1.0,0.0,1.0};
	render_pass_begin_info.pClearValues  = &clear_value;
	render_pass_begin_info.renderPass = main_render_pass;
	DescData test_data;
	test_data.color = {1.0f,1.0f,1.0f,1.0f};


	void* desc_ptr;
	vmaMapMemory(allocator, get_current_frame().data.allocation, &desc_ptr);
	memcpy(desc_ptr, &test_data, sizeof(DescData));	
	vmaUnmapMemory(allocator, get_current_frame().data.allocation);
	vkCmdBindDescriptorSets(get_current_frame().command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, triangle_pipeline_layout, 0,1, &get_current_frame().desc_set, 0, nullptr);

	vkCmdBeginRenderPass(get_current_frame().command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(get_current_frame().command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, triangle_pipeline);
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(get_current_frame().command_buffer, 0, 1, &vbo.buffer.buffer, &offset);
	PushConstantData pcd;
	pcd.color = glm::vec4(1.0f,0.0f,1.0f,1.0f);
	vkCmdPushConstants(get_current_frame().command_buffer, triangle_pipeline_layout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantData), &pcd);
	vkCmdDraw(get_current_frame().command_buffer,3,1,0,0);
	vkCmdEndRenderPass(get_current_frame().command_buffer);
	
	vkEndCommandBuffer(get_current_frame().command_buffer);
	VkSubmitInfo submit_info = {};
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &get_current_frame().command_buffer;
	VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	submit_info.pWaitDstStageMask = &wait_stage;
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores= &get_current_frame().present_semaphore;
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = &get_current_frame().render_semaphore;
	vkQueueSubmit(graphics_queue,1,&submit_info,get_current_frame().render_fence);
	VkPresentInfoKHR present_info = {};
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = &get_current_frame().render_semaphore;
	
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.swapchainCount = 1;
	present_info.pSwapchains = &vkb_swapchain.swapchain;
	present_info.pImageIndices = &swapchain_image_index;
	vkQueuePresentKHR(graphics_queue, &present_info);
	
}

void VulkanEngine::run()
{
	SDL_Event e;
	bool bQuit = false;

	//main loop
	while (!bQuit)
	{
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//close the window when user alt-f4s or clicks the X button			
			if (e.type == SDL_QUIT) bQuit = true;
		}

		draw();
	}
}

