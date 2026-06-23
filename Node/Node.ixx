/**
 * @file Node.ixx
 * @brief 节点类
 * @version 1.0
 * @author Chaim
 * @date 2024/10/06
 */

module;
export module CEngine.Node:Node;
import :NodeType;
import :Behaviour;
import std;
import CEngine.Base;
import CEngine.Utils;
import CEngine.Logger;

namespace CEngine {
    /**
     * @brief 节点类
     * @remark 仅实现父子关系
     * @todo 实现引擎相关处理流程
     */
    export class Node : public Object {
    public:
        const static char *TAG;

        static Node *Create() {
            return new Node();
        }

        Node(const Node &other) = delete;
        Node &operator=(const Node &other) = delete;
        Node(Node &&other) noexcept = delete;
        Node &operator=(Node &&other) = delete;

        ~Node() override {
            for (const auto child: Children | std::views::values) {
                delete child;
            }
            if (Behaviour) delete Behaviour;
        }

        virtual NodeType GetType() { return NodeType::Node; }
        virtual bool IsType(NodeType type) { return type == NodeType::Node; }

        /**
         * 获取子级数量
         * @return 子级数量
         */
        std::size_t GetChildCount() const {
            return Children.size();
        }

        /**
         * 获取全部子级指针
         * @return 全部子级指针的vector容器
         */
        std::vector<Node *> GetChildren() {
            return std::ranges::to<std::vector<Node *> >(Children | std::views::values);
        }

        /**
         * 添加子级
         * @remark 注意：若节点已有父级，则会从原父级弹出！
         * @param node 子级
         */
        void AddChild(Node *node) {
            // 判断节点是否有原父级
            if (const auto raw_parent = node->Parent; raw_parent != nullptr) {
                raw_parent->RemoveChild(node->Name); // 从原父级中删除
            }
            node->Parent = this;
            node->setName(node->Name); // 原地设置，让setName自动判断是否有重复名
            Children.emplace(node->Name, node);
        }

        /**
         * 获得指定子级指针
         * @param name 目标名称
         * @return 目标子级指针
         */
        template<class T> requires std::derived_from<T, Node>
        T *GetChild(const std::string_view name) {
            if (auto it = Children.find(name); it != Children.end()) {
                return dynamic_cast<T *>(it->second);
            }
            return nullptr;
        }

        /// @link GetChild \endlink
        template<class T> requires std::derived_from<T, Node>
        T *operator[](const std::string_view name) {
            return GetChild<T>(name);
        }

        /**
         * 弹出子级
         * @param name 目标名称
         * @return 目标对象的optional<Node>容器
         */
        template<class T> requires std::derived_from<T, Node>
        T *PopChild(const std::string_view name) {
            const auto t = GetChild<T>(name);
            if (t != nullptr)
                t->Parent = nullptr;
            return t;
        }

        /**
         * 删除子级
         * @param name 目标名称
         */
        void RemoveChild(const std::string_view name) {
            delete PopChild<Node>(name);
        }

        void RemoveAllChildren() {
            for (const auto child: Children | std::views::values) {
                delete child;
            }
            Children.clear();
        }

        /**
         * 判断是否存在对应子级
         * @param name 子级名称
         * @return 是否存在对应子级
         */
        bool HasChild(const std::string_view name) const {
            return Children.contains(name);
        }

        /// @file Export.ixx
        void PrintChildrenTree(Logger::LogLevel ll = Logger::LogLevel::D) {
            auto Log = Logger(ll, TAG);
            Log << "\n";
            std::stack<std::pair<Node *, int> > _stack;
            _stack.push({this, 0});
            while (!_stack.empty()) {
                auto [node, tab] = _stack.top();
                _stack.pop();
                for (int i = 0; i < tab; ++i) {
                    Log << "    ";
                }
                Log << "(" << typeid(*node).name() << ") " << node->Name << "\n";
                for (auto child: node->Children | std::views::values) {
                    _stack.push({child, tab + 1});
                }
            }
            Logger(ll, TAG) << "输出完成";
        }

        /// @property Parent
        Node *getParent() const { return Parent; }

        /// @property Name
        std::string getName() const { return Name; }

        /// @property Name
        void setName(const std::string_view name) {
            const std::string new_name = name.empty() ? Utils::GenerateUUID() : std::string(name);
            if (Parent != nullptr) {
                // 判断所在层级中Name是否唯一
                auto desire_name = new_name;
                for (int i = 0; Parent->HasChild(desire_name); ++i) {
                    desire_name = std::format("{}.{}", new_name, i);
                }
                if (Parent->Children.contains(Name)) {
                    auto s = Parent->Children.extract(Name); // 弹出改key
                    s.key() = desire_name;
                    Parent->Children.insert(std::move(s));
                }
                Name = desire_name;
            } else {
                Name = new_name;
            }
        }

        /// @property Behaviour
        Behaviour *GetBehaviour() const { return Behaviour; }

        /// @property Behaviour
        void SetBehaviour(Behaviour *behaviour) {
            if (Behaviour != nullptr) delete Behaviour;
            Behaviour = behaviour;
            if (Behaviour == nullptr) return;
            Behaviour->SetParentNode(this);
        }

        /// @property 是否激活
        bool IsActive() const { return Actived; }

        /// @property 设置激活
        void SetActive(bool active) { Actived = active; }

    protected:
        Node() {
            Name = Utils::GenerateUUID();
        }

        /// @brief Node的名称（在同层级中唯一）
        std::string Name;
        /// @brief 父级
        Node *Parent = nullptr;
        /// @brief 所有子级
        std::unordered_map<std::string, Node *, StringHash, StringEqual> Children;
        /// @brief Behaviour
        Behaviour *Behaviour = nullptr;
        /// @brief 是否激活
        bool Actived = true;
    };

    const char *Node::TAG = "Node";
}
