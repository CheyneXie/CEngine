/**
 * @file SceneFBO.ixx
 * @author Cheyne Xie
 * @brief 场景颜色帧缓冲
 * @version 1.0
 * @date 2026-07-01
 *
 */

module;
#include <glad/glad.h>
export module CEngine.RenderUnit:SceneFBO;
import CEngine.Base;
import CEngine.Render;
import CEngine.EventBus;
import CEngine.Logger;
import std;

#define $SetSceneTextureParameter() \
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); \
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); \
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); \
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)

namespace CEngine::RenderUnit {
    export class SceneFBO : public Object {
    public:
        static SceneFBO* Get() {
            static std::unique_ptr<SceneFBO> instance = std::unique_ptr<SceneFBO>(new SceneFBO());
            return instance.get();
        }

        static void Init(void *window, int width, int height) {
            LogD("SceneFBO") << "SceneFBO 初始化";
            auto sf = Get();
            if (sf->Initialized) {
                delete sf;
                sf = Get();
            }
            auto& scene = *sf;
            scene.Width = width;
            scene.Height = height;
            // FBO
            glGenFramebuffers(1, &scene.FBO);
            glBindFramebuffer(GL_FRAMEBUFFER, scene.FBO);

            // HDR 颜色
            glGenTextures(1, &scene.ColorTexture);
            glBindTexture(GL_TEXTURE_2D, scene.ColorTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
            $SetSceneTextureParameter();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, scene.ColorTexture, 0);

            // 深度
            glGenTextures(1, &scene.DepthTexture);
            glBindTexture(GL_TEXTURE_2D, scene.DepthTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, scene.DepthTexture, 0);

            // 颜色附件
            GLuint attachments[1] = { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers(1, attachments);

            // 检查 FBO 完整性
            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE) {
                LogE("SceneFBO") << "FBO错误: 代码 = " << status;
            }

            // FBO Rest
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            scene.Initialized = true;
        }

        /**
         * @brief 绑定 FBO 并清空颜色
         */
        void Bind() {
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        /**
         * @brief 绑定但不情况（延迟渲染需要保留前向渲染的结果）
         */
        void BindNoClear() {
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        }

        /**
         * @brief 解绑 FBO
         */
        void Unbind() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        /**
         * @brief 使用场景颜色纹理
         * @param sp 着色器程序
         */
        void Use(ShaderProgram *sp) {
            sp->SetUniform("SceneColor", Texture::_Use(ColorTexture));
        }

        std::pair<int, int> GetSize() {
            return {Width, Height};
        }

        /// @property FBO
        unsigned int getFBO() const { return FBO; }
        /// @property ColorTexture
        unsigned int getColorTexture() const { return ColorTexture; }
        /// @property DepthTexture
        unsigned int getDepthTexture() const { return DepthTexture; }

        SceneFBO(const SceneFBO &) = delete;
        SceneFBO &operator=(SceneFBO &) = delete;
        ~SceneFBO() {
            if (ColorTexture) glDeleteTextures(1, &ColorTexture);
            if (DepthTexture) glDeleteTextures(1, &DepthTexture);
        }

    private:
        SceneFBO() = default;

        bool Initialized = false;

        int Width = 0, Height = 0;

        unsigned int FBO = 0;
        unsigned int ColorTexture = 0;
        unsigned int DepthTexture = 0;
    };

    export inline auto& SceneFBO() {
        return *SceneFBO::Get();
    }
}
