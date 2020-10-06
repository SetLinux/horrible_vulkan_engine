#!/bin/bash

for filename in ~/cpp_projects/vulkan_engine/shaders/*.vert; do
	rm $filename.spv
	glslangValidator -V --target-env vulkan1.1 $filename -o $filename.spv 	
done
for filename in ~/cpp_projects/vulkan_engine/shaders/*.frag; do
	rm $filename.spv
	glslangValidator -V --target-env vulkan1.1 $filename -o $filename.spv 	
done
