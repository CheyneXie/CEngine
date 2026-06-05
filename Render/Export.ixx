module;
#include <assimp/material.h>
export module CEngine.Render;
export import :GLSL;
export import :ShaderProgram;
export import :ShaderManager;
export import :Mesh;
export import :Material;
export import :Texture;
export import :ShaderUniformVar;
export import :Camera;

namespace CEngine {
    // @formatter:off
    export const char *aiTextureTypeToString(const aiTextureType &type) {
        switch (type) {
            case aiTextureType_DIFFUSE:          return "Diffuse";
            case aiTextureType_SPECULAR:         return "Specular";
            case aiTextureType_AMBIENT:          return "Ambient";
            case aiTextureType_EMISSIVE:         return "Emissive";
            case aiTextureType_HEIGHT:           return "Height";
            case aiTextureType_NORMALS:          return "Normals";
            case aiTextureType_SHININESS:        return "Shininess";
            case aiTextureType_OPACITY:          return "Opacity";
            case aiTextureType_DISPLACEMENT:     return "Displacement";
            case aiTextureType_LIGHTMAP:         return "Lightmap";
            case aiTextureType_REFLECTION:       return "Reflection";
            case aiTextureType_BASE_COLOR:       return "Base Color";
            case aiTextureType_NORMAL_CAMERA:    return "Normal Camera";
            case aiTextureType_EMISSION_COLOR:   return "Emission Color";
            case aiTextureType_METALNESS:        return "Metalness";
            case aiTextureType_DIFFUSE_ROUGHNESS:return "Diffuse Roughness";
            case aiTextureType_AMBIENT_OCCLUSION:return "Ambient Occlusion";
            case aiTextureType_SHEEN:            return "Sheen";
            case aiTextureType_CLEARCOAT:        return "Clearcoat";
            case aiTextureType_TRANSMISSION:     return "Transmission";
            default:                             return "Unknown";
        }
    }
    // @formatter:on
}
