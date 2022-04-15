#version 460 core

layout(location=0) in vec2 vertPos;

out vec2 pos;

void main()
{
	pos = vertPos;
	gl_Position = vec4(vertPos,0.5,1.0);
}