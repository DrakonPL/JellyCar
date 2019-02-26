#version 330 core
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform vec4 colors;

out vec4 color;

void main()
{
    color = colors * texture(ourTexture, TexCoord);
}