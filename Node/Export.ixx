module;
#include <typeinfo>
export module CEngine.Node;
export import :Node;
export import :Node3D;
export import :RenderUnit3D;
export import :PBR3D;
export import :Camera3D;
export import :Behaviour;
export import :BehaviourFactory;
import CEngine.Logger;

void CEngine::Node::PrintChildrenTree(const Logger::LogLevel ll) {
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
