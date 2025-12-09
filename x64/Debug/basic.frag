#version 330 core

in vec2 chTex;
out vec4 outCol;

uniform sampler2D uTex;
uniform vec4 uColor;
uniform bool uUseTexture;

void main()
{
    if (uUseTexture) {
        outCol = texture(uTex, chTex) * uColor;
    } else {
        outCol = uColor;
    }
}