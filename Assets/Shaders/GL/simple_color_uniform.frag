#version 330 core

out vec4 color;
uniform vec4 myColor;

void main()
{
	color = myColor;
}