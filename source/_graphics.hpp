#ifndef _GRAPHICS
#define _GRAPHICS
#define GL_SILENCE_DEPRECATION
#include <iostream>
#include <OpenGL/gl3.h>
#include <fstream>
#include <sstream>
#include <GLFW/glfw3.h>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
using namespace std;
using namespace glm;

#define DIRECTIONAL_LIGHT 1
#define POINT_LIGHT 2
#define SPOT_LIGHT 3

inline float min(float a, float b);
inline float max(float a, float b);

/**
 * @brief Structure representing the properties of a light source.
 */
struct light_props {
    int type;
    vec3 position;
    vec3 color;

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    float ambientCoeff;
    float diffuseCoeff;
    float specularCoeff;

    float cutOff = 0;
    float outerCutOff = 0;
    vec3 direction = vec3(0, 0, 0);

    light_props(
        int type,
        vec3 position,
        vec3 color,
        float ambientCoeff,
        float diffuseCoeff,
        float specularCoeff
    ) : type(type),
        position(position),
        color(color),
        ambientCoeff(ambientCoeff),
        diffuseCoeff(diffuseCoeff),
        specularCoeff(specularCoeff) { }

    static class instanced_geometry_buffer* createSource(light_props& props, class shader_program* sp);
};

/**
 * @brief Structure representing the material light properties.
 */
struct material_props {
    float ambientStrength = 1.0f;   /**< Strength of ambient lighting for the material. */
    float diffuseStrength = 1.0f;   /**< Strength of diffuse lighting for the material. */
    float specularStrength = 1.0f;  /**< Strength of specular lighting for the material. */
    material_props() {}
    material_props(float ambientStrength, float diffuseStrength, float specularStrength) :
        ambientStrength(ambientStrength),
        diffuseStrength(diffuseStrength),
        specularStrength(specularStrength) { }
};


namespace geometry {
    /**
        @brief Calculates the blending factor for a given parameter 'u' based on the B-spline basis functions.
        @param u The parameter value for which the blending factor is calculated.
        @param k The index of the current knot span.
        @param d The degree of the B-spline basis functions.
        @param knots The knot vector defining the B-spline basis functions.
        @return The blending factor for the given parameter value 'u'.
    **/
    float b_spline_blend(float u, int k, int d, vector<float>& knots);
    vector<vec3> b_spline(vector<vec3> cp, vector<float> knots, int approxM);


    /**
    @brief Calculates a B-spline surface based on the given control points, knot vectors, degrees, and number of points.
    @param C A 2D vector representing the control points of the B-spline surface.
    @param knotsU The knot vector for the U direction of the B-spline surface.
    @param knotsV The knot vector for the V direction of the B-spline surface.
    @param degreeU The degree of the B-spline basis functions in the U direction.
    @param degreeV The degree of the B-spline basis functions in the V direction.
    @param numPointsU The number of points to be calculated in the U direction.
    @param numPointsV The number of points to be calculated in the V direction.
    @return A 2D vector representing the calculated B-spline surface points.
    **/
    vector<vec3> b_spline_surface(
        vector<vector<vec3>> C,
        vector<float> knotsU,
        vector<float> knotsV,
        int degreeU, int degreeV,
        int numPointsU, int numPointsV);


    vector<vec3> sphere(glm::vec3 center, float radius, int stacks, int slices);
};


/**
 * @brief A class representing a shader program
 */
class shader_program {
public:
    shader_program();
    ~shader_program();

    /**
     * @brief Sets the light and material properties for the shader program.
     *
     * @param lightProperties Light properties struct containing the position, color, and coefficients of the light source.
     * @param materialProperties Material light properties struct containing the ambient, diffuse, and specular strengths.
     * @param viewDirection Optional parameter for the direction of the viewer.
     */
    void setLights(vector<light_props>& lights, material_props& materialProperties, vec3 viewDirection = vec3(0, 0, -1));


    /**
     * @brief Loads the vertex and fragment shaders, attaches them to the shader program, links and validates the program.
     *
     * @param vertexShaderPath Path to the vertex shader file.
     * @param fragmentShaderPath Path to the fragment shader file.
     */
    void load(const char* vertexShaderPath, const char* fragmentShaderPath);
    void use();

