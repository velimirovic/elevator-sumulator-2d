#version 330 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTex;

out vec2 chTex;

uniform vec2 uPos;
uniform vec2 uScale;
uniform bool uFlipX;  // Nova uniforma za flipovanje

void main()
{
    vec2 texCoord = inTex;
    if (uFlipX) {
        texCoord.x = 1.0 - texCoord.x;  // Obrni X koordinatu
    }
    
    vec2 scaledPos = inPos * uScale;
    gl_Position = vec4(scaledPos + uPos, 0.0, 1.0);
    chTex = texCoord;
}