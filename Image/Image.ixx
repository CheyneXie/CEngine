/**
 * @file Image.ixx
 * @brief 图像类
 * @version 2.0
 * @author Chaim
 * @date 2024/09/30
 */

module;
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
export module CEngine.Image:Image;
import CEngine.Base;
import CEngine.Utils;
import CEngine.Logger;
import std;

namespace CEngine {
    ColorMode STBITypeToColorMode(int type) {
        switch (type) {
            case static_cast<int>(STBI_grey): return ColorMode::Gray;
            case static_cast<int>(STBI_grey_alpha): return ColorMode::GrayA;
            case static_cast<int>(STBI_rgb): return ColorMode::RGB;
            case static_cast<int>(STBI_rgb_alpha): return ColorMode::RGBA;
            default:
                LogE("STBITypeToColorMode") << "不支持像素格式: " << type;
                return ColorMode::NONE;
        }
    }
    template <typename T>
    concept IsDerivedFromBase = std::derived_from<T, Pixel>;
    template <typename T>
    concept SupportedPtr = std::is_same_v<T, float> || std::is_same_v<T, unsigned char> || std::is_same_v<T, unsigned short>;

    export class Image {
    public:
        Image(const Image &) = delete;
        Image &operator=(Image &tex) = delete;
        Image(Image &&other) noexcept : Width(other.Width), Height(other.Height), Precision(other.Precision), mColorMode(other.mColorMode), Data(other.Data) {
            other.Data = nullptr;
        }
        Image &operator=(Image &&other) noexcept {
            if (this != &other) {
                delete[] Data;
                Width = other.Width;
                Height = other.Height;
                Precision = other.Precision;
                mColorMode = other.mColorMode;
                Data = other.Data;
                other.Data = nullptr;
            }
            return *this;
        }

        Image() = default;

        template<IsDerivedFromBase T>
        Image(const unsigned int width, const unsigned int height, const int precision, T *data) : Width(width), Height(height), Precision(precision) {
            if constexpr (std::is_same_v<T, Gray>) mColorMode = ColorMode::Gray;
            else if constexpr (std::is_same_v<T, GrayA>) mColorMode = ColorMode::GrayA;
            else if constexpr (std::is_same_v<T, RGB>) mColorMode = ColorMode::RGB;
            else if constexpr (std::is_same_v<T, RGBA>) mColorMode = ColorMode::RGBA;
            else if constexpr (std::is_same_v<T, Pixel>) static_assert(false, "不接受基类 Pixel 指针");
            else static_assert(false, "未知颜色模式");

            const unsigned int size = width * height;
            if (size == 0) {
                LogE(TAG) << "图像长或宽不能为零!";
                return;
            }

            Data = new T[size];
            memcpy(Data, data, sizeof(T) * size);
        }

        ~Image() {
            delete[] Data;
        }

        template <SupportedPtr T>
        static Image FromBuffer(const unsigned int width, const unsigned int height, const int precision, T *buffer, const ColorMode mode) {
            assert(mode != ColorMode::NONE && "ColorMode 不合法");
            const unsigned int size = width * height;
            if (size == 0) {
                LogE(TAG) << "图像长或宽不能为零!";
                return Image();
            }

            switch (mode) {
                case ColorMode::Gray: {
                    std::vector<Gray> data;
                    data.reserve(size);
                    for (unsigned int i = 0; i < size; i++)
                        data.emplace_back(buffer[i]);
                    return Image(width, height, precision, data.data());
                }
                case ColorMode::GrayA: {
                    std::vector<GrayA> data;
                    data.reserve(size);
                    for (unsigned int i = 0; i < size; i++)
                        data.emplace_back(buffer[i * 2], buffer[i * 2 +1]);
                    return Image(width, height, precision, data.data());
                }
                case ColorMode::RGB: {
                    std::vector<RGB> data;
                    data.reserve(size);
                    for (unsigned int i = 0; i < size; i++)
                        data.emplace_back(buffer[i * 3], buffer[i * 3 + 1], buffer[i * 3 + 2]);
                    return Image(width, height, precision, data.data());
                }
                case ColorMode::RGBA: {
                    std::vector<RGBA> data;
                    data.reserve(size);
                    for (unsigned int i = 0; i < size; i++)
                        data.emplace_back(buffer[i * 4], buffer[i * 4 + 1], buffer[i * 4 + 2], buffer[i * 4 + 3]);
                    return Image(width, height, precision, data.data());
                }
                default: return Image();
            }
        }

        static Image FromFile(const char *file_path) {
            if (!Utils::FileExists(file_path)) {
                LogE(TAG) << "文件不存在: " << file_path;
                return Image();
            }
            int width = 0;
            int height = 0;
            int PicType = 0;
            if (stbi_is_hdr(file_path)) {
                float *buf = stbi_loadf(file_path, &width, &height, &PicType, STBI_default);
                auto img = FromBuffer(width, height, 32, buf, STBITypeToColorMode(PicType));
                stbi_image_free(buf);
                return std::move(img);
            } else if (stbi_is_16_bit(file_path)) {
                unsigned short *buf = stbi_load_16(file_path, &width, &height, &PicType, STBI_default);
                auto img = FromBuffer(width, height, 16, buf, STBITypeToColorMode(PicType));
                stbi_image_free(buf);
                return std::move(img);
            } else {
                unsigned char *buf = stbi_load(file_path, &width, &height, &PicType, STBI_default);
                auto img = FromBuffer(width, height, 8, buf, STBITypeToColorMode(PicType));
                stbi_image_free(buf);
                return std::move(img);
            }
            return Image();
        }

        bool IsValid() const { return Data != nullptr; }
        const Pixel *GetBuffer() const { return Data; }
        unsigned int GetWidth() const { return Width; }
        unsigned int GetHeight() const { return Height; }
        unsigned int GetPrecision() const { return Precision; }
        ColorMode GetColorMode() const { return mColorMode; }

    private:
        const static char *TAG;
        unsigned int Width = 0;
        unsigned int Height = 0;
        unsigned int Precision = 0;
        ColorMode mColorMode = ColorMode::NONE;
        Pixel *Data = nullptr;
    };

    const char *Image::TAG = "Image";
}
