//
// Created by Cheyne Xie on 26-07-01.
//

#version 430 core

in VS_OUT
{
    vec2 UV;
} FSIn;

out vec4 FragColor;

uniform sampler2D SceneColor;

// ACES Filmic（Narkowicz 近似）
vec3 ACESFilm(vec3 x)
{
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main()
{
    vec3 hdr = texture(SceneColor, FSIn.UV).rgb;

    // ACES 色调映射
    vec3 ldr = ACESFilm(hdr);

    //  gamma
    vec3 mapped = pow(ldr, vec3(1.0 / 2.2));

    FragColor = vec4(mapped, 1.0);
}
