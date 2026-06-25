/**
 * @file Directional.ixx
 * @author Cheyne Xie
 * @brief 平行光
 * @version 1.0
 * @date 2026-06-24
 * 
 */

module;
#include <glm/glm.hpp>
export module CEngine.Light:Directional;
import :Base;
import std;

namespace CEngine::Light {
    export class Directional : public Base {
    public:
        Type GetType() override { return Type::Directional; }

        static std::unique_ptr<Directional> Create() {
            return std::unique_ptr<Directional>(new Directional());
        }

        Directional(const Directional&) = delete;
        Directional& operator=(const Directional&) = delete;
        Directional(Directional&&) = default;
        Directional& operator=(Directional&&) = default;

        bool SetDirectionManually = false;

        glm::vec3 getDirection() { return Direction; }
        void setDirection(glm::vec3&& dir) { Direction = dir; }

    protected:
        Directional() = default;

        glm::vec3 Direction = {0, -1, 0};
    };
}