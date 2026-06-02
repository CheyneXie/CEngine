/**
 * @file Factory.ixx
 * @brief Behaviour工厂
 * @version 1.0
 * @author Chaim
 * @date 2024/10/20
 */

export module CEngine.Node:BehaviourFactory;
import :Behaviour;
import std;
import CEngine.Logger;

namespace CEngine {
    export class BehaviourFactory {
    public:
        template<class T>
        static void Register() {
            Behaviours[T::Name] = []() -> Behaviour* {
                LogD("Behaviour工厂") << "创建Behaviour: " << T::Name;
                return new T();
            };
            LogI("Behaviour工厂") << "注册Behaviour: " << T::Name;
        }

        static Behaviour* CreateBehaviour(const std::string &name) {
            if (Behaviours.contains(name))
                return Behaviours[name]();
            return nullptr;
        }

    private:
        static std::unordered_map<std::string, std::function<Behaviour*()> > Behaviours;
    };

    std::unordered_map<std::string, std::function<Behaviour*()> > BehaviourFactory::Behaviours;
}
