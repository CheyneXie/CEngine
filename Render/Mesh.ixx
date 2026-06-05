/**
 * @file Mesh.ixx
 * @brief 网格体类
 * @version 1.0
 * @author Chaim
 * @date 2024/10/05
 */

module;
#include <glad/glad.h>
#include <glm/glm.hpp>
export module CEngine.Render:Mesh;
import :Texture;
import std;
import CEngine.Base;
import CEngine.Logger;

namespace CEngine {
    /**
     * @brief 顶点信息
     */
    export struct VertexInfo {
        /// @brief 顶点位置
        glm::vec3 Position;
        /// @brief 法线
        glm::vec3 Normal;
        /// @brief 切线
        glm::vec4 Tangent;
        /// @brief 纹理坐标
        glm::vec2 TexCoord;
        /**
        * 比较运算符
        */
        bool operator==(const VertexInfo &other) const {
            return Position == other.Position && Normal == other.Normal && TexCoord == other.TexCoord;
        }

        void Print(Logger &logger) const {
            logger << "{" << Position << "," << Normal << "," << TexCoord << "}";
        }
    };


    /**
    * @class Mesh
    * @brief 网格基类
    * @remark 仅实现OpenGL相关功能
    */
    export class Mesh final : public Object {
    public:
        const static char *TAG;
        static std::vector<Mesh *> All_Instances;

        Mesh(const Mesh &other) = delete;
        Mesh(Mesh &&other) = delete;
        Mesh &operator=(const Mesh &other) = delete;
        Mesh &operator=(Mesh &&other) = delete;

        ~Mesh() override {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
        };

        /**
         * @param vbi 顶点信息数据
         * @param ebi 索引数据
         */
        static Mesh *Create(const std::vector<VertexInfo> &vbi, const std::vector<unsigned int> &ebi) {
            return new Mesh(vbi, ebi);
        }

        /**
        * 渲染Mesh
        * @remark 请先设置Shader
        */
        void Render() const {
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, static_cast<int>(indices_size), GL_UNSIGNED_INT, nullptr);
        }

        /// 不重要
        std::string Name;

    protected:
        /**
         * @param vbi 顶点信息数据
         * @param ebi 索引数据
         */
        Mesh(const std::vector<VertexInfo> &vbi, const std::vector<unsigned int> &ebi) {
            indices_size = ebi.size();
            const auto vbi_size = vbi.size() * sizeof(VertexInfo);
            LogD(TAG) << "向GPU传输数据(顶点信息: " << vbi_size << "字节, 索引: " << indices_size << "组)";
            /// 生成VAO
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);
            // 处理顶点信息数据
            /// 生成VBO
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            /// 传入VBO数据
            glBufferData(GL_ARRAY_BUFFER, vbi_size, vbi.data(), GL_STATIC_DRAW);
            /// 设置锚定点
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), reinterpret_cast<void *>(offsetof(VertexInfo, Position)));
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), reinterpret_cast<void *>(offsetof(VertexInfo, Normal)));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), reinterpret_cast<void *>(offsetof(VertexInfo, Tangent)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), reinterpret_cast<void *>(offsetof(VertexInfo, TexCoord)));
            glEnableVertexAttribArray(3);
            // 处理索引数据
            /// 生成EBO
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            /// 传入EBO数据
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size * sizeof(unsigned int), ebi.data(), GL_STATIC_DRAW);
            // 解绑VAO
            glBindVertexArray(0);
            All_Instances.push_back(this);
        };
        /// @brief VAO
        unsigned int VAO = 0;
        /// @brief VBO
        unsigned int VBO = 0;
        /// @brief EBO
        unsigned int EBO = 0;
        /// @brief 索引总数
        size_t indices_size = 0;
    };

    const char *Mesh::TAG = "Mesh";
    std::vector<Mesh *> Mesh::All_Instances;
}
