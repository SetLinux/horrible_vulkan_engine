#include <fstream>
#include <vk_initializers.h>
#include <vulkan/vulkan_core.h>
void vkinit::create_render_pass(VkDevice device,VkSubpassDescription subpasses,VkAttachmentDescription attachments,VkRenderPass* render_pass){
	VkRenderPassCreateInfo render_pass_create_info = {};
	render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_create_info.subpassCount =1; 
	render_pass_create_info.pSubpasses = &subpasses;
	render_pass_create_info.attachmentCount = 1;
	render_pass_create_info.pAttachments = &attachments;
	render_pass_create_info.pNext = nullptr;
	render_pass_create_info.flags = 0;
	render_pass_create_info.dependencyCount = 0;
	vkCreateRenderPass(device,&render_pass_create_info ,nullptr, render_pass);
}

VkPipelineShaderStageCreateInfo vkinit::pipeline_shader_stage_create(VkShaderModule module,VkShaderStageFlagBits flags){
	VkPipelineShaderStageCreateInfo shader_stage_create_info = {};
	shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stage_create_info.pNext = nullptr;
	shader_stage_create_info.flags = 0;
	shader_stage_create_info.stage = flags;
	shader_stage_create_info.module = module;
	shader_stage_create_info.pName = "main";
	return shader_stage_create_info;
}
VkPipelineVertexInputStateCreateInfo vkinit::pipeline_vertex_input_state_create(std::vector<VkVertexInputAttributeDescription>& attr_description,std::vector<VkVertexInputBindingDescription>& bindings_description){
	VkPipelineVertexInputStateCreateInfo state_create_info = {};
	state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	state_create_info.pNext = nullptr;
	state_create_info.vertexBindingDescriptionCount = bindings_description.size();
	state_create_info.pVertexBindingDescriptions = bindings_description.data();
	state_create_info.vertexAttributeDescriptionCount = attr_description.size();
	state_create_info.pVertexAttributeDescriptions = attr_description.data();
	state_create_info.flags = 0;
	return state_create_info;
}

VkPipelineInputAssemblyStateCreateInfo vkinit::pipeline_input_assembly_create_info(VkPrimitiveTopology topo){
	VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {};
	input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly_info.pNext = nullptr;
	input_assembly_info.topology = topo;
	input_assembly_info.flags = 0;
	return input_assembly_info;
}
VkPipelineRasterizationStateCreateInfo vkinit::pipeline_rasterization_state_create(VkPolygonMode mode){
	VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {};
	rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization_state_create_info.pNext = nullptr;
	rasterization_state_create_info.polygonMode = mode;
	rasterization_state_create_info.lineWidth = 1.0f;
	rasterization_state_create_info.depthBiasConstantFactor = 0.0f;
	rasterization_state_create_info.flags = 0;
	rasterization_state_create_info.cullMode = VK_CULL_MODE_NONE;
	rasterization_state_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterization_state_create_info.depthBiasEnable = VK_FALSE;
	rasterization_state_create_info.depthBiasClamp = 0.0f;
	rasterization_state_create_info.depthBiasSlopeFactor = 0.0f;
	return rasterization_state_create_info;
}
VkPipelineMultisampleStateCreateInfo vkinit::multisample_state_create(){
	VkPipelineMultisampleStateCreateInfo multisample_create_info = {};
	multisample_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample_create_info.minSampleShading = 1.0;
	multisample_create_info.pNext = nullptr;
	multisample_create_info.flags = 0;
	multisample_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisample_create_info.pSampleMask = nullptr;
	multisample_create_info.sampleShadingEnable = VK_FALSE;
	multisample_create_info.alphaToOneEnable = VK_FALSE;
	multisample_create_info.sampleShadingEnable = VK_FALSE;
	return multisample_create_info;
}
VkPipelineColorBlendAttachmentState vkinit::color_blend_attachment_state_create(){
	VkPipelineColorBlendAttachmentState color_blend_attachment_state = {};
	color_blend_attachment_state.colorWriteMask =  VK_COLOR_COMPONENT_R_BIT |VK_COLOR_COMPONENT_G_BIT |VK_COLOR_COMPONENT_B_BIT |VK_COLOR_COMPONENT_A_BIT ;
	color_blend_attachment_state.blendEnable = VK_FALSE;
	return color_blend_attachment_state;
}

