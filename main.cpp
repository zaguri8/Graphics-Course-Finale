#define GL_SILENCE_DEPRECATION
#include "_graphics.hpp"
#include "_camera.hpp"
#include <glm/gtx/quaternion.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


using namespace std;
using namespace glm;

int windowWidth = 1280, windowHeight = 720;
Camera camera(windowWidth / static_cast<float>(windowHeight));


const char* VERTEX_SHADER_PATH = "shaders/vertex_shader_instanced.glsl";
const char* FRAGMENT_SHADER_PATH = "shaders/fragment_shader.glsl";

const char* VERTEX_SHADER_PATH_TEXTURED = "shaders/vertex_shader_textured.glsl";
const char* FRAGMENT_SHADER_PATH_TEXTURED = "shaders/fragment_shader_textured.glsl";



/**
 * @brief Updates the width and height of the window when its size changes.
 *
 * @param window     Pointer to the GLFW window.
 * @param newWidth   The new width of the window.
 * @param newHeight  The new height of the window.
 */
void frameSizeChange(GLFWwindow* window, int newWidth, int newHeight) {
    windowWidth = newWidth;
    windowHeight = newHeight;
    // Camera Processing
    camera.setAspect(windowWidth / static_cast<float>(windowHeight));
}

/**
 * @brief Creates a GLFW window.
 *
 * @return GLFWwindow* Pointer to the GLFW window.
 */
GLFWwindow* make_win() {
    if (!glfwInit()) {
        cout << "There was an issue loading glfw.." << endl;
        return NULL;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow* pWindowHandle = glfwCreateWindow(windowWidth, windowHeight, "Final Project", nullptr, nullptr);

    if (!pWindowHandle) {
        cout << "There was an issue initializing glfw window!" << endl;
        return NULL;
    }


    return pWindowHandle;
}

void gl_enable() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_2D);
}


void initMenu(GLFWwindow* pWindowHandle) {
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("fonts/Inter.ttf", 16.0f);

    // Setup ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(pWindowHandle, true);
    ImGui_ImplOpenGL3_Init("#version 150");

}

