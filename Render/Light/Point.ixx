/**
 * @file Point.ixx
 * @author Cheyne Xie
 * @brief 点光
 * @version 1.0
 * @date 2026-06-24
 * 
 */

module;
#include <glad/glad.h>
#include <glm/glm.hpp>
export module CEngine.Light:Point;
import :Base;

namespace CEngine::Light {
    struct alignas(16) PointLightSSBO {
        glm::vec3 Position;         // offset  0, size 12, align 4
        float     ConstantFactor;   // offset 12, size 4,  align 4
        float     LinearFactor;     // offset 16, size 4,  align 4
        float     QuadraticFactor;  // offset 20, size 4,  align 4
        std::byte padding[8];       // offset 24, size 8
        glm::vec4 Color;            // offset 32, size 16, align 16
    };
    
    export class Point : public Base {
    public:
        /**
         * @brief 上传点光源数据（固定槽: 1）
         * @param lights 点光对象数组
         * @return int 点光数量
         */
        static int UploadSSBO(std::vector<Base*>& lights) {
            std::vector<PointLightSSBO> data;
            for (auto& l : lights) {
                if (l->GetType() != Type::Point) continue;
                auto p = static_cast<Point*>(l);
                data.push_back({
                    p->getPosition(),
                    p->getConstantFactor(),
                    p->getLinearFactor(),
                    p->getQuadraticFactor(),
                    {},
                    glm::vec4(p->getColor().ToVec3(), p->getIlluminance())
                });
            }
            static GLuint ssbo;
            glGenBuffers(1, &ssbo);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
            glBufferData(
                GL_SHADER_STORAGE_BUFFER,
                data.size() * sizeof(PointLightSSBO),
                data.data(),
                GL_DYNAMIC_DRAW
            );
            constexpr unsigned int BINDING_POINT = 1;
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BINDING_POINT, ssbo);
            return static_cast<int>(data.size());
        }

        Type GetType() override { return Type::Point; }

        static std::unique_ptr<Point> Create() {
            return std::unique_ptr<Point>(new Point());
        }

        Point(const Point&) = delete;
        Point& operator=(const Point&) = delete;
        Point(Point&&) = default;
        Point& operator=(Point&&) = default;

        glm::vec3 getPosition() { return Position; }
        // Position通过上层Light3D自动设置
        void setPosition(glm::vec3&& pos) { Position = pos; }

        float getConstantFactor() { return ConstantFactor; }
        void setConstantFactor(float k) { ConstantFactor = k; }

        float getLinearFactor() { return LinearFactor; }
        void setLinearFactor(float k) { LinearFactor = k; }

        float getQuadraticFactor() { return QuadraticFactor; }
        void setQuadraticFactor(float k) { QuadraticFactor = k; }

    protected:
        Point() = default;

        glm::vec3 Position = {0, -1, 0};

        /**
         * @brief 衰减常数
         */
        float ConstantFactor = 1;
        /**
         * @brief 线性衰减系数
         */
        float LinearFactor = 0.14;
        /**
         * @brief 平方衰减系数
         */
        float QuadraticFactor = 0.07;
    };
}