#ifndef PROJECT_PANEL_H
#define PROJECT_PANEL_H

#include <string>

class ProjectPanel {
public:
    ProjectPanel();
    // Set the root path for the project this panel should display
    void setProjectPath(const std::string& path);
    void render();

private:
    // Ensure default folders exist under the project path (runs once)
    void ensureDefaultFolders();
    void renderDirectoryTree(const std::string& path, int depth = 0);
    std::string projectPath_;
    bool initialized_ = false;
};

#endif
