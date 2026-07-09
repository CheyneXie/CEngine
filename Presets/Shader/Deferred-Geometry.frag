//
// Created by Cheyne Xie on 26-06-23.
// Updated by Cheyne Xie on 26-06-23.
//

#version 430 core

in VS_OUT
{
    vec3 WorldPosition;
    vec2 UV;
    mat3 TBN;
} FSIn;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoAO;
layout (location = 3) out vec2 gMetallicRoughness;
layout (location = 4) out vec3 gEmission;


uniform sampler2D Tex_BaseColor;
uniform sampler2D Tex_Emissive;
uniform sampler2D Tex_Height;
uniform sampler2D Tex_Normals;
uniform sampler2D Tex_Opacity;
uniform sampler2D Tex_Metalness;
uniform sampler2D Tex_Roughness;
uniform sampler2D Tex_AmbientOcclusion;

layout (std140, binding = 2) uniform Material_Parameters
{
    float Emissive_Intensity;
    float Metallic;
    float Roughness;
    float Opacity;

    vec4 Diffuse_Color;
    vec4 Emission_Color;
};

vec3 GetNormal()
{
    vec3 NormalTex = texture(Tex_Normals, FSIn.UV).xyz;
    NormalTex = NormalTex * 2.0 - 1.0;
    return normalize(FSIn.TBN * NormalTex);
}


void main()
{
    gPosition = FSIn.WorldPosition;
    gNormal = GetNormal();
    gAlbedoAO = clamp(vec4(texture(Tex_BaseColor, FSIn.UV).xyz * Diffuse_Color.xyz, texture(Tex_AmbientOcclusion, FSIn.UV).x), 0, 1);
    gMetallicRoughness = clamp(vec2(texture(Tex_Metalness, FSIn.UV).x * Metallic, texture(Tex_Roughness, FSIn.UV).x * Roughness), 0, 1);
    gEmission = texture(Tex_Emissive, FSIn.UV).xyz * Emission_Color.xyz * Emissive_Intensity;
}