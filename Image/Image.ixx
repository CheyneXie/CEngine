/**
 * @file Image.ixx
 * @brief 图像类
 * @version 1.0
 * @author Chaim
 * @date 2024/09/30
 */

export module CEngine.Image:Image;
import :Pixel;
import :ImageBuffer;

namespace CEngine {
    export class Image {
    public:
        template<typename T>
        Image(const unsigned int width, const unsigned int height, T data) : Width(width), Height(height), mColorMode(ColorMode::NONE), Data(nullptr) {
            const unsigned int size = width * height;
            if (size == 0) {
                LogE(TAG) << "图像长或宽不能为零!";
                return;
            }
            if constexpr (std::is_same_v<T, GRAY *>) mColorMode = ColorMode::GRAY;
            else if constexpr (std::is_same_v<T, GRAY_A *>) mColorMode = ColorMode::GRAY_A;
            else if constexpr (std::is_same_v<T, RGB *>) mColorMode = ColorMode::RGB;
            else if constexpr (std::is_same_v<T, RGBA *>) mColorMode = ColorMode::RGBA;
            else static_assert(false, "未知颜色模式");
            if (mColorMode != ColorMode::NONE) {
                Data = new T[size];
                memcpy(Data, data, sizeof(T) * size);
            } else {
                LogE(TAG) << "未知数据类型!";
                return;
            }
        }

        Image(const unsigned int width, const unsigned int height, const unsigned char *buf, const ColorMode mode) : Width(width), Height(height),
            mColorMode(mode), Data(nullptr) {
            const unsigned int size = width * height;
            if (size == 0) {
                LogE(TAG) << "图像长或宽不能为零!";
                return;
            }
            if (mColorMode == ColorMode::GRAY) {
                const auto data = new GRAY[size];
                Data = data;
                for (unsigned int i = 0; i < size; i++) {
                    data[i].Value = buf[i];
                }
            } else if (mColorMode == ColorMode::GRAY_A) {
                auto *data = new GRAY_A[size];
                Data = data;
                for (unsigned int i = 0; i < size; i++) {
                    data[i].Value = buf[i * 2];
                    data[i].Alpha = buf[i * 2 + 1];
                }
            } else if (mColorMode == ColorMode::RGB) {
                const auto data = new RGB[size];
                Data = data;
                for (unsigned int i = 0; i < size; i++) {
                    data[i].R = buf[i * 3];
                    data[i].G = buf[i * 3 + 1];
                    data[i].B = buf[i * 3 + 2];
                }
            } else if (mColorMode == ColorMode::RGBA) {
                const auto data = new RGBA[size];
                Data = data;
                for (unsigned int i = 0; i < size; i++) {
                    data[i].R = buf[i * 4];
                    data[i].G = buf[i * 4 + 1];
                    data[i].B = buf[i * 4 + 2];
                    data[i].A = buf[i * 4 + 3];
                }
            } else {
                LogE(TAG) << "未知数据类型!";
                return;
            }
        }

        explicit Image(const ImageBuffer &image_buf) {
            new(this) Image(image_buf.GetWidth(), image_buf.GetHeight(), image_buf.GetBuffer(), image_buf.GetColorMode());
        }

        explicit Image(const char *image_path) {
            new(this) Image(ImageBuffer(image_path));
        }

        ~Image() {
            if (!Data) return;
            if (mColorMode == ColorMode::GRAY) {
                delete[] static_cast<GRAY *>(Data);
            } else if (mColorMode == ColorMode::GRAY_A) {
                delete[] static_cast<GRAY_A *>(Data);
            } else if (mColorMode == ColorMode::RGB) {
                delete[] static_cast<RGB *>(Data);
            } else if (mColorMode == ColorMode::RGBA) {
                delete[] static_cast<RGBA *>(Data);
            } else {
                LogE(TAG) << "数据存在, 但类型未定义, 无法清除!";
            }
        }

        bool IsValid() const {
            if (Data) return true;
            return false;
        }

        void *GetBuffer() const {
            if (mColorMode == ColorMode::GRAY) {
                return static_cast<GRAY *>(Data);
            }
            if (mColorMode == ColorMode::GRAY_A) {
                return static_cast<GRAY_A *>(Data);
            }
            if (mColorMode == ColorMode::RGB) {
                return static_cast<RGB *>(Data);
            }
            if (mColorMode == ColorMode::RGBA) {
                return static_cast<RGBA *>(Data);
            }
            return nullptr;
        }

        unsigned int GetWidth() const { return Width; }
        unsigned int GetHeight() const { return Height; }
        ColorMode GetColorMode() const { return mColorMode; }

    private:
        const static char *TAG;
        unsigned int Width;
        unsigned int Height;
        ColorMode mColorMode;
        void *Data;
    };

    const char *Image::TAG = "Image";
}
