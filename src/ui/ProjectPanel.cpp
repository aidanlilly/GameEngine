#include "ProjectPanel.h"
#include "imgui.h"
#include <filesystem>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>

namespace fs = std::filesystem;

ProjectPanel::ProjectPanel() : projectPath_("GameProject"), initialized_(false) {}

void ProjectPanel::setProjectPath(const std::string& path) {
    initialized_ = false;
    try {
        fs::path p(path);
        auto try_use = [&](const fs::path& candidate) -> bool {
            std::error_code ec;
            fs::path can = fs::weakly_canonical(candidate, ec);
            if (ec) can = candidate;
            if (fs::exists(can) && fs::is_directory(can)) {
                projectPath_ = can.string();
                return true;
            }
            return false;
        };

        // If absolute and valid, use it
        if (p.is_absolute()) {
            if (try_use(p)) return;
        }

        // Otherwise, search relative to current working directory and parents
        fs::path cwd = fs::current_path();
        std::vector<fs::path> candidates = {
            p,
            cwd / p,
            cwd.parent_path() / p,
            cwd.parent_path().parent_path() / p
        };
        for (const auto& c : candidates) {
            if (try_use(c)) return;
        }

        // Fallback: keep as provided string (may be non-existent)
        projectPath_ = path;
    } catch (...) {
        projectPath_ = path;
    }
}

void ProjectPanel::ensureDefaultFolders() {
    if (initialized_) return;
    initialized_ = true;
    if (projectPath_.empty()) return;
    try {
        fs::path meshesDir = fs::path(projectPath_) / "Meshes";
        if (!fs::exists(meshesDir)) fs::create_directories(meshesDir);
        const char* names[] = {"Sphere", "Pyramid", "Cube"};
        for (const char* n : names) {
            fs::path f = meshesDir / (std::string(n) + ".mesh");
            if (!fs::exists(f)) {
                std::ofstream ofs(f.string(), std::ios::out | std::ios::trunc);
                if (ofs) {
                    ofs << "type: " << n << "\n";
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "ensureDefaultFolders error: " << e.what() << std::endl;
    }
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
        
        // Skip hidden files at any depth
        if (filename[0] == '.') continue;

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
            // Enable drag for .mesh files
            std::string ext = entry.path().extension().string();
            if (ext == ".mesh" && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                std::string name = entry.path().stem().string();
                ImGui::SetDragDropPayload("ASSET_MESH", name.c_str(), (name.size() + 1) * sizeof(char));
                ImGui::TextUnformatted(name.c_str());
                ImGui::EndDragDropSource();
            }
        }
    }
}

void ProjectPanel::render() {
    ensureDefaultFolders();
    
    ImGui::BeginChild("ProjectPanel", ImVec2(0, 0), true);
    
    if (!projectPath_.empty()) {
        // Validate folder exists
        if (!fs::exists(projectPath_) || !fs::is_directory(projectPath_)) {
            ImGui::TextDisabled("Folder not found: %s", projectPath_.c_str());
            std::string cwdStr;
            try { cwdStr = fs::current_path().string(); } catch (...) { cwdStr = "<unknown>"; }
            ImGui::TextDisabled("CWD: %s", cwdStr.c_str());
            ImGui::EndChild();
            return;
        }

        // Header: show the name of the folder
        std::string rootLabel;
        try {
            fs::path p = fs::path(projectPath_);
            rootLabel = p.filename().string();
            if (rootLabel.empty()) rootLabel = p.string();
        } catch (...) {
            rootLabel = projectPath_;
        }
        ImGui::TextUnformatted(rootLabel.c_str());
        ImGui::Separator();

        // Show only the contents (no root node)
        renderDirectoryTree(projectPath_);
    } else {
        ImGui::TextDisabled("No project path set");
    }
    
    ImGui::EndChild();
}
