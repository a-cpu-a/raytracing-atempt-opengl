#version 460 core

layout(location=0) in vec2 vertPos;

out vec2 uv;

void main()
{
	uv = vertPos*0.5+0.5;
	gl_Position = vec4(vertPos,0.5,1.0);
}