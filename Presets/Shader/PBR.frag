//
// Created by chaim on 24-10-14.
//

#version 430

in vec3 Vertex_Position;
in vec3 Vertex_Normal;
in vec2 UV;

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

out vec4 FragColor;

void main() {
    FragColor = texture(Tex_Diffuse, UV);
}