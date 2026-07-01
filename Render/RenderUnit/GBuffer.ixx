/**
 * @file GBuffer.ixx
 * @author Cheyne Xie
 * @brief GBuffer
 * @version 0.1
 * @date 2026-06-21
 * 
 */

module;
#include <glad/glad.h>
export module CEngine.RenderUnit:GBuffer;
import CEngine.Base;
import CEngine.Render;
import CEngine.EventBus;
import CEngine.Logger;
import std;

#define $SetTextureParameter() \
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); \
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); \
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); \
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)

namespace CEngine::RenderUnit {
    export class GBuffer : public Object {
    public:
        static GBuffer* Get() {
            static std::unique_ptr<GBuffer> instance = std::unique_ptr<GBuffer>(new GBuffer());
            return instance.get();
        }

        static void Init(void *window, int width, int height) {
            // 需要把该函数注册到 FramebufferSize 更改事件
            LogD("GBuffer") << "GBuffer 初始化";
            auto gb = Get();
            if (gb->Initialized) {
                // 删除重新创建
                delete gb;
                gb = Get();
            }
            auto& gBuffer = *gb;
            gBuffer.Width = width;
            gBuffer.Height = height;
            // FBO
            glGenFramebuffers(1, &gBuffer.FBO);
            glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.FBO);

            // 深度
            glGenTextures(1, &gBuffer.DepthTexture);
            glBindTexture(GL_TEXTURE_2D, gBuffer.DepthTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            $SetTextureParameter();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gBuffer.DepthTexture, 0);
            // 创建并绑定 Depth RBO
            // glGenRenderbuffers(1, &gBuffer.DepthRBO);
            // glBindRenderbuffer(GL_RENDERBUFFER, gBuffer.DepthRBO);
            // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
            // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gBuffer.DepthRBO);

            // 位置
            glGenTextures(1, &gBuffer.PositionTexture);
            glBindTexture(GL_TEXTURE_2D, gBuffer.PositionTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
            $SetTextureParameter();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBuffer.PositionTexture, 0);

            // 法线
            glGenTextures(1, &gBuffer.NormalTexture);
            glBindTexture(GL_TEXTURE_2D, gBuffer.NormalTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
            $SetTextureParameter();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gBuffer.NormalTexture, 0);

            // 基础色 + AO
            glGenTextures(1, &gBuffer.AlbedoAOTexture);
            glBindTexture(GL_TEXTURE_2D, gBuffer.AlbedoAOTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
            $SetTextureParameter();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gBuffer.AlbedoAOTexture, 0);

            // 金属度 + 粗糙度
            glGenTextures(1, &gBuffer.MetallicRoughnessTexture);
            glBindTexture(GL_TEXTURE_2D, gBuffer.MetallicRoughnessTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, width, height, 0, GL_RG, GL_FLOAT, nullptr);
            $SetTextureParameter();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gBuffer.MetallicRoughnessTexture, 0);

            // 自发光
            glGenTextures(1, &gBuffer.EmissionTexture);
            glBindTexture(GL_TEXTURE_2D, gBuffer.EmissionTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
            $SetTextureParameter();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gBuffer.EmissionTexture, 0);

            // attachments
            GLuint attachments[5] = { 
                GL_COLOR_ATTACHMENT0, 
                GL_COLOR_ATTACHMENT1, 
                GL_COLOR_ATTACHMENT2,
                GL_COLOR_ATTACHMENT3,
                GL_COLOR_ATTACHMENT4,
            };
            glDrawBuffers(5, attachments);

            // 检查 FBO 完整性
            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE) {
                LogE("GBuffer") << "FBO错误: 代码 = " << status;
            }

            // FBO Reset
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void Bind() {
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        void Unbind() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void Use(ShaderProgram *sp) {
            sp->SetUniform("GBuffer_Depth", Texture::_Use(DepthTexture));
            sp->SetUniform("GBuffer_Position", Texture::_Use(PositionTexture));
            sp->SetUniform("GBuffer_Normal", Texture::_Use(NormalTexture));
            sp->SetUniform("GBuffer_AlbedoAO", Texture::_Use(AlbedoAOTexture));
            sp->SetUniform("GBuffer_MetallicRoughness", Texture::_Use(MetallicRoughnessTexture));
            sp->SetUniform("GBuffer_Emission", Texture::_Use(EmissionTexture));
        }

        std::array<std::pair<const char*, unsigned int>, 6> MakeInfoForUI() {
            return {{
                {"Depth", DepthTexture},
                {"Position", PositionTexture},
                {"Normal", NormalTexture},
                {"Albedo & AO", AlbedoAOTexture},
                {"Metallic & Roughness", MetallicRoughnessTexture},
                {"Emission", EmissionTexture}
            }};
        }

        std::pair<int, int> GetSize() {
            return {Width, Height};
        }

        GBuffer(const GBuffer &) = delete;
        GBuffer &operator=(GBuffer &tex) = delete;
        ~GBuffer() {
            glDeleteTextures(1, &DepthTexture);
            glDeleteTextures(1, &PositionTexture);
            glDeleteTextures(1, &NormalTexture);
            glDeleteTextures(1, &AlbedoAOTexture);
            glDeleteTextures(1, &MetallicRoughnessTexture);
            glDeleteTextures(1, &EmissionTexture);
        }

    private:
        GBuffer() = default;

        bool Initialized = false;

        int Width, Height;

        unsigned int FBO = 0;

        unsigned int DepthTexture = 0;

        unsigned int PositionTexture = 0;

        unsigned int NormalTexture = 0;

        unsigned int AlbedoAOTexture = 0;

        unsigned int MetallicRoughnessTexture = 0;

        unsigned int EmissionTexture = 0;
    };
    export inline auto& GBuffer() {
        return *GBuffer::Get();
    }
}