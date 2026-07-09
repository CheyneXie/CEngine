/**
 * @file IBL.ixx
 * @author Cheyne Xie
 * @brief Image-Based Lighting（基于图像的光照）
 * @version 1.0
 * @date 2026-06-28
 *
 * @remark 生成 irradiance 立方图、prefilter 立方图（GGX）、BRDF LUT（2D）。
 */

module;
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ThirdParty/Environments/mirrored_hall_1k.hdr.h"
export module CEngine.Render:IBL;
import CEngine.Base;
import CEngine.Image;
import :Texture;
import :ShaderProgram;
import :Mesh;
import CEngine.Utils.RenderUtils;
import CEngine.Utils;
import CEngine.Logger;
import std;

namespace CEngine {
    namespace {
        // IBL 三张纹理固定占用的纹理槽
        constexpr int IBL_IRRADIANCE_SLOT = 12;
        constexpr int IBL_PREFILTER_SLOT = 13;
        constexpr int IBL_BRDFLUT_SLOT = 14;

        constexpr int IBL_ENV_SIZE = 512;       // envCube 每面尺寸
        constexpr int IBL_IRRADIANCE_SIZE = 32; // irradiance 每面尺寸
        constexpr int IBL_PREFILTER_SIZE = 128; // prefilter 每面尺寸
        constexpr int IBL_PREFILTER_MIPS = 5;   // prefilter mip 层数
        constexpr int IBL_BRDFLUT_SIZE = 512;   // brdfLUT 尺寸
    }

    /**
     * @brief 立方图（不经过 Texture）
     * @remark 不导出，仅供 IBL 内部使用。
     */
    struct Cubemap {
        GLuint id = 0;
        int perFaceSize = 0;
        int mipLevels = 1;

