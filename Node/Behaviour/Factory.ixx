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
        static const char* TAG;
        template<class T>
        static void Register() {
            Behaviours[T::Name] = []() -> Behaviour* {
                LogD(TAG) << "创建Behaviour: " << T::Name;
                return new T(T::Name);
            };
            LogI("Behaviour工厂") << "注册Behaviour: " << T::Name;
        }

        static Behaviour* CreateBehaviour(const std::string &name) {
            if (Behaviours.contains(name))
                return Behaviours[name]();
            else LogE(TAG) << "未找到 Behaviour : " << name;
            return nullptr;
        }

        static auto GetAllBehavioursName() {
            return Behaviours | std::views::keys;
        }

    private:
        static std::unordered_map<std::string, std::function<Behaviour*()> > Behaviours;
    };
    const char* BehaviourFactory::TAG = "Behaviour工厂";
    std::unordered_map<std::string, std::function<Behaviour*()> > BehaviourFactory::Behaviours;
}
