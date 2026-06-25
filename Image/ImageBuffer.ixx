/**
 * @file ImageBuffer.ixx
 * @brief 图片数据
 * @version 1.0
 * @author Chaim
 * @date 2024/09/29
 */

module;
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
export module CEngine.Image:ImageBuffer;
import CEngine.Base;
import CEngine.Logger;

namespace CEngine {
    export class ImageBuffer {
    public:
        ImageBuffer(const unsigned int width, const unsigned int height, const unsigned char *data, const ColorMode mode) : Data(nullptr), Width(width),
            Height(height), mColorMode(mode) {
            const unsigned int size = width * height * ColorMode_GetChannelCount(mode);
            if (size == 0) {
                LogE(TAG) << "图像长或宽不能为零!";
                return;
            }
            if (mode == ColorMode::NONE) {
                LogE(TAG) << "未知数据类型!";
                return;
            }
            Data = new unsigned char[size];
            memcpy(Data, data, sizeof(unsigned char) * size);
        };

        explicit ImageBuffer(const char *img_path) : Data(nullptr), Width(0), Height(0), mColorMode(ColorMode::NONE) {
            LogI(TAG) << "读取文件: " << img_path;
            int width = 0;
            int height = 0;
            int PicType = 0;
            unsigned char *buf = stbi_load(img_path, &width, &height, &PicType, STBI_default);
            switch (PicType) {
                case static_cast<int>(STBI_grey):
                    mColorMode = ColorMode::GRAY;
                    break;
                case static_cast<int>(STBI_grey_alpha):
                    mColorMode = ColorMode::GRAY_A;
                    break;
                case static_cast<int>(STBI_rgb):
                    mColorMode = ColorMode::RGB;
                    break;
                case static_cast<int>(STBI_rgb_alpha):
                    mColorMode = ColorMode::RGBA;
                    break;
                default: break;
            }
            new(this) ImageBuffer(width, height, buf, mColorMode);
            stbi_image_free(buf);
        }

        ~ImageBuffer() {
            delete[] Data;
        }

        bool IsValid() const {
            if (Data) return true;
            return false;
        }

        const unsigned char *GetBuffer() const { return Data; }
        unsigned int GetWidth() const { return Width; }
        unsigned int GetHeight() const { return Height; }
        ColorMode GetColorMode() const { return mColorMode; }

    private:
        const static char *TAG;
        unsigned char *Data;
        unsigned int Width;
        unsigned int Height;
        ColorMode mColorMode{};
    };

    const char *ImageBuffer::TAG = "ImageBuffer";
}
