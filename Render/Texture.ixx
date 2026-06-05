/**
 * @file Texture.ixx
 * @brief 纹理贴图
 * @version 1.0
 * @author Chaim
 * @date 2024/10/05
 */

module;
#include <glad/glad.h>

#include <utility>
#include "md5.hpp"
export module CEngine.Render:Texture;
import std;
import CEngine.Base;
import CEngine.Image;
import CEngine.Logger;
import CEngine.Utils;

namespace CEngine {
    export class Texture final : public Object {
    public:
        static const char *TAG;
        static std::unordered_map<std::string, Texture *> All_Instances;

        /// 重置纹理槽，需要在每次DrawCall后调用
        static void ResetTextureSlot() {
            CurrentTextureSlot = 0;
        }

        static Texture *Create(std::string name, const ImageBuffer &img) {
            // 非法路径字符作为引擎内置纹理的标识符
            if (!name.contains("<") & !name.contains(">")) {
                // 计算MD5
                const auto _md5 = md5::digestString(img.GetBuffer(), img.GetHeight() * img.GetWidth());
                // 非内置纹理添加MD5防撞
                name += "#" + _md5;
            }
            if (All_Instances.contains(name)) return All_Instances[name];
            // 上传GPU
            unsigned int id = 0;
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            int internalFormat = GL_RGBA8;
            int dataFormat = GL_RGBA;
            switch (img.GetColorMode()) {
                case ColorMode::GRAY:
                    internalFormat = GL_R8;
                    dataFormat = GL_RED;
                    break;
                case ColorMode::GRAY_A:
                    internalFormat = GL_RG8;
                    dataFormat = GL_RG;
                    break;
                case ColorMode::RGB:
                    internalFormat = GL_RGB8;
                    dataFormat = GL_RGB;
                    break;
                case ColorMode::RGBA:
                    internalFormat = GL_RGBA8;
                    dataFormat = GL_RGBA;
                    break;
                default: break;
            }
            // internalFormat = GL_RGB;
            // dataFormat = GL_RGB;
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, static_cast<GLsizei>(img.GetWidth()), static_cast<GLsizei>(img.GetHeight()), 0, dataFormat,
                         GL_UNSIGNED_BYTE, img.GetBuffer());
            glBindTexture(GL_TEXTURE_2D, 0);
            auto tex = new Texture(id, name, internalFormat, dataFormat, img.GetWidth(), img.GetHeight());
            All_Instances.emplace(name, tex);
            return tex;
        }

        static Texture *FromFile(const char *img_path) {
            if (!Utils::FileExists(img_path)) {
                LogE(TAG) << "文件不存在: " << img_path;
                return nullptr;
            }
            const auto img = ImageBuffer(img_path);
            return Create(Utils::GetFileName(img_path, true), img);
        }

        Texture(const unsigned int id, std::string name, const int internal_format, const int data_format, const unsigned int width, const unsigned int height)
            : TextureID(id), InternalFormat(internal_format), DataFormat(data_format), Width(width), Height(height), Md5(std::move(name)) {
        }

        Texture(const Texture &) = delete;
        Texture(Texture &&tex) = delete;
        Texture &operator=(Texture &&tex) = delete;
        Texture &operator=(Texture &tex) = delete;

        ~Texture() override {
            glDeleteTextures(1, &TextureID);
            All_Instances.erase(Md5);
        }

        int Use() const {
            if (CurrentTextureSlot > 15) {
                LogE(TAG) << "当前纹理槽已满！";
                return -1;
            }
            glActiveTexture(GL_TEXTURE0 + CurrentTextureSlot);
            glBindTexture(GL_TEXTURE_2D, TextureID);
            return CurrentTextureSlot++;
        }

        static void UnUse() {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        /// @property TextureID
        unsigned int getTextureID() const { return TextureID; }

        /// @property InternalFormat
        int getInternalFormat() const { return InternalFormat; }

        /// @property DataFormat
        int getDataFormat() const { return DataFormat; }

        /// @property Md5
        std::string getMd5() const { return Md5; }

        /// @property Width
        unsigned int getWidth() const { return Width; }

        /// @property Height
        unsigned int getHeight() const { return Height; }

    private:
        /// 记录纹理槽，需要在每次DrawCall后重置为零
        static int CurrentTextureSlot;
        unsigned int TextureID = 0;
        int InternalFormat = GL_RGBA8;
        int DataFormat = GL_RGBA;
        unsigned int Width, Height;
        std::string Md5;
    };

    const char *Texture::TAG = "Texture";
    std::unordered_map<std::string, Texture *> Texture::All_Instances;
    int Texture::CurrentTextureSlot = 0;
}
