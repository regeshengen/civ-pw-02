#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

static std::string readFile(const char* path) {
    std::ifstream in(path, std::ios::in | std::ios::binary);
    if (!in) {
        std::cerr << "ERROR: could not open file: " << path << std::endl;
        return "";
    }
    std::ostringstream contents;
    contents << in.rdbuf();
    return contents.str();
}

static GLuint compileShader(const char* source, GLenum type, const char* name = "") {
    GLuint shader = glCreateShader(type);
    const char* src = source;
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (ok == GL_FALSE) {
        GLint len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(len+1);
        glGetShaderInfoLog(shader, len, nullptr, log.data());
        std::cerr << "ERROR compiling shader " << name << ":\n" << log.data() << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

static GLuint createProgramFromFiles(const char* vertPath, const char* fragPath) {
    std::string vsrc = readFile(vertPath);
    std::string fsrc = readFile(fragPath);
    if (vsrc.empty() || fsrc.empty()) return 0;

    GLuint v = compileShader(vsrc.c_str(), GL_VERTEX_SHADER, vertPath);
    GLuint f = compileShader(fsrc.c_str(), GL_FRAGMENT_SHADER, fragPath);
    if (!v || !f) return 0;

    GLuint prog = glCreateProgram();
    glAttachShader(prog, v);
    glAttachShader(prog, f);
    glLinkProgram(prog);

    GLint ok = GL_FALSE;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (ok == GL_FALSE) {
        GLint len = 0;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(len+1);
        glGetProgramInfoLog(prog, len, nullptr, log.data());
        std::cerr << "ERROR linking program:\n" << log.data() << std::endl;
        glDeleteProgram(prog);
        glDeleteShader(v);
        glDeleteShader(f);
        return 0;
    }

    glDetachShader(prog, v);
    glDetachShader(prog, f);
    glDeleteShader(v);
    glDeleteShader(f);
    return prog;
}

int main() {
    if (!glfwInit()) {
        std::cerr << "ERROR: GLFW init failed\n";
        return -1;
    }

    // Request OpenGL 4.1 Core (macOS)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // macOS
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const int WIDTH = 800, HEIGHT = 600;
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Fragment Raytracer", nullptr, nullptr);
    if (!window) {
        std::cerr << "ERROR: failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum gerr = glewInit();
    if (gerr != GLEW_OK) {
        std::cerr << "ERROR: GLEW init failed: " << glewGetErrorString(gerr) << std::endl;
        return -1;
    }

    // Create fullscreen triangle (3 vertices)
    float verts[] = {
        -1.0f, -1.0f,
         3.0f, -1.0f,
        -1.0f,  3.0f
    };

    GLuint vao = 0, vbo = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // attribute 0: vec2 position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // load/compile/link shaders
    GLuint program = createProgramFromFiles("shaders/fullscreen.vert", "shaders/raytrace.frag");
    if (!program) {
        std::cerr << "ERROR: failed to create shader program\n";
        return -1;
    }

    glUseProgram(program);
    GLint locResolution = glGetUniformLocation(program, "uResolution");
    GLint locTime = glGetUniformLocation(program, "uTime");
    GLint locCamPos = glGetUniformLocation(program, "uCameraPos");
    GLint locCamTarget = glGetUniformLocation(program, "uCameraTarget");

    // simple camera
    float camPos[3] = {0.0f, 0.5f, 2.0f};
    float camTarget[3] = {0.0f, 0.0f, -1.0f};

    double t0 = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);

        float time = float(glfwGetTime() - t0);

        glUseProgram(program);
        glUniform2f(locResolution, (float)w, (float)h);
        glUniform1f(locTime, time);
        glUniform3fv(locCamPos, 1, camPos);
        glUniform3fv(locCamTarget, 1, camTarget);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(program);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    glfwTerminate();
    return 0;
}
