//
// Created by Cheyne Xie on 24-10-14.
// Updated by Cheyne Xie on 26-06-05.
//

#version 430 core

in VS_OUT
{
    vec3 WorldPosition;
    vec2 UV;
    mat3 TBN;
} FSIn;

out vec4 FragColor;

uniform sampler2D Tex_BaseColor;
uniform sampler2D Tex_Emissive;
uniform sampler2D Tex_Height;
uniform sampler2D Tex_Normals;
uniform sampler2D Tex_Opacity;
uniform sampler2D Tex_Metalness;
uniform sampler2D Tex_Roughness;
uniform sampler2D Tex_AmbientOcclusion;

uniform vec3 CameraPosition;

uniform vec3 LightDirection = vec3(0.5, -1.0, 0.5);
uniform vec3 LightColor = vec3(1.0);
uniform float LightIntensity = 5.0;

layout (std140) uniform Material_Parameters
{
    float Emissive_Intensity;
    float Metallic;
    float Roughness;
    float Opacity;

    vec4 Diffuse_Color;
    vec4 Emission_Color;
};

const float PI = 3.14159265359;

// ------------------------------------------------------------
// GGX Distribution
// ------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float Roughness)
{
    float a = Roughness * Roughness;
    float a2 = a * a;

    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = NdotH2 * (a2 - 1.0) + 1.0;

    return a2 / max(PI * denom * denom, 0.000001);
}

// ------------------------------------------------------------
// Geometry
// ------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float Roughness)
{
    float r = Roughness + 1.0;
    float k = (r * r) / 8.0;

    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float Roughness)
{
    float ggx1 = GeometrySchlickGGX(max(dot(N, V), 0.0), Roughness);
    float ggx2 = GeometrySchlickGGX(max(dot(N, L), 0.0), Roughness);
    return ggx1 * ggx2;
}

// ------------------------------------------------------------
// Fresnel
// ------------------------------------------------------------
vec3 FresnelSchlick(float CosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - CosTheta, 0.0, 1.0), 5.0);
}

// ------------------------------------------------------------
// Normal Mapping
// ------------------------------------------------------------
vec3 GetNormal()
{
    vec3 NormalTex = texture(Tex_Normals, FSIn.UV).xyz;
    NormalTex = NormalTex * 2.0 - 1.0;
    return normalize(FSIn.TBN * NormalTex);
}

// ------------------------------------------------------------
// Main
// ------------------------------------------------------------
void main()
{
    vec3 Albedo = texture(Tex_BaseColor, FSIn.UV).rgb;

    // FragColor = vec4(Albedo, 1.0);

    // return;

    // sRGB -> Linear
    Albedo = pow(Albedo, vec3(2.2));

    Albedo *= Diffuse_Color.rgb;

    float MetallicValue = texture(Tex_Metalness, FSIn.UV).r * Metallic;

    float RoughnessValue = texture(Tex_Roughness, FSIn.UV).r * Roughness;

    RoughnessValue = clamp(RoughnessValue, 0.045, 1.0);

    float AO = texture(Tex_AmbientOcclusion, FSIn.UV).r;

    float Alpha = texture(Tex_Opacity, FSIn.UV).r * Opacity;

    vec3 Emissive = texture(Tex_Emissive, FSIn.UV).rgb * Emission_Color.rgb * Emissive_Intensity;

    vec3 N = GetNormal();

    vec3 V = normalize(CameraPosition - FSIn.WorldPosition);

    // LightDirection = 光照射方向（UE风格）
    vec3 L = normalize(-LightDirection);

    vec3 H = normalize(V + L);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, Albedo, MetallicValue);

    float NDF = DistributionGGX(N, H, RoughnessValue);

    float G = GeometrySmith(N, V, L, RoughnessValue);

    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 Numerator = NDF * G * F;

    float Denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);

    vec3 Specular = Numerator / max(Denominator, 0.0001);

    vec3 kS = F;

    vec3 kD = vec3(1.0) - kS;

    kD *= 1.0 - MetallicValue;

    float NdotL = max(dot(N, L), 0.0);

    vec3 Radiance = LightColor * LightIntensity;

    vec3 DirectLighting = (kD * Albedo / PI + Specular) * Radiance * NdotL;

    vec3 Ambient = Albedo * AO * 0.03;

    vec3 Color = Ambient + DirectLighting + Emissive;

    // Reinhard Tonemap
    Color = Color / (Color + vec3(1.0));

    // Linear -> sRGB
    Color = pow(Color, vec3(1.0 / 2.2));

    FragColor = vec4(Color, Alpha);
}