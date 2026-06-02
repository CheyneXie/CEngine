//
// Created by chaim on 24-09-30.
//

#version 430 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 TexCoord;
layout (location = 0) uniform mat4 Transform;

void main() {
    gl_Position = Transform * vec4(Position, 1.0);
}