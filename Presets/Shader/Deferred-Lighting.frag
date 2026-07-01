//
// Created by Cheyne Xie on 26-06-28.
// Updated by Cheyne Xie on 26-06-28.
//

#version 430 core

in VS_OUT
{
    vec2 UV;
} FSIn;

out vec4 FragColor;

layout (std140, binding = 0) uniform FrameConstants {
    vec4 CameraPosition;            // 相机位置
    vec4 LightDirection;            // 主方向光方向
    vec4 LightColorAndIntensity;    // 主方向光颜色 + 光强度
};

struct PointLight {
    vec3 Position;
    float ConstantFactor;
    float LinearFactor;
    float QuadraticFactor;
};

layout (std430, binding = 1) buffer PointLightBuffer {
    PointLight PointLights[];
};

uniform sampler2D GBuffer_Depth;
uniform sampler2D GBuffer_Position;
uniform sampler2D GBuffer_Normal;
uniform sampler2D GBuffer_AlbedoAO;
uniform sampler2D GBuffer_MetallicRoughness;
uniform sampler2D GBuffer_Emission;

void main()
{
    // TODO
}