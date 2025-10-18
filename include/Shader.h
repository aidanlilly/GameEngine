#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>

class Shader {
public:
    unsigned int ID;

    // Constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    
    // Destructor
    ~Shader();

    // Use/activate the shader
    void use() const;

    // Utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, float x, float y, float z, float w) const;
    void setMat4(const std::string& name, const float* value) const;

private:
    // Utility function for loading shader source
    std::string loadShaderSource(const char* path);
    
    // Utility function for compiling shaders
    unsigned int compileShader(unsigned int type, const char* source);
    
    // Check compilation/linking errors
    void checkCompileErrors(unsigned int shader, const std::string& type);
};

#endif
