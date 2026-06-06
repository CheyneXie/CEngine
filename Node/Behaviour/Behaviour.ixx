/**
 * @file Behaviour.ixx
 * @brief
 * @version 1.0
 * @author Chaim
 * @date 2024/10/20
 */

export module CEngine.Node:Behaviour;
import CEngine.Base;
import CEngine.Logger;

namespace CEngine {
    export class Node;

    export class Behaviour : public Object {
    public:
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
        std::string GetName() { return Name; }

    protected:
        Node *ParentNode;
        std::string Name;

    private:
        bool ReadyCalled = false;
    };
}
