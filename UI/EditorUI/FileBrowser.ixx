/**
 * @file FileBrowser.ixx
 * @brief 文件浏览器
 * @version 1.0
 * @author Chaim
 * @date 2024/10/16
 */

module;
#include "imgui/imgui.h"
export module CEngine.EditorUI:FileBrowser;
import :SceneTreeBrowser;
import std;
import CEngine.Logger;
import CEngine.ModelImporter;
import CEngine.Engine;
import CEngine.Render;

namespace fs = std::filesystem;

namespace CEngine {
    // 文件节点类型（文件或文件夹）
    enum class FileNodeType {
        File,
        Directory
    };

    // 文件节点结构体，用于缓存文件和目录信息
    struct FileNode {
        fs::path path; // path
        std::string name;
        FileNodeType type; // 节点类型
        std::vector<FileNode> children; // 子节点列表（如果是目录，则存储子节点）

        FileNode(const fs::path &path, const FileNodeType type) : path(path), type(type) {
            name = path.filename().string();
        }
    };

    // 缓存存储类
    class FileBrowser {
    public:
        // 根节点存储
        FileNode root;

        // 构造函数初始化根节点
        FileBrowser(const fs::path &directory_path) : root("Root", FileNodeType::Directory), root_path(directory_path) {
        }

        // 刷新缓存的函数，读取文件系统并更新缓存
        void RefreshCache() {
            root.children.clear(); // 清空原缓存
            CacheDirectory(root_path, root); // 从根目录开始重新缓存
        }

        // 展示缓存的文件树，按文件夹在前、文件在后显示
        void ShowDirectory(const SceneTreeBrowser &scene_tree_browser, FileNode *node = nullptr) {
            if (node == nullptr) node = &root;
            if (node->type == FileNodeType::Directory) {
                auto flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
                if (node == &root) flags |= ImGuiTreeNodeFlags_DefaultOpen;
                // 使用 TreeNodeEx 让目录节点具有选中状态
                if (ImGui::TreeNodeEx(node->name.c_str(), flags)) {
                    // 按文件夹在前，文件在后排序
                    std::ranges::sort(node->children, [](const FileNode &a, const FileNode &b) {
                        return a.type == FileNodeType::Directory && b.type == FileNodeType::File;
                    });

                    // 递归展示子节点
                    for (auto &child: node->children) {
                        ShowDirectory(scene_tree_browser, &child);
                    }
                    ImGui::TreePop();
                }
            } else if (node->type == FileNodeType::File) {
                // 如果是文件
                ImGui::Selectable(std::format("   {}", node->name).c_str());
                // 展示右键菜单
                if (ImGui::BeginPopupContextItem(node->name.c_str())) {
                    for (const auto& name: (ShaderProgram::All_Instances | std::views::keys)) {
                        if (ImGui::MenuItem(std::format("Open With Shader \"{}\"", name).c_str())) {
                            if (const auto model = ModelImporter::import_model(node->path.string().c_str(), name); model->IsValid()) {
                                if (scene_tree_browser.NodeSelected != nullptr)
                                    scene_tree_browser.NodeSelected->AddChild(model);
                                else
                                    Engine::GetIns()->getRoot()->AddChild(model);
                            }
                        }
                    }
                    ImGui::EndPopup();
                }
            }
        }

        /// @property root_path
        void setRootPath(const fs::path &path) {
            if (!exists(path)) {
                LogE("EditorUI:FileBrowser") << "错误的文件夹路径: " << path.string();
                return;
            }
            root_path = path;
            RefreshCache();
        }

        /// @property root_path
        fs::path getRootPath() {
            return root_path;
        }

    private:
        fs::path root_path;
        // 递归函数，将目录和文件缓存到FileNode结构中
        void CacheDirectory(const fs::path &directory_path, FileNode &parent_node) {
            for (const auto &entry: fs::directory_iterator(directory_path)) {
                const auto &path = entry.path();
                if (entry.is_directory()) {
                    FileNode dirNode(path, FileNodeType::Directory);
                    CacheDirectory(path, dirNode); // 递归缓存子目录
                    parent_node.children.push_back(dirNode);
                } else if (entry.is_regular_file()) {
                    parent_node.children.push_back(FileNode(path, FileNodeType::File));
                }
            }
        }
    };
}