    void attach();


    /**
     * @brief Sets a uniform variable of type mat4 in the shader program.
     *
     * @param name Name of the uniform variable.
     * @param value glm::mat4 value to be set.
     */
    void setUniform(const char* name, const glm::mat4& value);

    /**
     * @brief Sets a uniform variable of type vec3 in the shader program.
     *
     * @param name Name of the uniform variable.
     * @param value glm::vec3 value to be set.
     */
    void setUniform(const char* name, const glm::vec3& value);
    /**
    * @brief Sets a uniform variable of type vec4 in the shader program.
    *
    * @param name Name of the uniform variable.
    * @param value glm::vec4 value to be set.
    */
    void setUniform(const char* name, const glm::vec4& value);
    /**
     * @brief Sets a uniform variable of type float in the shader program.
     *
     * @param name Name of the uniform variable.
     * @param value float value to be set.
     */
    void setUniform(const char* name, float value);
    /**
     * @brief Sets a uniform variable of type int in the shader program.
     *
     * @param name Name of the uniform variable.
     * @param value int value to be set.
     */
    void setUniform(const char* name, int value);
    /**
     * @brief Sets a uniform variable of type bool in the shader program.
     *
     * @param name Name of the uniform variable.
     * @param value bool value to be set.
     */
    void setUniform(const char* name, bool value);
    /**
     * @brief Returns the ID of the shader program.
     *
     * @return GLuint ID of the shader program.
     */
    GLuint getProgram();
    /**
     * @brief Checks if the shader program is loaded and ready to be used.
     *
     * @return bool True if the shader program is loaded, false otherwise.
     */
    bool isLoaded();

private:
    bool loaded;
    bool attached;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint m_program;
    std::map<std::string, GLint> m_uniforms;


    /**
     * @brief Loads the shader source from a file and compiles the shader.
     *
     * @param path Path to the shader source file.
     * @param shader ID of the shader object.
     */
    void loadShader(const char* path, GLuint shader);
    /**
     * @brief Checks the compile or link status of the shader program or shader object and prints an error message if needed.
     *
     * @param shader ID of the shader program or shader object.
     * @param flag Compile or link status flag.
     * @param isProgram Flag indicating if the object is a shader program or shader object.
     * @param errorMessage Error message to be printed.
     */
    void checkShader(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);

    /**
     * @brief Retrieves the location of a uniform variable in the shader program.
     * If the location is not already cached, it queries and caches the location.
     *
     * @param name Name of the uniform variable.
     * @return GLint Location of the uniform variable.
     */
    GLint getUniformLocation(const char* name);
};


struct texture_2d {
    texture_2d();
    ~texture_2d();
    void loadTextureFromFile(const char* filename);
    void loadTextureFromData(const unsigned char* data);
    void use();
    void unuse();

    GLuint texture;
};


struct DrawPattern {
    DrawPattern() {}
    DrawPattern(GLenum drawMode, size_t start, size_t count) :
        drawMode(drawMode),
        start(start),
        count(count) {}

    GLenum drawMode;
    size_t start;
    size_t count;
};

/**
 * @brief A structure representing a geometry buffer containing vertex, color, normal, and index data.
 */
struct geometry_buffer {

    geometry_buffer(
        vector<vec3> vertices,
        vector<vec3> colors,
        vector<vec3> normals,
        vector<unsigned int> indices
    );
    geometry_buffer();
    virtual ~geometry_buffer();
    /**
     * @brief Sets the vertex data of the geometry buffer.
     *
     * @param vertices Vector of vec3 representing the vertex positions.
     */
    void setVertices(vector<vec3>& vertices);
    /**
    * @brief Sets the color data of the geometry buffer.
    *
    * @param colors Vector of vec3 representing the vertex colors.
    */
    void setColors(vector<vec3>& colors);
    /**
    * @brief Sets the normal data of the geometry buffer.
    *
    * @param normals Vector of vec3 representing the vertex normals.
    */
    void setNormals(vector<vec3>& normals);
    /**
    * @brief Sets the index data of the geometry buffer.
    *
    * @param indices Vector of unsigned integers representing the vertex indices.
    */
    void setIndices(vector<unsigned int>& indices);

