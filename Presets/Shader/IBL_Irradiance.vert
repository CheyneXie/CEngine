//
// Created by Cheyne Xie on 26-07-01.
//

#version 430 core

layout(location = 0) in vec3 InPosition;

layout(location = 0) uniform mat4 Model;
layout(location = 1) uniform mat4 View;
layout(location = 2) uniform mat4 Projection;

out VS_OUT
{
    vec3 WorldDir; // 传出采用方向
} VSOut;

void main()
{
    VSOut.WorldDir = InPosition;
    gl_Position = Projection * View * vec4(InPosition, 1.0);
}
