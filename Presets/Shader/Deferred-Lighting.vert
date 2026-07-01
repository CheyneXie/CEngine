//
// Created by Cheyne Xie on 26-06-28.
// Updated by Cheyne Xie on 26-06-28.
//

#version 430 core

out VS_OUT
{
    vec2 UV;
} VSOut;

void main() {
    float x = (gl_VertexID == 2) ? 3.0 : -1.0;
    float y = (gl_VertexID == 1) ? 3.0 : -1.0;
    
    gl_Position = vec4(x, y, 0.0, 1.0);
    VSOut.UV = vec2(x, y) * 0.5 + 0.5;
}