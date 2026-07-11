//
// Created by Cheyne Xie on 24-10-14.
// Updated by Cheyne Xie on 26-07-01.
//

#version 430 core

in VS_OUT
{
    vec3 WorldPosition;
    vec2 UV;
    mat3 TBN;
} FSIn;

out vec4 FragColor;

// 帧常量
layout (std140, binding = 0) uniform FrameConstants {
    vec4 CameraPosition;            // 相机位置
    vec4 LightDirection;            // 主方向光方向（从光出发）
    vec4 LightColorAndIntensity;    // RGB主方向光颜色 + A光强度
    float DiffuseIBLStrength;       // IBL 强度
    float SpecularIBLStrength;      // IBL 强度
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

// 材质参数
layout (std140, binding = 2) uniform Material_Parameters
{
    float Emissive_Intensity;
    float Metallic;
    float Roughness;
    float Opacity;
    vec4 Diffuse_Color;
    vec4 Emission_Color;
};

// 材质贴图
uniform sampler2D Tex_BaseColor;
uniform sampler2D Tex_Emissive;
uniform sampler2D Tex_Height;
uniform sampler2D Tex_Normals;
uniform sampler2D Tex_Opacity;
uniform sampler2D Tex_Metalness;
uniform sampler2D Tex_Roughness;
uniform sampler2D Tex_AmbientOcclusion;

// IBL
uniform samplerCube IrradianceMap;
uniform samplerCube PrefilterMap;
uniform sampler2D BRDFLUT;

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
    float ggx1 = GeometrySchlickGGX(max(dot(N, V), 0.0), Roughness);
    float ggx2 = GeometrySchlickGGX(max(dot(N, L), 0.0), Roughness);
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float CosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - CosTheta, 0.0, 1.0), 5.0);
}

vec3 FresnelRoughness(float CosTheta, vec3 F0, float Roughness)
{
    return F0 + (max(vec3(1.0 - Roughness), F0) - F0) * pow(clamp(1.0 - CosTheta, 0.0, 1.0), 5.0);
}

vec3 GetNormal()
{
    vec3 NormalTex = texture(Tex_Normals, FSIn.UV).xyz;
    NormalTex = NormalTex * 2.0 - 1.0;
    return normalize(FSIn.TBN * NormalTex);
}

void main()
{
    vec3 Albedo = texture(Tex_BaseColor, FSIn.UV).rgb;
    // gamma
    Albedo = pow(Albedo, vec3(2.2));
    Albedo *= Diffuse_Color.rgb;

    float MetallicValue = texture(Tex_Metalness, FSIn.UV).r * Metallic;
    float RoughnessValue = texture(Tex_Roughness, FSIn.UV).r * Roughness;
    RoughnessValue = clamp(RoughnessValue, 0.045, 1.0);
    float AO = texture(Tex_AmbientOcclusion, FSIn.UV).r;
    float Alpha = texture(Tex_Opacity, FSIn.UV).r * Opacity;
    vec3 Emissive = texture(Tex_Emissive, FSIn.UV).rgb * Emission_Color.rgb * Emissive_Intensity;

    vec3 N = GetNormal();
    vec3 V = normalize(CameraPosition.xyz - FSIn.WorldPosition);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, Albedo, MetallicValue);

    vec3 Lo = vec3(0.0);

    // 主方向光
    {
        vec3 L = normalize(-LightDirection.xyz);
        vec3 H = normalize(V + L);

        float NDF = DistributionGGX(N, H, RoughnessValue);
        float G = GeometrySmith(N, V, L, RoughnessValue);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 Numerator = NDF * G * F;
        float Denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 Specular = Numerator / max(Denominator, 0.0001);

        vec3 kS = F;
        vec3 kD = (vec3(1.0) - kS) * (1.0 - MetallicValue);

        float NdotL = max(dot(N, L), 0.0);
        vec3 Radiance = LightColorAndIntensity.rgb * LightColorAndIntensity.a;
        Lo += (kD * Albedo / PI + Specular) * Radiance * NdotL;
    }

    // 点光
    for (int i = 0; i < PointLightCount; i++)
    {
        vec3 toLight = PointLights[i].Position - FSIn.WorldPosition;
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

        vec3 Numerator = NDF * G * F;
        float Denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 Specular = Numerator / max(Denominator, 0.0001);

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

    vec3 irradiance = texture(IrradianceMap, N).rgb;
    vec3 diffuseIBL = irradiance * Albedo * DiffuseIBLStrength;

    vec3 prefilteredColor = textureLod(PrefilterMap, R, RoughnessValue * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(BRDFLUT, vec2(max(dot(N, V), 0.0), RoughnessValue)).rg;
    vec3 specularIBL = prefilteredColor * (F * brdf.x + brdf.y) * SpecularIBLStrength;

    vec3 Ambient = (kD * diffuseIBL + specularIBL) * AO;

    // 输出
    vec3 Color = Ambient + Lo + Emissive;
    FragColor = vec4(Color, Alpha);
}
