/**
* @file Logger.ixx
 * @brief 日志类
 * @version 1.0
 * @author Chaim
 * @date 2024/09/27
 */

module;
#include <cstdio>
#include "glm/glm.hpp"
export module CEngine.Logger;
import std;

#define DEBUG_MODE

namespace CEngine {
    export class Logger {
    public:
        /**
         * @brief 颜色枚举值
         */
        struct Color {
            const static char *FONT_BLACK;
            const static char *FONT_RED;
            const static char *FONT_GREEN;
            const static char *FONT_YELLOW;
            const static char *FONT_BLUE;
            const static char *FONT_WHITE;
            const static char *FONT_GRAY;
            const static char *FONT_DEFAULT;

            const static char *BACKGROUND_BLACK;
            const static char *BACKGROUND_RED;
            const static char *BACKGROUND_RED_2;
            const static char *BACKGROUND_GREEN;
            const static char *BACKGROUND_GREEN_2;
            const static char *BACKGROUND_YELLOW;
            const static char *BACKGROUND_YELLOW_2;
            const static char *BACKGROUND_BLUE;
            const static char *BACKGROUND_BLUE_2;
            const static char *BACKGROUND_WHITE;
            const static char *BACKGROUND_GRAY;
            const static char *BACKGROUND_DEFAULT;
        };

        /**
         * @日志等级枚举值
         */
        enum class LogLevel {
            D,
            I,
            W,
            E,
            S
        };

        /**
         * 生成颜色代码
         * @param font 字体颜色
         * @param bg 背景颜色
         * @return 对应的颜色代码
         */
        static std::string MakeColorCode(const char *font, const char *bg);

        Logger() : Logger(LogLevel::I, "日志") {
        }

        /**
         * @param ll 日志等级
         * @param TAG 日志标签
         */
        Logger(LogLevel ll, const char *TAG);

        ~Logger() = default;

        /**
         * 输出日志
         * @param value 输出内容
         */
        template<typename T>
        Logger &operator<<(const T &value);

    private:
        LogLevel level;
    };

    const char *Logger::Color::FONT_BLACK = "30";
    const char *Logger::Color::FONT_RED = "91";
    const char *Logger::Color::FONT_GREEN = "92";
    const char *Logger::Color::FONT_YELLOW = "93";
    const char *Logger::Color::FONT_BLUE = "34";
    const char *Logger::Color::FONT_WHITE = "97";
    const char *Logger::Color::FONT_GRAY = "37";
    const char *Logger::Color::FONT_DEFAULT = "39";

    const char *Logger::Color::BACKGROUND_BLACK = "40";
    const char *Logger::Color::BACKGROUND_RED = "41";
    const char *Logger::Color::BACKGROUND_RED_2 = "101";
    const char *Logger::Color::BACKGROUND_GREEN = "42";
    const char *Logger::Color::BACKGROUND_GREEN_2 = "102";
    const char *Logger::Color::BACKGROUND_YELLOW = "43";
    const char *Logger::Color::BACKGROUND_YELLOW_2 = "103";
    const char *Logger::Color::BACKGROUND_BLUE = "44";
    const char *Logger::Color::BACKGROUND_BLUE_2 = "106";
    const char *Logger::Color::BACKGROUND_WHITE = "47";
    const char *Logger::Color::BACKGROUND_GRAY = "100";
    const char *Logger::Color::BACKGROUND_DEFAULT = "49";


