//
// Created by Cheyne Xie on 26-07-01.
// IBL_EquirectToCube.frag — 把 equirectangular HDR（sampler2D）映射到立方体面
//

#version 430 core

in VS_OUT
{
    vec3 WorldDir;
} FSIn;

out vec4 FragColor;

uniform sampler2D equirectMap;

const float PI = 3.14159265359;

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(clamp(v.y, -1.0, 1.0)));
    uv *= vec2(0.1591, 0.3183); // (1/(2π), 1/π)
    return uv + 0.5;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(FSIn.WorldDir));
    vec3 color = texture(equirectMap, uv).rgb;
    FragColor = vec4(color, 1.0);
}
