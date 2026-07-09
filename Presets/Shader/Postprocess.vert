//
// Created by Cheyne Xie on 26-07-01.
//

#version 430 core

out VS_OUT
{
    vec2 UV;
} VSOut;

void main()
{
    // CCW 左下→右下→左上
    float x = (gl_VertexID == 1) ? 3.0 : -1.0;
    float y = (gl_VertexID == 2) ? 3.0 : -1.0;

    gl_Position = vec4(x, y, 0.0, 1.0);
    VSOut.UV = vec2(x, y) * 0.5 + 0.5;
}
