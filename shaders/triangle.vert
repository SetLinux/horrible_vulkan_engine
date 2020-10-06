//we will be using glsl version 4.5 syntax
#version 450
layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec2 vTexCoord;
layout (location = 0) out vec2 uTexCoord;
void main() 
{
	//const array of positions for the triangle
	const vec3 positions =  vPosition.xyz;
	gl_Position = vec4(positions, 1.0f);
	uTexCoord = vTexCoord;
}

