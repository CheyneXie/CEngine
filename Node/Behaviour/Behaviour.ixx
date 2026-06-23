/**
 * @file Behaviour.ixx
 * @brief
 * @version 1.0
 * @author Chaim
 * @date 2024/10/20
 */

export module CEngine.Node:Behaviour;
import std;
import CEngine.Base;
import CEngine.Logger;

namespace CEngine {
    export class Node;

    export class Behaviour : public Object {
    public:
        static const char *TAG;
        Behaviour(const char *name) : Name(name) {
        }
        ~Behaviour() override = default;

        void Process(const double DeltaTime) {
            if (!ReadyCalled) if (!Ready()) return;
            Update(DeltaTime);
        }

        virtual bool Ready() = 0;

        virtual void Update(double DeltaTime) = 0;

        virtual void Release() {
        }

        void SetParentNode(Node *node) { ParentNode = node; }
        Node *GetParentNode() const { return ParentNode; }
        std::string GetName() const { return Name; }

        
        /// 参数基类，用于类型擦除
        struct ParamHolderBase {
            std::type_index type;
            virtual ~ParamHolderBase() = default;
            ParamHolderBase(std::type_index _type) : type(_type) {}
        };

        /// 参数具体类
        template<typename T>
        struct ParamHolder : ParamHolderBase {
            std::function<T()> getter;
            std::function<void(T)> setter;
            ParamHolder() : ParamHolderBase(typeid(T)) {}
        };

        auto& GetParams() { return ParamsStorage; }

        template<typename T>
        T Get(std::string_view name) const {
            auto it = ParamsStorage.find(name);
            if (it == ParamsStorage.end()) {
                LogE(TAG) << "未找到参数: " << name;
                throw std::runtime_error("[Behaviour]未找到参数");
            }
            if (it->second->type != typeid(T)) {
                LogE(TAG) << "参数类型不匹配: " << name;
                throw std::runtime_error("[Behaviour]参数类型不匹配");
            }
            auto holder = static_cast<ParamHolder<T>*>(it->second.get());
            return holder->getter();
        }

        template<typename T>
        T Set(std::string_view name) const {
            auto it = ParamsStorage.find(name);
            if (it == ParamsStorage.end()) {
                LogE(TAG) << "未找到参数: " << name;
                throw std::runtime_error("[Behaviour]未找到参数");
            }
            if (it->second->type != typeid(T)) {
                LogE(TAG) << "参数类型不匹配: " << name;
                throw std::runtime_error("[Behaviour]参数类型不匹配");
            }
            auto holder = static_cast<ParamHolder<T>*>(it->second.get());
            return holder->setter();
        }

    protected:
        ///
        Node *ParentNode;

        /// 将由派生类的静态字段Name自动传入，不要运行时修改
        const std::string Name;

        /// 参数存储，存储擦除后的指针
        std::unordered_map<std::string_view, std::unique_ptr<ParamHolderBase>> ParamsStorage;

        /// 注册参数
        template<typename T>
        void RegisterParam(std::string_view name, std::function<T()> getter, std::function<void(T)> setter) {
            auto holder = std::make_unique<ParamHolder<T>>();
            holder->getter = std::move(getter);
            holder->setter = std::move(setter);
            ParamsStorage[name] = std::move(holder);
        }

    private:
        bool ReadyCalled = false;
    };
    const char *Behaviour::TAG = "Behaviour";
}
