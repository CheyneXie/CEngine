//
// Created by Cheyne Xie on 26-07-01.
//

#version 430 core

in VS_OUT
{
    vec3 WorldDir;
} FSIn;

out vec4 FragColor;

uniform samplerCube environmentMap;

void main()
{
    vec3 color = texture(environmentMap, normalize(FSIn.WorldDir)).rgb;
    FragColor = vec4(color, 1.0);
}
