//
// Created by chaim on 24-10-14.
//

#version 430

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;

layout (location = 0) uniform mat4 Transform;

uniform sampler2D Tex_Diffuse;
uniform sampler2D Tex_Specular;
uniform sampler2D Tex_Ambient;
uniform sampler2D Tex_Emissive;
uniform sampler2D Tex_Height;
uniform sampler2D Tex_Normals;
uniform sampler2D Tex_Shininess;
uniform sampler2D Tex_Opacity;
uniform sampler2D Tex_Displacement;
uniform sampler2D Tex_Lightmap;
uniform sampler2D Tex_Reflection;
uniform sampler2D Tex_BaseColor;
uniform sampler2D Tex_NormalCamera;
uniform sampler2D Tex_EmissionColor;
uniform sampler2D Tex_Metalness;
uniform sampler2D Tex_DiffuseRoughness;
uniform sampler2D Tex_AmbientOcclusion;
uniform sampler2D Tex_Sheen;
uniform sampler2D Tex_Clearcoat;
uniform sampler2D Tex_Transmission;

layout (std140) uniform Material_Parameters
{
    float Emissive_Intensity;
    float Metallic;
    float Roughness;
    float Opacity;
    vec4 Diffuse_Color;
    vec4 Specular_Color;
    vec4 Emission_Color;
    vec4 Reflective_Color;
    vec4 Transparent_Color;
};

out vec3 Vertex_Position;
out vec3 Vertex_Normal;
out vec2 UV;

void main() {
    gl_Position = Transform * vec4(Position, 1.0);
    Vertex_Position = Position;
    Vertex_Normal = Normal;
    UV = TexCoord;
}