VkPipelineColorBlendStateCreateInfo vkinit::color_blend_state_create(bool logic_op_enable, VkLogicOp logic_op, VkPipelineColorBlendAttachmentState *color_blend_states, size_t color_blend_attachment_state_count){
	VkPipelineColorBlendStateCreateInfo pipeline_blend_state_create_info = {};
	pipeline_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	pipeline_blend_state_create_info.pNext = nullptr;
	pipeline_blend_state_create_info.flags = 0;
	pipeline_blend_state_create_info.logicOpEnable = logic_op_enable;
	pipeline_blend_state_create_info.logicOp = logic_op;
	pipeline_blend_state_create_info.attachmentCount = color_blend_attachment_state_count;
	pipeline_blend_state_create_info.pAttachments = color_blend_states;
	
	return pipeline_blend_state_create_info;
}
VkPipelineViewportStateCreateInfo vkinit::viewport_state_create(VkViewport* viewport, VkRect2D* scissors){
	VkPipelineViewportStateCreateInfo viewport_state_create = {};
	viewport_state_create.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state_create.pNext = nullptr;
	viewport_state_create.scissorCount = 1;
	viewport_state_create.pScissors = scissors;
	viewport_state_create.viewportCount = 1;
	viewport_state_create.pViewports = viewport;
	viewport_state_create.flags = 0;
	return viewport_state_create;
}
VkPipeline vkinit::PipelineBuilder::create_pipeline(VkDevice device, VkRenderPass render_pass){
	std::cout << " Current viewport " << viewport.width << " : " << viewport.height << std::endl;
	auto viewport_state = vkinit::viewport_state_create(&viewport, &scissors);	
	auto multisample_state = vkinit::multisample_state_create();
	std::vector<VkPipelineShaderStageCreateInfo> shader_stages_create_info = {};
	for(auto& [module,flags]: this->shader_stages){
	//	VkShaderModule module = std::get<0>(element);	
	//	VkShaderStageFlagBits flags = std::get<1>(element);	
		shader_stages_create_info.push_back(vkinit::pipeline_shader_stage_create(module, flags));
	}
	auto rasterization_state = vkinit::pipeline_rasterization_state_create(this->poly_mode);
	auto vertex_input_state = vkinit::pipeline_vertex_input_state_create(this->attr_description, this->bindings_description);
	auto vertex_input_assembly_state = vkinit::pipeline_input_assembly_create_info(this->topology);
	auto color_blend_attachment = vkinit::color_blend_attachment_state_create();
	auto color_blend_state = vkinit::color_blend_state_create(this->color_blend_state.logic_op_enable,this->color_blend_state.logic_op,&color_blend_attachment,1);
	VkGraphicsPipelineCreateInfo g_pipeline_info = {};
	g_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	g_pipeline_info.pNext = nullptr;
	g_pipeline_info.pInputAssemblyState = &vertex_input_assembly_state;
	g_pipeline_info.pMultisampleState = &multisample_state;
	g_pipeline_info.pVertexInputState = &vertex_input_state;
	g_pipeline_info.pColorBlendState = &color_blend_state;
	g_pipeline_info.pRasterizationState = &rasterization_state;
	g_pipeline_info.pViewportState = &viewport_state;
	g_pipeline_info.renderPass = render_pass;
	g_pipeline_info.pStages = shader_stages_create_info.data();
	g_pipeline_info.stageCount = shader_stages_create_info.size();

	g_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
	g_pipeline_info.basePipelineIndex = 0;
	VkPipelineLayoutCreateInfo pllci = vkinit::pipeline_layout_create(this->push_constant_ranges,this->descriptor_layout);
	vkCreatePipelineLayout(device,&pllci, nullptr,&this->layout);
	g_pipeline_info.layout = layout;
	VkPipeline pipeline_object;
	if(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &g_pipeline_info, nullptr, &pipeline_object)){
		std::cout << "Creating a graphics pipeline failed" << std::endl;
		return VK_NULL_HANDLE;
	}else{
		return pipeline_object;
	}
}
VkShaderModule vkinit::compile_shader(VkDevice device,std::string_view path){
	std::ifstream input_stream(std::string(path),std::ios::binary | std::ios::in | std::ios::ate);
	uint32_t file_size = input_stream.tellg();
	std::vector<uint32_t> file_data = std::vector<uint32_t>(file_size / sizeof(uint32_t));
	input_stream.seekg(0); input_stream.read((char*)file_data.data(),file_size);
	input_stream.close();
	VkShaderModuleCreateInfo shader_create_info = {};
	shader_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shader_create_info.pNext = nullptr;
	shader_create_info.codeSize = file_size;
	shader_create_info.pCode = file_data.data();
	VkShaderModule shader_module = {};
	vkCreateShaderModule(device, &shader_create_info, nullptr, &shader_module);
	return shader_module;
}
VkPipelineLayoutCreateInfo vkinit::pipeline_layout_create(const std::vector<VkPushConstantRange>& push_ranges,std::vector<VkDescriptorSetLayout>& descriptor_layout){
	VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
	pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_create_info.pNext = nullptr;
	pipeline_layout_create_info.pushConstantRangeCount = push_ranges.size();
	pipeline_layout_create_info.pPushConstantRanges = push_ranges.data();
	pipeline_layout_create_info.setLayoutCount = descriptor_layout.size();
	pipeline_layout_create_info.pSetLayouts = descriptor_layout.data(); 
	pipeline_layout_create_info.flags = 0;
	return pipeline_layout_create_info;
}
VkPushConstantRange vkinit::push_constant_range_create(uint32_t offset, size_t size,VkShaderStageFlags flags){
	VkPushConstantRange pcr = {};
	pcr.offset = offset;
	pcr.size = size;
	pcr.stageFlags = flags;
	return pcr;
}
void vkinit::UpdateContext::init_context(VkDevice device){
	VkFenceCreateInfo fence_create_info = {};
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_create_info.pNext = nullptr;
	fence_create_info.flags = 0;//VK_FENCE_CREATE_SIGNALED_BIT;
	vkCreateFence(device,&fence_create_info,nullptr, &upload_fence);
	VkCommandPoolCreateInfo pool_create_info = {};
	pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_create_info.pNext = nullptr;
	pool_create_info.flags = 0;
	vkCreateCommandPool(device, &pool_create_info, nullptr, &command_pool);
};
void vkinit::UpdateContext::immediate_submit(VkDevice device,VkQueue queue,std::function<void (VkCommandBuffer)> function){
	
	VkCommandBufferAllocateInfo cmd_alloc_info = {};
	cmd_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmd_alloc_info.pNext = nullptr;
	cmd_alloc_info.commandPool = command_pool;
	cmd_alloc_info.commandBufferCount = 1;
	VkCommandBuffer cmd;
	vkAllocateCommandBuffers(device, &cmd_alloc_info, &cmd);
	VkCommandBufferBeginInfo cmd_begin_info = {};
	cmd_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmd_begin_info.pNext = nullptr;
	cmd_begin_info.pInheritanceInfo = 0;
	cmd_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(cmd,&cmd_begin_info);
	function(cmd);
	vkEndCommandBuffer(cmd);
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.pNext = nullptr;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &cmd;
	
	vkQueueSubmit(queue, 1, &submit_info, upload_fence);
	vkWaitForFences(device, 1, &upload_fence, true, 999999999);
	vkResetFences(device, 1, &upload_fence);
	vkResetCommandPool(device, command_pool,0); 
};

