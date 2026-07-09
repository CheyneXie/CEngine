//
// Created by Cheyne Xie on 26-06-28.
// Updated by Cheyne Xie on 26-07-01.
//

#version 430 core

in VS_OUT
{
    vec2 UV;
} FSIn;

out vec4 FragColor;

// 帧常量
layout (std140, binding = 0) uniform FrameConstants {
    vec4 CameraPosition;            // 相机位置
    vec4 LightDirection;            // 主方向光方向（从光出发）
    vec4 LightColorAndIntensity;    // RGB主方向光颜色 + A光强度
};

// 点光
struct PointLight {
    vec3 Position;
    float ConstantFactor;
    float LinearFactor;
    float QuadraticFactor;
    vec4 ColorAndIntensity;
};
layout (std430, binding = 1) buffer PointLightBuffer {
    PointLight PointLights[];
};
uniform int PointLightCount;

// Guffer
uniform sampler2D GBuffer_Depth;
uniform sampler2D GBuffer_Position;
uniform sampler2D GBuffer_Normal;
uniform sampler2D GBuffer_AlbedoAO;
uniform sampler2D GBuffer_MetallicRoughness;
uniform sampler2D GBuffer_Emission;

// IBL
uniform samplerCube IrradianceMap;
uniform samplerCube PrefilterMap;
uniform sampler2D BRDFLUT;
uniform float IBLStrengh = 1.0;

// 常量
const float PI = 3.14159265359;
const float MAX_REFLECTION_LOD = 4.0;


float DistributionGGX(vec3 N, vec3 H, float Roughness)
{
    float a = Roughness * Roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denom = NdotH2 * (a2 - 1.0) + 1.0;
    return a2 / max(PI * denom * denom, 0.000001);
}

float GeometrySchlickGGX(float NdotV, float Roughness)
{
    float r = Roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float Roughness)
{
    return GeometrySchlickGGX(max(dot(N, V), 0.0), Roughness)
         * GeometrySchlickGGX(max(dot(N, L), 0.0), Roughness);
}

vec3 FresnelSchlick(float CosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - CosTheta, 0.0, 1.0), 5.0);
}

vec3 FresnelRoughness(float CosTheta, vec3 F0, float Roughness)
{
    return F0 + (max(vec3(1.0 - Roughness), F0) - F0) * pow(clamp(1.0 - CosTheta, 0.0, 1.0), 5.0);
}

void main()
{
    float depth = texture(GBuffer_Depth, FSIn.UV).r;
    if (depth >= 1.0) discard;

    vec3 WorldPos = texture(GBuffer_Position, FSIn.UV).rgb;
    vec3 N = normalize(texture(GBuffer_Normal, FSIn.UV).rgb);
    vec4 albedoAO = texture(GBuffer_AlbedoAO, FSIn.UV);
    vec3 Albedo = albedoAO.rgb;
    float AO = albedoAO.a;
    vec2 mr = texture(GBuffer_MetallicRoughness, FSIn.UV).rg;
    float MetallicValue = mr.r;
    float RoughnessValue = clamp(mr.g, 0.045, 1.0);
    vec3 Emissive = texture(GBuffer_Emission, FSIn.UV).rgb;

    vec3 V = normalize(CameraPosition.xyz - WorldPos);

    vec3 F0 = mix(vec3(0.04), Albedo, MetallicValue);

    // 主方向光
    vec3 Lo = vec3(0.0);
    {
        vec3 L = normalize(-LightDirection.xyz);
        vec3 H = normalize(V + L);

        float NDF = DistributionGGX(N, H, RoughnessValue);
        float G = GeometrySmith(N, V, L, RoughnessValue);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 Specular = NDF * G * F / max(4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0), 0.0001);

        vec3 kS = F;
        vec3 kD = (vec3(1.0) - kS) * (1.0 - MetallicValue);

        float NdotL = max(dot(N, L), 0.0);
        vec3 Radiance = LightColorAndIntensity.rgb * LightColorAndIntensity.a;
        Lo += (kD * Albedo / PI + Specular) * Radiance * NdotL;
    }

    // 点光
    for (int i = 0; i < PointLightCount; i++)
    {
        vec3 toLight = PointLights[i].Position - WorldPos;
        float dist = length(toLight);
        vec3 L = toLight / max(dist, 0.0001);
        vec3 H = normalize(V + L);

        float att = 1.0 / (PointLights[i].ConstantFactor
                         + PointLights[i].LinearFactor * dist
                         + PointLights[i].QuadraticFactor * dist * dist);
        if (att <= 0.0) continue;

        float NDF = DistributionGGX(N, H, RoughnessValue);
        float G = GeometrySmith(N, V, L, RoughnessValue);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 Specular = NDF * G * F / max(4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0), 0.0001);

        vec3 kS = F;
        vec3 kD = (vec3(1.0) - kS) * (1.0 - MetallicValue);

        float NdotL = max(dot(N, L), 0.0);
        vec3 Radiance = PointLights[i].ColorAndIntensity.rgb * PointLights[i].ColorAndIntensity.a * att;
        Lo += (kD * Albedo / PI + Specular) * Radiance * NdotL;
    }

    // 环境光（split-sum）
    vec3 R = reflect(-V, N);
    vec3 F = FresnelRoughness(max(dot(N, V), 0.0), F0, RoughnessValue);
    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - MetallicValue);

    vec3 irradiance = texture(IrradianceMap, N).rgb * IBLStrengh;
    vec3 diffuseIBL = irradiance * Albedo;

    vec3 prefilteredColor = textureLod(PrefilterMap, R, RoughnessValue * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(BRDFLUT, vec2(max(dot(N, V), 0.0), RoughnessValue)).rg;
    vec3 specularIBL = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 Ambient = (kD * diffuseIBL + specularIBL) * AO;

    // 输出
    vec3 Color = Ambient + Lo + Emissive;
    FragColor = vec4(Color, 1.0);
}
