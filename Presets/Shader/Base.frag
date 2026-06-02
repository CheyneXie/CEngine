//
// Created by chaim on 24-09-30.
//

#version 430 core

out vec4 Color;

uniform vec4 mColor = vec4(1.);

void main() {
    Color = mColor;
}