VkImageCreateInfo vkinit::image_create(const VkExtent3D& extent,const VkImageUsageFlags& image_usage,const VkFormat& image_format){
	VkImageCreateInfo image_create_info = {};
	image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_create_info.pNext = nullptr;
	image_create_info.imageType = VK_IMAGE_TYPE_2D;
	image_create_info.usage =  image_usage;
	image_create_info.format = image_format;
	image_create_info.extent = extent;
	image_create_info.mipLevels = 1;
	image_create_info.arrayLayers = 1;
	image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
	return image_create_info;
};
VkImageViewCreateInfo vkinit::image_view_create(const VkFormat& format, const VkImage& image, const VkImageAspectFlags& flags){
	VkImageViewCreateInfo view_create_info = {};
	view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_create_info.pNext = nullptr;
	view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view_create_info.format = format;
	view_create_info.image = image;
	view_create_info.subresourceRange.aspectMask = flags;
	view_create_info.subresourceRange.levelCount = 1;
	view_create_info.subresourceRange.baseMipLevel = 0;
	view_create_info.subresourceRange.layerCount = 1;
	view_create_info.subresourceRange.baseArrayLayer = 0;
	return view_create_info;
};
VkSamplerCreateInfo vkinit::sampler_create(VkFilter filters,VkSamplerAddressMode address_mode){
	VkSamplerCreateInfo sampler_create_info = {};
	sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_create_info.pNext = nullptr;
	sampler_create_info.minFilter = filters;
	sampler_create_info.magFilter = filters;
	sampler_create_info.addressModeU = address_mode;
	sampler_create_info.addressModeW = address_mode;
	sampler_create_info.addressModeV = address_mode;
	return sampler_create_info;
};
