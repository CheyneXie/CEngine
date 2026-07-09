//
// Created by Cheyne Xie on 26-07-01.
// IBL_Irradiance.frag — 卷积环境立方图得到漫反射辐照度
//

#version 430 core

in VS_OUT
{
    vec3 WorldDir;
} FSIn;

out vec4 FragColor;

uniform samplerCube environmentMap;

const float PI = 3.14159265359;

void main()
{
    vec3 normal = normalize(FSIn.WorldDir);

    vec3 irradiance = vec3(0.0);

    // 构造切线空间
    vec3 up = abs(normal.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(0.0, 0.0, 1.0);
    vec3 right = normalize(cross(up, normal));
    up = cross(normal, right);

    float sampleDelta = 0.025;
    float nrSamples = 0.0;
    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // 球面坐标 -> 笛卡尔（切线空间）
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            // 转到世界空间
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

            irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));

    FragColor = vec4(irradiance, 1.0);
}