        void Allocate(int perFaceSize_, int mips, GLenum internalFormat) {
            if (id == 0) glGenTextures(1, &id);
            perFaceSize = perFaceSize_;
            mipLevels = mips;
            glBindTexture(GL_TEXTURE_CUBE_MAP, id);
            for (int i = 0; i < 6; i++) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat,
                             perFaceSize, perFaceSize, 0, GL_RGB, GL_FLOAT, nullptr);
            }
            if (mips > 1) {
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
                // 为每层 mip 预分配存储（捕获时按 mip 渲染）
                for (int i = 0; i < 6; i++) {
                    for (int m = 1; m < mips; m++) {
                        int s = perFaceSize >> m;
                        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m, internalFormat,
                                     s, s, 0, GL_RGB, GL_FLOAT, nullptr);
                    }
                }
            } else {
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        }

        void BindToSlot(int slot) const {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_CUBE_MAP, id);
        }

        void Delete() {
            if (id) { glDeleteTextures(1, &id); id = 0; }
        }

        ~Cubemap() { Delete(); }
        Cubemap() = default;
        Cubemap(const Cubemap&) = delete;
        Cubemap& operator=(const Cubemap&) = delete;
    };

    export class IBL final : public Object {
    public:
        static const char *TAG;

        IBL() = default;
        ~IBL() override {
            if (envEquirect) glDeleteTextures(1, &envEquirect);
            if (brdfLUT) glDeleteTextures(1, &brdfLUT);
            if (captureFBO) glDeleteFramebuffers(1, &captureFBO);
        }

        IBL(const IBL&) = delete;
        IBL& operator=(const IBL&) = delete;

        /**
         * @brief 从 HDR equirectangular 文件加载并生成
         * 
         * @param path 文件路径
         */
        bool LoadFromHDRFile(const char *path) {
            if (!Utils::FileExists(path)) {
                LogE(TAG) << "环境贴图不存在: " << path;
                return false;
            }
            auto img = Image::FromFile(path, true);
            if (!img.IsValid()) {
                LogE(TAG) << "环境贴图加载失败: " << path;
                return false;
            }
            UploadEquirect(img);
            LogI(TAG) << "从 HDR 文件加载环境贴图: " << path
                      << " (" << img.GetWidth() << "x" << img.GetHeight() << ")";
            Generate();
            return true;
        }

        /**
         * @brief 加载默认环境
         */
        void LoadDefault() {
            auto img = Image::FromMemory(mirrored_hall_1k_hdr, mirrored_hall_1k_hdr_len, true);
            if (!img.IsValid()) {
                LogE(TAG) << "默认环境贴图加载失败";
                std::exit(-1);
                return;
            }
            UploadEquirect(img);
            Generate();
        }

        /**
         * @brief 惰性确保已生成（Bind 前调用）
         */
        void EnsureGenerated() {
            if (!generated) LoadDefault();
        }

        /**
         * @brief 绑定 IBL 三张纹理到着色器（irradiance/prefilter/brdfLUT）
         * @remark 固定槽 12/13/14，幂等。供 PBR/Deferred-Lighting 批渲染时调用。
         */
        void Bind(ShaderProgram *sp) {
            EnsureGenerated();
            irradianceCube.BindToSlot(IBL_IRRADIANCE_SLOT);
            prefilterCube.BindToSlot(IBL_PREFILTER_SLOT);
            glActiveTexture(GL_TEXTURE0 + IBL_BRDFLUT_SLOT);
            glBindTexture(GL_TEXTURE_2D, brdfLUT);

            sp->SetUniform("IrradianceMap", IBL_IRRADIANCE_SLOT);
            sp->SetUniform("PrefilterMap", IBL_PREFILTER_SLOT);
            sp->SetUniform("BRDFLUT", IBL_BRDFLUT_SLOT);
        }

        /**
         * @brief 绑定环境立方图到槽 0（天空盒用）
         */
        void BindSkybox(ShaderProgram *sp) {
            EnsureGenerated();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, envCube.id);
            sp->SetUniform("environmentMap", 0);
        }

        // 全局活动 IBL（供 PBR/Deferred 取用）
        static IBL *GetActive() { return sActive; }
        void SetActive() { sActive = this; }

        /// @property EnvCubemap
        GLuint getEnvCubemap() const { return envCube.id; }
        /// @property IrradianceCubemap
        GLuint getIrradianceCubemap() const { return irradianceCube.id; }
        /// @property PrefilterCubemap
        GLuint getPrefilterCubemap() const { return prefilterCube.id; }
        /// @property BRDFLUT
        GLuint getBRDFLUT() const { return brdfLUT; }

    private:
        /**
         * @brief 把 equirect Image 上传为 GL_TEXTURE_2D (RGB32F)
         */
        void UploadEquirect(const Image &img) {
            if (envEquirect == 0) glGenTextures(1, &envEquirect);
            glBindTexture(GL_TEXTURE_2D, envEquirect);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F,
                         static_cast<GLsizei>(img.GetWidth()),
                         static_cast<GLsizei>(img.GetHeight()),
                         0, GL_RGB, GL_FLOAT, img.GetBuffer());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        /**
         * @brief 获取 6 个立方体面捕获视图
         * @param i 哪个面
         */
        static glm::mat4 CaptureView(int i) {
            switch (i) {
                case 0: return glm::lookAt(glm::vec3(0), glm::vec3( 1, 0, 0), glm::vec3(0, -1, 0));
                case 1: return glm::lookAt(glm::vec3(0), glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0));
                case 2: return glm::lookAt(glm::vec3(0), glm::vec3(0,  1, 0), glm::vec3(0, 0,  1));
                case 3: return glm::lookAt(glm::vec3(0), glm::vec3(0, -1, 0), glm::vec3(0, 0, -1));
                case 4: return glm::lookAt(glm::vec3(0), glm::vec3(0, 0,  1), glm::vec3(0, -1, 0));
                case 5: return glm::lookAt(glm::vec3(0), glm::vec3(0, 0, -1), glm::vec3(0, -1, 0));
                default: return glm::mat4(1.0f);
            }
        }

        void EnsureCaptureFBO() {
            if (captureFBO == 0) glGenFramebuffers(1, &captureFBO);
        }

        /**
         * @brief 把某层立方体面纹理附件挂到捕获 FBO，并设置视口
         * 
         * @param cubeId 
         * @param face 
         * @param mip 
         * @param perFaceSize 
         */
        void AttachCubeFace(GLuint cubeId, int face, int mip, int perFaceSize) {
            glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
            int s = perFaceSize >> mip;
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, cubeId, mip);
            glViewport(0, 0, s, s);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        void Attach2D(GLuint texId) {
            glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_2D, texId, 0);
            glViewport(0, 0, IBL_BRDFLUT_SIZE, IBL_BRDFLUT_SIZE);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        /**
         * @brief 渲染捕获用的立方体
         */
        void RenderCaptureCube(ShaderProgram *sp, const glm::mat4 &view, const glm::mat4 &proj) {
            sp->SetUniform(1, view);
            sp->SetUniform(2, proj);
            if (auto cube = Mesh::GetEngineMesh("Cube"))
                cube->Render();
        }

        /**
         * @brief 执行 4 个捕获 pass
         */
        void Generate() {
            EnsureCaptureFBO();
            glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
            // 捕获相机在单位 Cube 内部
            glDisable(GL_CULL_FACE);

            const glm::mat4 captureProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

            // —— 1. EquirectToCube ——
            envCube.Allocate(IBL_ENV_SIZE, 1, GL_RGB16F);
            {
                auto *sp = ShaderProgram::Get("IBL_EquirectToCube");
                sp->Use();
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, envEquirect);
                sp->SetUniform("equirectMap", 0);
                for (int i = 0; i < 6; i++) {
                    AttachCubeFace(envCube.id, i, 0, IBL_ENV_SIZE);
                    RenderCaptureCube(sp, CaptureView(i), captureProj);
                }
            }

            // —— 2. Irradiance ——
            irradianceCube.Allocate(IBL_IRRADIANCE_SIZE, 1, GL_RGB16F);
            {
                auto *sp = ShaderProgram::Get("IBL_Irradiance");
                sp->Use();
                envCube.BindToSlot(0);
                sp->SetUniform("environmentMap", 0);
                for (int i = 0; i < 6; i++) {
                    AttachCubeFace(irradianceCube.id, i, 0, IBL_IRRADIANCE_SIZE);
                    RenderCaptureCube(sp, CaptureView(i), captureProj);
                }
            }

            // —— 3. Prefilter ——
            prefilterCube.Allocate(IBL_PREFILTER_SIZE, IBL_PREFILTER_MIPS, GL_RGB16F);
            {
                auto *sp = ShaderProgram::Get("IBL_Prefilter");
                sp->Use();
                envCube.BindToSlot(0);
                sp->SetUniform("environmentMap", 0);
                for (int mip = 0; mip < IBL_PREFILTER_MIPS; mip++) {
                    float roughness = float(mip) / float(IBL_PREFILTER_MIPS - 1);
                    sp->SetUniform("Roughness", roughness);
                    for (int i = 0; i < 6; i++) {
                        AttachCubeFace(prefilterCube.id, i, mip, IBL_PREFILTER_SIZE);
                        RenderCaptureCube(sp, CaptureView(i), captureProj);
                    }
                }
            }

            // —— 4. BRDF LUT ——
            if (brdfLUT == 0) glGenTextures(1, &brdfLUT);
            glBindTexture(GL_TEXTURE_2D, brdfLUT);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, IBL_BRDFLUT_SIZE, IBL_BRDFLUT_SIZE,
                         0, GL_RG, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
            {
                auto *sp = ShaderProgram::Get("IBL_BRDFLUT");
                sp->Use();
                Attach2D(brdfLUT);
                glBindVertexArray(Utils::GetEmptyVAO());
                glDrawArrays(GL_TRIANGLES, 0, 3);
                glBindVertexArray(0);
            }

            // 恢复状态
            glDepthMask(GL_TRUE);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // glViewport(0, 0, w, h); // 会被主循环重设，暂不处理
            glDeleteTextures(1, &envEquirect);

            generated = true;
            LogS(TAG) << "IBL 生成完成";
        }

        GLuint envEquirect = 0;   // GL_TEXTURE_2D (RGB32F) 源
        Cubemap envCube;
        Cubemap irradianceCube;
        Cubemap prefilterCube;
        GLuint brdfLUT = 0;       // GL_TEXTURE_2D (RG16F)
        GLuint captureFBO = 0;
        bool generated = false;

        static IBL *sActive;
    };

    const char *IBL::TAG = "IBL";
    IBL *IBL::sActive = nullptr;
}
