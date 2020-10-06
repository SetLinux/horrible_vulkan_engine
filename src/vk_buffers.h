#pragma once
#include "vk_mem_alloc.h"
#include <vulkan/vulkan_core.h>
#include <vector>
#include "vk_initializers.h"
#include "stb_image.h"
#include <string_view>
#include <string>
struct AllocatedBuffer{
	VkBuffer buffer; VmaAllocation allocation;
};
struct AllocatedImage{
	VkImage image;
	VmaAllocation allocation;
	VkImageView view;
};
template<typename T>
struct VBO{
	std::vector<T> vertices;	
	AllocatedBuffer buffer;
	void initialize_buffer(const VmaAllocator& allocator,uint32_t buffer_size){
		VkBufferCreateInfo buffer_info = {};
		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_info.pNext = nullptr;
		buffer_info.size = buffer_size;
		buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		VmaAllocationCreateInfo alloc_create_info = {};
		alloc_create_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		vmaCreateBuffer(allocator, &buffer_info, &alloc_create_info, &buffer.buffer, &buffer.allocation, nullptr);
	}
	void push_data(const VmaAllocator& allocator,T* data,uint32_t count){
		void* d;
		vmaMapMemory(allocator, buffer.allocation,&d);
		memcpy(d, data, count *  sizeof(T));
		vmaUnmapMemory(allocator, buffer.allocation);
		
	}
};
AllocatedBuffer allocate_buffer(const VmaAllocator& allocator,size_t allocation_size,VkBufferUsageFlags buf_usage_falgs, VmaMemoryUsage mem_usage);
AllocatedImage allocate_image(const VkDevice& device,const VkQueue& queue,const VmaAllocator& allocator,const std::string& file_name);
