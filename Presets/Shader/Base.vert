//
// Created by Cheyne Xie on 24-09-30.
// Created by Cheyne Xie on 26-06-05.
//

#version 430 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 TexCoord;

layout(location = 0) uniform mat4 Model;
layout(location = 1) uniform mat4 View;
layout(location = 2) uniform mat4 Projection;

void main() {
    vec4 WorldPosition = Model * vec4(Position, 1.0);
    gl_Position = Projection * View * WorldPosition;
}