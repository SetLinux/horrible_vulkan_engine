#version 450

//output write
layout (location = 0) out vec4 outFragColor;
layout (location = 0) in vec2 uTexCoord;
layout(set = 0, binding = 0) uniform  CameraBuffer{   
	vec4 color;
} cameraData;

layout(set = 0, binding = 1) uniform sampler2D tex1;
layout( push_constant ) uniform constants
{
	vec4 data;
} PushConstants;

void main() 
{
		outFragColor = texture(tex1,uTexCoord);
}
