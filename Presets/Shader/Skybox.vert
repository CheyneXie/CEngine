//
// Created by Cheyne Xie on 26-07-01.
//

#version 430 core

layout(location = 0) in vec3 InPosition;

layout(location = 1) uniform mat4 View; // 传入时需已去掉平移
layout(location = 2) uniform mat4 Projection;

out VS_OUT
{
    vec3 WorldDir; // 传出采用方向
} VSOut;

void main()
{
    VSOut.WorldDir = InPosition;
    vec4 pos = Projection * View * vec4(InPosition, 1.0);
    gl_Position = pos.xyww; // z = w -> NDC z = 1.0（远平面）
}
