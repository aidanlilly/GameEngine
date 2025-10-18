#ifndef PROJECT_PANEL_H
#define PROJECT_PANEL_H

#include <string>

class ProjectPanel {
public:
    ProjectPanel();
    void render();
    void setProjectPath(const std::string& path);

private:
    void renderDirectoryTree(const std::string& path, int depth = 0);
    std::string projectPath_;
};

#endif
