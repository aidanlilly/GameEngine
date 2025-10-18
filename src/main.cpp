#define GLEW_STATIC
#include <GL/glew.h>
// Keep main tiny: just run the Application
#include "Application.h"
int main() {
    Application app;
    return app.run();
}
#include "ui/InspectorPanel.h"
