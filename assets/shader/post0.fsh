#version 460 core

in vec2 uv;

layout(location=0) out vec3 fragColor;

uniform sampler2D screenTex;

void main()
{
	//discard;
	fragColor = texture(screenTex, uv).rgb;
	//	(
	//	texture(pastTex, uv).rgb+
	//	texture(nowTex, uv).rgb
	//	) * 0.5;
}