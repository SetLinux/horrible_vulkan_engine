#include "vk_buffers.h"
#include "vk_initializers.h"
#include "vk_mem_alloc.h"
#include <cstring>
#include <vulkan/vulkan_core.h>
AllocatedBuffer allocate_buffer(const VmaAllocator& allocator,size_t allocation_size, VkBufferUsageFlags buf_usage_falgs, VmaMemoryUsage mem_usage){
	AllocatedBuffer ab;
	VkBufferCreateInfo buffer_create_info = {};
	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.pNext = nullptr;
	buffer_create_info.size = allocation_size;
	buffer_create_info.usage = buf_usage_falgs;
	VmaAllocationCreateInfo alloc_create_info = {};
	alloc_create_info.usage = mem_usage;
	vmaCreateBuffer(allocator,&buffer_create_info,&alloc_create_info,&ab.buffer,&ab.allocation,nullptr);
	return ab;
};
AllocatedImage allocate_image(const VkDevice& device,const VkQueue& queue,const VmaAllocator& allocator,const std::string& file_name){

	AllocatedImage the_image;


	int width{0},height{0},channels{0};
	stbi_uc* image_data = stbi_load(file_name.c_str(), &width, &height, &channels,STBI_rgb_alpha);
	size_t image_size = width * height * 4;
	AllocatedBuffer staging_buffer = allocate_buffer(allocator, image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
	void* buf_data;
	vmaMapMemory(allocator, staging_buffer.allocation, &buf_data);
	memcpy(buf_data,image_data,image_size);
	vmaUnmapMemory(allocator,staging_buffer.allocation);

	VkExtent3D image_extent;
	image_extent.width = static_cast<uint32_t>(width);
	image_extent.height = static_cast<uint32_t>(height);
	image_extent.depth = 1;
	auto allocation_image_buffer_info = vkinit::image_create(image_extent,VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,VK_FORMAT_R8G8B8A8_UNORM);
	VmaAllocationCreateInfo alloc_info = {};
	alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	vmaCreateImage(allocator,&allocation_image_buffer_info,&alloc_info,&the_image.image,&the_image.allocation,nullptr);
	vkinit::UpdateContext uc;
	uc.init_context(device);
	uc.immediate_submit(device, queue, [&](VkCommandBuffer cmd){
		VkImageSubresourceRange sb_r = {};
		sb_r.levelCount = 1;
		sb_r.layerCount = 1;
		sb_r.baseArrayLayer = 0;
		sb_r.baseMipLevel = 0;
		sb_r.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		VkImageMemoryBarrier img_mem_barrier = {};
		img_mem_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		img_mem_barrier.pNext = nullptr;
		img_mem_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		img_mem_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

		img_mem_barrier.image = the_image.image;
		img_mem_barrier.subresourceRange = sb_r;
		img_mem_barrier.srcAccessMask = 0;
		img_mem_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkCmdPipelineBarrier(cmd,VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &img_mem_barrier);
		VkBufferImageCopy buff_img_cpy = {};
		buff_img_cpy.imageExtent = image_extent;
		buff_img_cpy.bufferImageHeight = 0;
		buff_img_cpy.bufferOffset = 0;
		buff_img_cpy.bufferRowLength = 0;
		buff_img_cpy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		buff_img_cpy.imageSubresource.layerCount = 1;
		buff_img_cpy.imageSubresource.baseArrayLayer = 0;
		buff_img_cpy.imageSubresource.mipLevel = 0;
		vkCmdCopyBufferToImage(cmd,staging_buffer.buffer,the_image.image,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1,&buff_img_cpy);
		VkImageMemoryBarrier img_readable_barrier = img_mem_barrier;
		img_readable_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		img_readable_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		img_readable_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		img_readable_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &img_readable_barrier);

			});
		auto view_info = vkinit::image_view_create(VK_FORMAT_R8G8B8A8_UNORM,the_image.image,VK_IMAGE_ASPECT_COLOR_BIT);
		vkCreateImageView(device,&view_info,nullptr,&the_image.view);
		return the_image;	
};