    Logger::Logger(const LogLevel ll, const char *TAG) : level(ll) {
        switch (ll) {
            case Logger::LogLevel::D:
#ifdef DEBUG_MODE
                std::cout << "\n" << MakeColorCode(Color::FONT_WHITE, Color::BACKGROUND_GRAY) << " 调试 " <<
                        MakeColorCode(Color::FONT_BLACK, Color::BACKGROUND_WHITE) << " " << TAG << " " << MakeColorCode(
                            Color::FONT_GRAY, Color::BACKGROUND_DEFAULT) << " ";
#endif // DEBUG_MODE
                break;
            case Logger::LogLevel::I:
                std::cout << "\n" << MakeColorCode(Color::FONT_WHITE, Color::BACKGROUND_BLUE) << " 信息 " <<
                        MakeColorCode(Color::FONT_BLACK, Color::BACKGROUND_BLUE_2) << " " << TAG << " " << MakeColorCode(
                            Color::FONT_BLUE, Color::BACKGROUND_DEFAULT) << " ";
                break;
            case Logger::LogLevel::W:
                std::cout << "\n" << MakeColorCode(Color::FONT_WHITE, Color::BACKGROUND_YELLOW) << " 警告 " <<
                        MakeColorCode(Color::FONT_BLACK, Color::BACKGROUND_YELLOW_2) << " " << TAG << " " << MakeColorCode(
                            Color::FONT_YELLOW, Color::BACKGROUND_DEFAULT) << " ";
                break;
            case Logger::LogLevel::E:
                std::cerr << "\n" << MakeColorCode(Color::FONT_WHITE, Color::BACKGROUND_RED) << " 错误 " <<
                        MakeColorCode(Color::FONT_BLACK, Color::BACKGROUND_RED_2) << " " << TAG << " " << MakeColorCode(
                            Color::FONT_RED, Color::BACKGROUND_DEFAULT) << " ";
                break;
            case Logger::LogLevel::S:
                std::cout << "\n" << MakeColorCode(Color::FONT_WHITE, Color::BACKGROUND_GREEN) << " 成功 " <<
                        MakeColorCode(Color::FONT_BLACK, Color::BACKGROUND_GREEN_2) << " " << TAG << " " << MakeColorCode(
                            Color::FONT_GREEN, Color::BACKGROUND_DEFAULT) << " ";
                break;
        }
    }

    std::string Logger::MakeColorCode(const char *font, const char *bg) {
        std::stringstream ss;
        ss << "\033[" << font << ";" << bg << "m";
        return ss.str();
    }

    template<typename T>
    Logger &Logger::operator<<(const T &value) {
        if (level == LogLevel::E)
            std::cerr << value;
#ifndef DEBUG_MODE
        else if (level == LogLevel::D)
            return *this;
#endif // !DEBUG_MODE
        else
            std::cout << value;
        std::fflush(stdout);
        return *this;
    }

    // template Logger &Logger::operator<<(int);
    // template Logger &Logger::operator<<(unsigned int);
    // template Logger &Logger::operator<<(float);
    // template Logger &Logger::operator<<(double);
    // template Logger &Logger::operator<<(long);
    // template Logger &Logger::operator<<(long long);
    // template Logger &Logger::operator<<(unsigned long);
    // template Logger &Logger::operator<<(unsigned long long);
    // template Logger &Logger::operator<<(char *);
    // template Logger &Logger::operator<<(const char *);
    // template Logger &Logger::operator<<(std::string);
    // template Logger &Logger::operator<<(bool);

    template<>
    Logger &Logger::operator<<<glm::vec2>(const glm::vec2 &value) {
        *this << "vec2( " << value.x << ", " << value.y << ")";
        return *this;
    }

    template<>
    Logger &Logger::operator<<<glm::vec3>(const glm::vec3 &value) {
        *this << "vec3( " << value.x << ", " << value.y << ", " << value.z << ")";
        return *this;
    }

    template<>
    Logger &Logger::operator<<<glm::vec4>(const glm::vec4 &value) {
        *this << "vec4( " << value.x << ", " << value.y << ", " << value.z << ", " << value.w << ")";
        return *this;
    }

    template<>
    Logger &Logger::operator<<<glm::mat3>(const glm::mat3 &value) {
        *this << "glm::mat3\n";
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                *this << value[i][j] << " ";
            }
            *this << "\n";
        }
        return *this;
    }

    template<>
    Logger &Logger::operator<<<glm::mat4>(const glm::mat4 &value) {
        *this << "glm::mat4\n";
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                *this << value[i][j] << " ";
            }
            *this << "\n";
        }
        return *this;
    }
}


namespace CEngine {
    export Logger LogD(const char *TAG) {
        return std::move(Logger(Logger::LogLevel::D, TAG));
    }

    export Logger LogI(const char *TAG) {
        return std::move(Logger(Logger::LogLevel::I, TAG));
    }

    export Logger LogS(const char *TAG) {
        return std::move(Logger(Logger::LogLevel::S, TAG));
    }

    export Logger LogW(const char *TAG) {
        return std::move(Logger(Logger::LogLevel::W, TAG));
    }

    export Logger LogE(const char *TAG) {
        return std::move(Logger(Logger::LogLevel::E, TAG));
    }
}