    void setShaderProgram(shader_program* sp);
    /**
    * @brief Updates the vertex, color, normal, and index buffers of the geometry buffer.
    */
    virtual void updateBuffers();

    void setDrawPatterns(vector<DrawPattern> drawPatterns);
    vector<vec3> vertices;
    vector<vec3> colors;
    vector<vec3> normals;
    vector<unsigned int> indices;
    virtual void draw();

public:
    GLuint vao, vbo, cbo, nbo, ebo;
    shader_program* sp;
    vector<DrawPattern> drawPatterns;
    /**
     * @brief Updates the vertex buffer with the current vertex data.
     */
    void updateVerticesBuffer();
    /**
     * @brief Updates the color buffer with the current color data.
     */
    void updateColorsBuffer();
    /**
     * @brief Updates the normal buffer with the current normal data.
     */
    void updateNormalsBuffer();
    /**
     * @brief Updates the index buffer with the current index data.
     */
    void updateIndicesBuffer();
    /**
     * @brief Generates the necessary OpenGL buffers (VAO, VBOs, EBO).
     */
    virtual void generateBuffers();
    /**
     * @brief Deletes the OpenGL buffers (VAO, VBOs, EBO) associated with the geometry buffer.
     */
    virtual void deleteBuffers();
    /**
     * @brief Binds the Vertex Array Object (VAO) for rendering.
     */
    void bindVertexArray();
};


class instanced_geometry_buffer : public geometry_buffer {
    using geometry_buffer::geometry_buffer;
public:
    instanced_geometry_buffer();
    virtual ~instanced_geometry_buffer();

    /**
     * @brief Sets the transformation matrices for each instance in the buffer.
     *
     * This method is used to update the position, rotation, and scale of each instance
     * of the geometry stored in this buffer. It is typically called once per frame, or
     * whenever the transformations of the instances change.
     *
     * The input to this method could be an array or vector of transformation matrices,
     * where each matrix corresponds to one instance of the geometry. The matrices should
     * be in the same order as the instances in the buffer.
     *
     * @param transformations The new transformation matrices for the instances.
     */
    void setTransformations(vector<mat4>& matrices);

    // overrride generateBuffers() to generate the model matrix buffer
    virtual void generateBuffers() override;
    // overrride deleteBuffers() to delete the model matrix buffer
    virtual void deleteBuffers() override;
    // override updateBuffers() to update the model matrix buffer
    virtual void updateBuffers() override;
    // override draw() to draw the geometry using instanced rendering
    virtual void draw() override;

    void updatePartialMatrices(int start, int end);
    void updatePartialMatrices(vector<pair<int, int>> ranges);

protected:
    GLuint mbo;
    vector<mat4> matrices;
    void updateMatricesBuffers();
};


class textured_geometry_buffer : public instanced_geometry_buffer {
    using instanced_geometry_buffer::instanced_geometry_buffer;
public:
    textured_geometry_buffer();

    void updateTextureCoordinatesBuffer();
    void generateBuffers() override;
    void deleteBuffers() override;
    void updateBuffers() override;

    void setTextureCoodinates(vector<vec2>& texCoords);
    void loadTextureFromFile(const char* texture_path);
    void loadTextureFromData(const unsigned char* data);

public:
    texture_2d texture;
    GLuint tbo;
    vector<vec2> tex_coords;
};

struct bounding_box {
    bounding_box() {}
    float xMin, yMin, zMin,
        xMax, yMax, zMax;
    bool contains(vec3 point);
};


class scene_obj {
public:
    scene_obj(geometry_buffer* gb);
    scene_obj();

    virtual ~scene_obj();
    void setGeometryBuffer(geometry_buffer* gb);

    const mat4& getModel() const;

    void rotate(float angle, vec3 axis, vec3 wrt = vec3(0, 0, 0));
    void translate(vec3 translation);
    void scale(vec3 scale);
    void setModel(mat4 model);
    void setMaterialProperties(material_props& materialProperties);

    const material_props& getMaterialProperties() const;

    virtual void draw();
    geometry_buffer* gb;
    bounding_box* boundingBox;
    mat4 model = mat4(1.0f);
    material_props materialProperties;

    static bounding_box* b_box(vector<vec3> points);
};


#endif

