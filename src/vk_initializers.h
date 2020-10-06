// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.
#pragma once
#include <iostream>
#include <vk_types.h>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <tuple>
#include <string>
#include <string_view>
#include <functional>
namespace vkinit {
	// ================================
	// Pipeline Builder Class 	  |
	// ================================
	struct PipelineBuilder{
		std::vector<std::tuple<VkShaderModule,VkShaderStageFlagBits>> shader_stages;
		VkViewport viewport;
		VkRect2D scissors; std::vector<VkPushConstantRange> push_constant_ranges;
		std::vector<VkDescriptorSetLayout> descriptor_layout;
		std::vector<VkVertexInputAttributeDescription> attr_description;
		std::vector<VkVertexInputBindingDescription> bindings_description;
		VkPrimitiveTopology topology;
		VkPolygonMode poly_mode;
		struct {
			bool logic_op_enable;
			VkLogicOp logic_op;
		}color_blend_state;
		VkPipelineLayout layout;		
		VkPipeline create_pipeline(VkDevice device, VkRenderPass render_pass);
	};
	//=================================
	// Render Pass Related Functions  |
	// ================================
	void create_render_pass(VkDevice device,VkSubpassDescription subpasses,VkAttachmentDescription attachments,VkRenderPass* render_pass);
	// =================================
	// Pipeline Related Functions	   |
	// =================================
	VkPipelineShaderStageCreateInfo pipeline_shader_stage_create(VkShaderModule module,VkShaderStageFlagBits flags);
	VkPipelineVertexInputStateCreateInfo pipeline_vertex_input_state_create(std::vector<VkVertexInputAttributeDescription>& attr_description,std::vector<VkVertexInputBindingDescription>& bindings_description);
	VkPipelineInputAssemblyStateCreateInfo pipeline_input_assembly_create_info(VkPrimitiveTopology topo);
	VkPipelineRasterizationStateCreateInfo pipeline_rasterization_state_create(VkPolygonMode mode);
	VkPipelineMultisampleStateCreateInfo multisample_state_create();
	VkPipelineColorBlendAttachmentState color_blend_attachment_state_create();
	VkPipelineColorBlendStateCreateInfo color_blend_state_create(bool logic_op_enable,VkLogicOp logic_op,VkPipelineColorBlendAttachmentState* color_blend_states,size_t color_blend_attachment_state_count);
	VkPipelineViewportStateCreateInfo viewport_state_create(VkViewport* viewport,VkRect2D* scissors);
	VkPipelineLayoutCreateInfo pipeline_layout_create(const std::vector<VkPushConstantRange>& push_ranges,std::vector<VkDescriptorSetLayout>& descriptor_layout);

	VkPushConstantRange push_constant_range_create(uint32_t offset,size_t size,VkShaderStageFlags flags); 
	// ==================================
	// Shader Related Functions  	    |
	// =================================as
	VkShaderModule compile_shader(VkDevice device,std::string_view path);	
	struct UpdateContext{
		VkFence upload_fence;
		VkCommandPool command_pool;
		void init_context(VkDevice device);
	
		void immediate_submit(VkDevice device,VkQueue queue,std::function<void(VkCommandBuffer)> function);
	};
	// ================================
	// Image Realted Functions        |
	// ================================
	VkImageCreateInfo image_create(const VkExtent3D& extent,const VkImageUsageFlags& image_usage,const VkFormat& image_format);	
	VkImageViewCreateInfo image_view_create(const VkFormat& format,const VkImage& image,const VkImageAspectFlags& flags);
	VkSamplerCreateInfo sampler_create(VkFilter filters,VkSamplerAddressMode address_mode = VK_SAMPLER_ADDRESS_MODE_REPEAT);
}


