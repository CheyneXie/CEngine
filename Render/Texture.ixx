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

        /// 重置纹理槽，需要在每次DrawCall后调用
        static void ResetTextureSlot() {
            CurrentTextureSlot = 0;
        }

        static Texture *Create(std::string name, const Image &img) {
            if (!img.IsValid()) {
                LogE(TAG) << "无效图像";
                return nullptr;
            }

            int internalFormat = GL_RGBA8;
            int dataFormat = GL_RGBA;
            int precision = img.GetPrecision();
            int per_size = sizeof(float);
            switch (img.GetColorMode()) {
                case ColorMode::Gray:
                    internalFormat = precision == 32 ? GL_R32F : precision == 16 ? GL_R16 : GL_R8;
                    dataFormat = GL_RED;
                    break;
                case ColorMode::GrayA:
                    internalFormat = precision == 32 ? GL_RG32F : precision == 16 ? GL_RG16 : GL_RG8;
                    dataFormat = GL_RG;
                    per_size *= 2;
                    break;
                case ColorMode::RGB:
                    internalFormat = precision == 32 ? GL_RGB32F : precision == 16 ? GL_RGB16 : GL_RGB8;
                    dataFormat = GL_RGB;
                    per_size *= 3;
                    break;
                case ColorMode::RGBA:
                    internalFormat = precision == 32 ? GL_RGBA32F : precision == 16 ? GL_RGBA16 : GL_RGBA8;
                    dataFormat = GL_RGBA;
                    per_size *= 4;
                    break;
                default: break;
            }

            // 非法路径字符作为引擎内置纹理的标识符
            if (!name.contains("<") & !name.contains(">")) {
                // 计算MD5
                const auto _md5 = md5::digest(img.GetBuffer(), img.GetHeight() * img.GetWidth() * per_size);
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
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, static_cast<GLsizei>(img.GetWidth()), static_cast<GLsizei>(img.GetHeight()),
                0, dataFormat, GL_FLOAT, img.GetBuffer());
            glBindTexture(GL_TEXTURE_2D, 0);
            auto tex = new Texture(id, name, internalFormat, dataFormat, img.GetWidth(), img.GetHeight(), img.GetPrecision());
            All_Instances.emplace(name, tex);
            return tex;
        }

        static Texture *FromFile(const char *img_path) {
            if (!Utils::FileExists(img_path)) {
                LogE(TAG) << "文件不存在: " << img_path;
                return nullptr;
            }
            const auto img = Image::FromFile(img_path);
            return Create(Utils::GetFileName(img_path, true), img);
        }

        /**
         * @brief 获取已创建的纹理
         * 
         * @param name 着色器程序名称
         * @return Texture* 着色器程序指针
         */
        static Texture *Get(std::string_view name) {
            auto it = All_Instances.find(name);
            if (it == All_Instances.end()) {
                LogE(TAG) << "未找到指定 Texture: " << name;
                return nullptr;
            }
            return it->second;
        }

        /**
         * @brief 获取已创建纹理的数量
         * 
         * @return int 数量
         */
        static int Num() {
            return All_Instances.size();
        }

        /**
         * @brief Get All Instances
         * 
         * @return Texture::All_Instances
         */
        const static auto& Get() {
            return All_Instances;
        }

        Texture(const unsigned int id, std::string name, const int internal_format, const int data_format, const unsigned int width, const unsigned int height, const unsigned int precision)
            : TextureID(id), InternalFormat(internal_format), DataFormat(data_format), Width(width), Height(height), Precision(precision), Name(std::move(name)) {
        }

        Texture(const Texture &) = delete;
        Texture(Texture &&tex) = delete;
        Texture &operator=(Texture &&tex) = delete;
        Texture &operator=(Texture &tex) = delete;

        ~Texture() override {
            glDeleteTextures(1, &TextureID);
            All_Instances.erase(Name);
        }

        static int RequestTextureSlot() {
            if (CurrentTextureSlot > 15) {
                LogE(TAG) << "当前纹理槽已满！";
                return 0;
            }
            return CurrentTextureSlot++;
        }

        static int _Use(unsigned int id) {
            auto slot = RequestTextureSlot();
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, id);
            return slot;
        }

        static void UnUse() {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        int Use() const {
            return _Use(TextureID);
        }

        /// @property TextureID
        unsigned int getTextureID() const { return TextureID; }

        /// @property InternalFormat
        int getInternalFormat() const { return InternalFormat; }

        /// @property DataFormat
        int getDataFormat() const { return DataFormat; }

        /// @property Name
        std::string getName() const { return Name; }

        /// @property Width
        unsigned int getWidth() const { return Width; }

        /// @property Height
        unsigned int getHeight() const { return Height; }

        /// @property Precision
        unsigned int getPrecision() const { return Precision; }

    private:
        /// 记录纹理槽，需要在每次DrawCall后重置为零
        static int CurrentTextureSlot;
        unsigned int TextureID = 0;
        int InternalFormat = GL_RGBA8;
        int DataFormat = GL_RGBA;
        unsigned int Width, Height, Precision;
        std::string Name;

        static std::unordered_map<std::string, Texture *, StringHash, StringEqual> All_Instances;
    };

    const char *Texture::TAG = "Texture";
    std::unordered_map<std::string, Texture *, StringHash, StringEqual> Texture::All_Instances;
    int Texture::CurrentTextureSlot = 0;
}
