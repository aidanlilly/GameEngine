#include "ProjectPanel.h"
#include "imgui.h"
#include <filesystem>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

ProjectPanel::ProjectPanel() : projectPath_(".") {}

void ProjectPanel::setProjectPath(const std::string& path) {
    projectPath_ = path;
}

void ProjectPanel::renderDirectoryTree(const std::string& path, int depth) {
    if (!fs::exists(path) || !fs::is_directory(path)) return;

    std::vector<fs::directory_entry> entries;
    for (auto& entry : fs::directory_iterator(path)) {
        entries.push_back(entry);
    }

    // Sort: directories first, then files, alphabetically
    std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
        bool aIsDir = a.is_directory();
        bool bIsDir = b.is_directory();
        if (aIsDir != bIsDir) return aIsDir > bIsDir;
        return a.path().filename().string() < b.path().filename().string();
    });

    for (auto& entry : entries) {
        std::string filename = entry.path().filename().string();
        
        // Skip build directories and hidden files
        if (filename == "build" || filename == ".git" || filename[0] == '.') continue;

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        if (entry.is_directory()) {
            flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
            bool isOpen = ImGui::TreeNodeEx(filename.c_str(), flags);
            if (isOpen) {
                renderDirectoryTree(entry.path().string(), depth + 1);
                ImGui::TreePop();
            }
        } else {
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth;
            ImGui::TreeNodeEx(filename.c_str(), flags);
        }
    }
}

void ProjectPanel::render() {
    ImGui::BeginChild("ProjectPanel", ImVec2(0, 0), true);
    ImGui::Text("Project Directory");
    ImGui::Separator();
    
    if (!projectPath_.empty()) {
        renderDirectoryTree(projectPath_);
    } else {
        ImGui::TextDisabled("No project path set");
    }
    
    ImGui::EndChild();
}