void showMenu() {
    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    ImGui::SetNextWindowSize(ImVec2(glm::max(windowWidth / 10, 100), glm::max(windowHeight / 10, 100)));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    if (ImGui::Begin("Options Menu", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
        if (ImGui::BeginMenu("Camera")) {
            if (ImGui::MenuItem("Cow POV", "Ctrl+P")) {
                //
            }
            ImGui::EndMenu();
        }
        ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

vector<unsigned int> getIndices(vector<vec3>& vertices) {
    const size_t n = vertices.size();
    vector<unsigned int> indices;
    for (int i = 0; i < n; i++)
        indices.push_back(i);
    return indices;
}


class spline : public scene_obj {
public:
    spline(
        shader_program* shader,
        array<vec3, 4> controls,
        vector<mat4> instances = { mat4(1.0f) },
        vec3 color = { 0,0,0 }
    ) {
        this->gb = new instanced_geometry_buffer();
        instanced_geometry_buffer& buffer = *static_cast<instanced_geometry_buffer*>(this->gb);
        vector<float> knots { 0, 0, 0, 0, 1, 1, 1, 1};
        vector<vec3> vertices = geometry::b_spline({ controls[0], controls[1], controls[2], controls[3] }, knots, 100);
        vector<unsigned int> indices = getIndices(vertices);
        vector<vec3> colors = { indices.size(), color };
        vector<vec3> normals = { indices.size(), normalize(vec3(0, 1, 1)) };
        buffer.setVertices(vertices);
        buffer.setIndices(indices);
        buffer.setColors(colors);
        buffer.setNormals(normals);
        buffer.setTransformations(instances);
        buffer.setDrawPatterns(vector<DrawPattern> { { GL_LINE_STRIP, /* start */ 0, /* count */ indices.size()} });
        buffer.setShaderProgram(shader);
        buffer.bindVertexArray();
        shader->attach();
        buffer.updateBuffers();
    }
};


class plane : public scene_obj {
public:
    plane(
        shader_program* shader,
        array<float, 4> bounds,
        vector<mat4> instances = { mat4(1.0f) },
        vec3 color = { 0,0,0 }
    ) {
        this->gb = new instanced_geometry_buffer();
        instanced_geometry_buffer& buffer = *static_cast<instanced_geometry_buffer*>(this->gb);
        vector<vec3> vertices;
        // bounds : { x_min, x_max, z_min, z_max }
        // draw as 2 triangles
        vertices.push_back(vec3(bounds[0], 0, bounds[2]));
        vertices.push_back(vec3(bounds[0], 0, bounds[3]));
        vertices.push_back(vec3(bounds[1], 0, bounds[3]));

        vertices.push_back(vec3(bounds[0], 0, bounds[2]));
        vertices.push_back(vec3(bounds[1], 0, bounds[3]));
        vertices.push_back(vec3(bounds[1], 0, bounds[2]));

        vector<unsigned int> indices = getIndices(vertices);
        vector<vec3> colors = { indices.size(), color };
        vector<vec3> normals = { indices.size(), normalize(vec3(0, 1, 0)) };
        buffer.setVertices(vertices);
        buffer.setIndices(indices);
        buffer.setColors(colors);
        buffer.setNormals(normals);
        buffer.setTransformations(instances);
        buffer.setDrawPatterns(vector<DrawPattern> { { GL_TRIANGLES, /* start */ 0, /* count */ indices.size()} });
        buffer.setShaderProgram(shader);
        buffer.bindVertexArray();
        shader->attach();
        buffer.updateBuffers();
    }
};


class halton_seq {
public:
    halton_seq(int base) : base(base), n(0), d(1) {}
    float next() {
        int x = d - n;
        if (x == 1) {
            n = 1;
            d *= base;
        }
        else {
            int y = d / base;
            while (x <= y)
                y /= base;
            n = (base + 1) * y - x;
        }
        return n / static_cast<float>(d);
    }
private:
    int base;
    int n;
    int d;
};

class halton_2d {
public:
    halton_2d() : h1(halton_seq(3)), h2(halton_seq(2)) { }
    vec2 next() {
        return { h1.next(),h2.next() };
    }
    halton_seq h1, h2;
};

int main() {

    GLFWwindow* pWindowHandle = make_win();
    glfwMakeContextCurrent(pWindowHandle);
    glfwSetFramebufferSizeCallback(pWindowHandle, frameSizeChange);
    initMenu(pWindowHandle);
    glClearColor(135 / 255.0f, 206 / 255.0f, 235 / 255.0f, 1.0f);

    shader_program shader;
    shader_program shader2;

    shader_program shader_textured;

    shader_textured.load(VERTEX_SHADER_PATH_TEXTURED, FRAGMENT_SHADER_PATH_TEXTURED);
    shader2.load(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
    shader.load(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);

    light_props light_scene{ DIRECTIONAL_LIGHT, vec3(0 ,10 ,-5), vec3(1,1,1), 0.8f, 1.0, 0.5 };

    material_props material{ 1, 1 ,1 };
    vector<light_props> lights {light_scene};



    vector<mat4> instances;
    halton_2d H;
    for (int i = 0; i < 50000; i++) {
        vec2 Next = H.next();
        instances.push_back(
            translate(mat4(1.0f), 5.0f * vec3(-Next[0], 0, -Next[1]))
        );
        instances.push_back(
            translate(mat4(1.0f), 5.0f * vec3(Next[0], 0, -Next[1]))
        );
    }
    spline spline_grass{
         &shader,
         /* controls*/
        { vec3(0,-1,-3),vec3(-0.05,-0.95,-3),vec3(0.1,-0.85,-3), vec3(0,-0.75, -3)  },
        /* instances*/
        instances,
        /* color */
        {0,0.4,0}
    };

    plane floor{
        &shader,
        /* bounds */
        { -100, 100, -100, 100 },
        /* instances */
        { translate(mat4(1.0f),{0,-1,0}) },
        /* color */
        { 0.5, 0.5, 0.5 }
    };




    gl_enable();

    camera.setPosition({ 0, 3, 3});
    camera.setPitch(-22.0f);
    while (!glfwWindowShouldClose(pWindowHandle)) {
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();

        // Process camera movement
        camera.processMovement(pWindowHandle);

        // Render the scene
        shader.use();
        shader.setLights(lights, material, camera.getFront());
        shader.setUniform("mView", camera.getViewMatrix());
        shader.setUniform("mProjection", camera.getProjectionMatrix());
        floor.draw();
        glDisable(GL_DEPTH_TEST);
        spline_grass.draw();



        showMenu();

        glfwSwapBuffers(pWindowHandle);
    }


    glfwTerminate();
    return 0;
}