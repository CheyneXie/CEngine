//
// Created by Cheyne Xie on 24-10-14.
// Updated by Cheyne Xie on 26-06-05.
//

#version 430 core

layout(location = 0) in vec3 InPosition;
layout(location = 1) in vec3 InNormal;
layout(location = 2) in vec4 InTangent;
layout(location = 3) in vec2 InUV;

layout(location = 0) uniform mat4 Model;
layout(location = 1) uniform mat4 View;
layout(location = 2) uniform mat4 Projection;

out VS_OUT
{
    vec3 WorldPosition;
    vec2 UV;
    mat3 TBN;
} VSOut;

void main()
{
    vec3 T = normalize(mat3(Model) * InTangent.xyz);
    vec3 N = normalize(mat3(Model) * InNormal);
    vec3 B = normalize(cross(N, T) * InTangent.w);

    VSOut.TBN = mat3(T, B, N);

    vec4 WorldPosition = Model * vec4(InPosition, 1.0);

    VSOut.WorldPosition = WorldPosition.xyz;
    VSOut.UV = InUV;

    gl_Position = Projection * View * WorldPosition;
}