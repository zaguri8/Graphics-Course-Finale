#include "_graphics.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stbi_image.h"

// ---------------  Geometry functions --------------- //
namespace geometry {
    float b_spline_blend(float u, int k, int d, vector<float>& knots) {
        if (d == 0) {
            return (knots[k] <= u && u < knots[k + 1]) ? 1 : 0;
        }
        float A = 0.0f, B = 0.0f;
        float ADen = knots[k + d] - knots[k];
        float BDen = knots[k + d + 1] - knots[k + 1];
        if (ADen != 0) {
            A = ((u - knots[k]) / ADen) * b_spline_blend(u, k, d - 1, knots);
        }
        if (BDen != 0) {
            B = ((knots[k + d + 1] - u) / BDen) * b_spline_blend(u, k + 1, d - 1, knots);
        }
        if (ADen == 0 && BDen == 0) {
            return 0;
        }
        return A + B;
    }

    vector<vec3> b_spline(vector<vec3> cp, vector<float> knots, int approxM) {

        if (cp.size() + 4 != knots.size()) {
            throw std::invalid_argument("Invalid size of knot vector for the given degree and control points.");
        }

        vector<vec3> spline_points;
        for (int i = 0; i < approxM; i++) {
            float u = static_cast<float>(i) / approxM;
            vec3 point(0.0f);
            for (int k = 0; k < cp.size(); k++) {
                float B = b_spline_blend(u, k, 3, knots);
                point += B * cp[k];
            }
            spline_points.push_back(point);
        }

        return spline_points;
    }


    vector<vec3> b_spline_surface(
        vector<vector<vec3>> C,
        vector<float> knotsU,
        vector<float> knotsV,
        int degreeU, int degreeV,
        int numPointsU, int numPointsV) {
        int n = C.size();
        int m = C[0].size();
        if (n != knotsU.size() - degreeU - 1) {
            throw std::invalid_argument("Invalid size of knot vector for the given degree and control points.");
        }

        if (m != knotsV.size() - degreeV - 1) {
            throw std::invalid_argument("Invalid size of knot vector for the given degree and control points.");
        }

        vector<vec3> points;
        for (int i = 0; i < numPointsU; i++) {
            float u = i / static_cast<float>(numPointsU);
            for (int j = 0; j < numPointsV; j++) {
                float v = j / static_cast<float>(numPointsV);
                glm::vec3 p(0, 0, 0);
                for (int k = 0; k < n; k++) {
                    for (int l = 0; l < m; l++) {
                        float Bu = b_spline_blend(u, k, degreeU, knotsU);
                        float Bv = b_spline_blend(v, l, degreeV, knotsV);
                        p += Bu * Bv * C[k][l];
                    }
                }
                points.push_back(glm::vec3(p));
            }
        }
        return points;
    }

    vector<vec3> sphere(glm::vec3 center, float radius, int stacks, int slices) {
        vector<vec3> vertices;
        for (int i = 0; i <= stacks; ++i) {
            float stackAngle = M_PI / float(stacks) * i; // range from 0 to pi
            float y = radius * cosf(stackAngle); // vertical position
            float stackRadius = radius * sinf(stackAngle);

            for (int j = 0; j <= slices; ++j) {
                float sliceAngle = 2 * M_PI / float(slices) * j; // range from 0 to 2pi
                float x = stackRadius * cosf(sliceAngle); // calculate horizontal positions
                float z = stackRadius * sinf(sliceAngle);

                vec3 vertex(x + center.x, y + center.y, z + center.z); // translate to center
                vertices.push_back(vertex);
            }
        }
        return vertices;
    }

};


// --------------- Shader Program --------------- //
shader_program::shader_program() {
    // Create Program
    m_program = glCreateProgram();
    // Create Shaders
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
}
shader_program::~shader_program() {
    glDeleteProgram(m_program);
}

void shader_program::setLights(vector<light_props>& lights, material_props& materialProperties, vec3 viewDirection) {
    function<string(string, int, string)> combine = [](string s1, int s2, string  s3) {
        return s1 + to_string(s2) + s3;
    };
    setUniform("numLights", static_cast<int>(lights.size()));
    setUniform("viewPos", viewDirection);
    setUniform("material.ambientStrength", materialProperties.ambientStrength);
    setUniform("material.diffueStrength", materialProperties.diffuseStrength);
    setUniform("material.specularStrength", materialProperties.specularStrength);
    for (int i = 0; i < lights.size(); i++) {
        light_props& lightProperties = lights[i];

        setUniform(combine("lights[", i, "].position").c_str(), lightProperties.position);
        setUniform(combine("lights[", i, "].color").c_str(), lightProperties.color);
        setUniform(combine("lights[", i, "].type").c_str(), lightProperties.type);
        setUniform(combine("lights[", i, "].direction").c_str(), lightProperties.direction);
        setUniform(combine("lights[", i, "].cutOff").c_str(), lightProperties.cutOff);
        setUniform(combine("lights[", i, "].outerCutOff").c_str(), lightProperties.outerCutOff);

        setUniform(combine("lights[", i, "].constant").c_str(), lightProperties.constant);
        setUniform(combine("lights[", i, "].linear").c_str(), lightProperties.linear);
        setUniform(combine("lights[", i, "].quadratic").c_str(), lightProperties.quadratic);

        setUniform(combine("lights[", i, "].ambientCoeff").c_str(), lightProperties.ambientCoeff);
        setUniform(combine("lights[", i, "].diffuseCoeff").c_str(), lightProperties.diffuseCoeff);
        setUniform(combine("lights[", i, "].specularCoeff").c_str(), lightProperties.specularCoeff);

    }
}


void shader_program::load(const char* vertexShaderPath, const char* fragmentShaderPath) {
    // Load Vertex Shader
    loadShader(vertexShaderPath, vertexShader);
    // Load Fragment Shader
    loadShader(fragmentShaderPath, fragmentShader);

    loaded = true;
}

void shader_program::attach() {
    if(attached) return;
    // Attach Shaders
    glAttachShader(m_program, vertexShader);
    glAttachShader(m_program, fragmentShader);

    // Link Program
    glLinkProgram(m_program);
    checkShader(m_program, GL_LINK_STATUS, true, "Error linking shader program");
    // Validate Program
    glValidateProgram(m_program);
    checkShader(m_program, GL_VALIDATE_STATUS, true, "Invalid shader program");

    // Delete Shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    attached = true;
}

void shader_program::use() {
    glUseProgram(m_program);
}


void shader_program::setUniform(const char* name, const glm::mat4& value) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, value_ptr(value));
}

void shader_program::setUniform(const char* name, const glm::vec3& value) {
    glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
}
void shader_program::setUniform(const char* name, const glm::vec4& value) {
    glUniform4f(getUniformLocation(name), value.x, value.y, value.z, value.w);
}

void shader_program::setUniform(const char* name, float value) {
    glUniform1f(getUniformLocation(name), value);
}

void shader_program::setUniform(const char* name, int value) {
    glUniform1i(getUniformLocation(name), value);
}

void shader_program::setUniform(const char* name, bool value) {
    glUniform1i(getUniformLocation(name), value);
}

GLuint shader_program::getProgram() { return m_program; };
bool shader_program::isLoaded() { return loaded; }
void shader_program::loadShader(const char* path, GLuint shader) {
    std::string shaderSource;
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        file.open(path);
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        shaderSource = stream.str();
    }
    catch (std::ifstream::failure e) {
        std::cout << "Error reading shader file: " << path << std::endl;
    }

    const char* source = shaderSource.c_str();
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    checkShader(shader, GL_COMPILE_STATUS, false, "Error compiling shader");
}

void shader_program::checkShader(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage) {
    GLint success = 0;
    GLchar error[1024] = { 0 };

    if (isProgram)
        glGetProgramiv(shader, flag, &success);
    else
        glGetShaderiv(shader, flag, &success);

    if (success == GL_FALSE) {
        if (isProgram)
            glGetProgramInfoLog(shader, sizeof(error), NULL, error);
        else
            glGetShaderInfoLog(shader, sizeof(error), NULL, error);
        std::cout << errorMessage << ": '" << error << "'" << std::endl;
    }
}


GLint shader_program::getUniformLocation(const char* name) {
    GLint loc;
    if (m_uniforms.find(name) != m_uniforms.end()) {
        loc = m_uniforms[name];
    }
    else {
        loc = glGetUniformLocation(m_program, name);
        m_uniforms[name] = loc;
    }
    return loc;
};

// -------------- texture_2d ------------------ //

void texture_2d::loadTextureFromFile(const char* filename) {
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, STBI_rgb_alpha);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

}

void texture_2d::loadTextureFromData(const unsigned char* data) {
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width = 0, height = 0;

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
}

void texture_2d::use() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
}
void texture_2d::unuse() {
    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

texture_2d::texture_2d() {
    glGenTextures(1, &texture);
}
texture_2d::~texture_2d() {
    glDeleteTextures(1, &texture);
}


// --------------- geometry_buffer --------------- //

geometry_buffer::geometry_buffer(
    vector<vec3> vertices,
    vector<vec3> colors,
    vector<vec3> normals,
    vector<unsigned int> indices
) : vertices(vertices), colors(colors), normals(normals), indices(indices) {
    generateBuffers();
}

geometry_buffer::geometry_buffer() {
    generateBuffers();
}


geometry_buffer::~geometry_buffer() {
    deleteBuffers();
}

instanced_geometry_buffer::~instanced_geometry_buffer() {
    instanced_geometry_buffer::deleteBuffers();
}

void geometry_buffer::setDrawPatterns(vector<DrawPattern> drawPatterns) {
    this->drawPatterns = drawPatterns;
}

void geometry_buffer::setVertices(vector<vec3>& vertices) {
    this->vertices = vertices;
}

void geometry_buffer::setColors(vector<vec3>& colors) {
    this->colors = colors;
}

void geometry_buffer::setNormals(vector<vec3>& normals) {
    this->normals = normals;
}

void geometry_buffer::setIndices(vector<unsigned int>& indices) {
    this->indices = indices;
}

void geometry_buffer::setShaderProgram(shader_program* sp) { this->sp = sp; }

void geometry_buffer::updateBuffers() {
    updateVerticesBuffer();
    updateColorsBuffer();
    updateNormalsBuffer();
    updateIndicesBuffer();
    // unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void geometry_buffer::draw() {
    bindVertexArray();
    for (auto drawPattern : drawPatterns) {
        glDrawElements(drawPattern.drawMode, drawPattern.count, GL_UNSIGNED_INT, (void*)(drawPattern.start * sizeof(unsigned int)));
    }
}

void geometry_buffer::updateVerticesBuffer() {
    bindVertexArray();
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), vertices.data(), GL_STATIC_DRAW);

    if (sp) {
        GLuint vPosLoc = glGetAttribLocation(sp->getProgram(), "vPos");
        glVertexAttribPointer(vPosLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(vPosLoc);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void geometry_buffer::updateColorsBuffer() {
    bindVertexArray();
    glBindBuffer(GL_ARRAY_BUFFER, cbo);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), colors.data(), GL_STATIC_DRAW);
    if (sp) {
        GLuint vColorLoc = glGetAttribLocation(sp->getProgram(), "vColor");
        glVertexAttribPointer(vColorLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(vColorLoc);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void geometry_buffer::updateNormalsBuffer() {
    bindVertexArray();
    glBindBuffer(GL_ARRAY_BUFFER, nbo);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), normals.data(), GL_STATIC_DRAW);
    if (sp) {
        GLuint vNormalLoc = glGetAttribLocation(sp->getProgram(), "vNormal");
        glVertexAttribPointer(vNormalLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(vNormalLoc);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void geometry_buffer::updateIndicesBuffer() {
    bindVertexArray();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void geometry_buffer::generateBuffers() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &cbo);
    glGenBuffers(1, &nbo);
    glGenBuffers(1, &ebo);
}

void geometry_buffer::deleteBuffers() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &cbo);
    glDeleteBuffers(1, &nbo);
    glDeleteBuffers(1, &ebo);
}

void geometry_buffer::bindVertexArray() {
    glBindVertexArray(vao);
}


void instanced_geometry_buffer::setTransformations(vector<mat4>& matrices) {
    this->matrices = matrices;
}

void instanced_geometry_buffer::updateBuffers() {
    geometry_buffer::updateBuffers();
    bindVertexArray();
    updateMatricesBuffers();
    glBindVertexArray(0);
}

void instanced_geometry_buffer::generateBuffers() {
    geometry_buffer::generateBuffers();
    glGenBuffers(1, &mbo);
}

void instanced_geometry_buffer::deleteBuffers() {
    geometry_buffer::deleteBuffers();
    glDeleteBuffers(1, &mbo);
}


void instanced_geometry_buffer::draw() {
    bindVertexArray();

    for (auto drawPattern : drawPatterns) {
        glDrawElementsInstanced(drawPattern.drawMode, drawPattern.count, GL_UNSIGNED_INT, (void*)(drawPattern.start * sizeof(unsigned int)), matrices.size());
    }
}

void instanced_geometry_buffer::updateMatricesBuffers() {
    geometry_buffer::bindVertexArray();
    // bind matrices buffer
    glBindBuffer(GL_ARRAY_BUFFER, mbo);
    glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(mat4), matrices.data(), GL_STATIC_DRAW);
    GLuint vInstanceLoc = glGetAttribLocation(sp->getProgram(), "instanceTransform");
    // attribute pointers for matrix (4 times vec4)
    for (int i = 0; i < 4; i++) {
        // matrix attribute
        glEnableVertexAttribArray(vInstanceLoc + i);
        glVertexAttribPointer(vInstanceLoc + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(vec4) * i));
        glVertexAttribDivisor(vInstanceLoc + i, 1);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void instanced_geometry_buffer::updatePartialMatrices(int start, int end) {
    geometry_buffer::bindVertexArray();
    // bind matrices buffer
    glBindBuffer(GL_ARRAY_BUFFER, mbo);
    glBufferSubData(GL_ARRAY_BUFFER, start * sizeof(mat4), (end - start) * sizeof(mat4), matrices.data() + start);
}

void instanced_geometry_buffer::updatePartialMatrices(vector<pair<int, int>> ranges) {
    geometry_buffer::bindVertexArray();
    // bind matrices buffer
    glBindBuffer(GL_ARRAY_BUFFER, mbo);
    for (auto range : ranges) {
        glBufferSubData(GL_ARRAY_BUFFER, range.first * sizeof(mat4), (range.second - range.first) * sizeof(mat4), matrices.data() + range.first);
    }
}

instanced_geometry_buffer::instanced_geometry_buffer() : geometry_buffer() {
    generateBuffers();
};

textured_geometry_buffer::textured_geometry_buffer() : instanced_geometry_buffer() {
    generateBuffers();
}
void textured_geometry_buffer::generateBuffers() {
    instanced_geometry_buffer::generateBuffers();
    glGenBuffers(1, &tbo);
}
void textured_geometry_buffer::updateBuffers() {
    instanced_geometry_buffer::updateBuffers();
    updateTextureCoordinatesBuffer();
    glBindVertexArray(0);
}

void textured_geometry_buffer::setTextureCoodinates(vector<vec2>& tex_coords) {
    this->tex_coords = tex_coords;
}
void textured_geometry_buffer::loadTextureFromFile(const char* texture_path) {
    texture.loadTextureFromFile(texture_path);
}

void textured_geometry_buffer::loadTextureFromData(const unsigned char* data) {
    texture.loadTextureFromData(data);
}

void textured_geometry_buffer::updateTextureCoordinatesBuffer() {
    bindVertexArray();
    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glBufferData(GL_ARRAY_BUFFER, tex_coords.size() * sizeof(vec2), tex_coords.data(), GL_STATIC_DRAW);
    if (sp) {
        GLuint vTexCoordLoc = glGetAttribLocation(sp->getProgram(), "vTexCoord");
        glVertexAttribPointer(vTexCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(vTexCoordLoc);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}


void textured_geometry_buffer::deleteBuffers() {
    instanced_geometry_buffer::deleteBuffers();
    glDeleteBuffers(1, &tbo);
}


instanced_geometry_buffer* light_props::createSource(light_props& props, shader_program* sp) {
    instanced_geometry_buffer* source_gb = new instanced_geometry_buffer();
    vector<vec3> vertices = geometry::sphere(props.position, 0.08, 400, 400);
    vector<vec3> colors(vertices.size(), props.color);
    vector<vec3> normals;
    vector<unsigned int> indices;
    vector<mat4> transforms{mat4(1.0f)};
    for (int i = 0; i < vertices.size(); i++) {
        vec3& pointOnSphere = vertices[i];
        normals.push_back(normalize(pointOnSphere));
        indices.push_back(i);
    }
    source_gb->setVertices(vertices);
    source_gb->setColors(colors);
    source_gb->setNormals(normals);
    source_gb->setIndices(indices);
    source_gb->setTransformations(transforms);
    source_gb->setShaderProgram(sp);
    source_gb->updateBuffers();

    return source_gb;
}




scene_obj::scene_obj(geometry_buffer* gb) : gb((gb)) {}
scene_obj::scene_obj() : gb(new geometry_buffer()) {}
scene_obj::~scene_obj() { if (boundingBox) delete boundingBox;  if (gb) delete gb; }

bounding_box* scene_obj::b_box(vector<vec3> points) {
    bounding_box* b = new bounding_box();
    b->xMin = b->yMin = b->zMin = 1000000;
    b->xMax = b->yMax = b->zMax = -1000000;
    for (int i = 0; i < points.size(); i++) {
        b->xMin = min(b->xMin, points[i].x);
        b->yMin = min(b->yMin, points[i].y);
        b->zMin = min(b->zMin, points[i].z);

        b->xMax = max(b->xMax, points[i].x);
        b->yMax = max(b->yMax, points[i].y);
        b->zMax = max(b->zMax, points[i].z);
    }
    return b;
}

bool bounding_box::contains(vec3 point) {
    return
        xMin <= point.x && point.x <= xMax &&
        yMin <= point.y && point.y <= yMax &&
        zMin <= point.z && point.z <= zMax;
}

void scene_obj::setGeometryBuffer(geometry_buffer* gb) {
    this->gb = gb;
    this->boundingBox = b_box(gb->vertices);
}

const mat4& scene_obj::getModel() const {
    return model;
}


void scene_obj::rotate(float angle, vec3 axis, vec3 wrt) {
    setModel(
        getModel() * toMat4(angleAxis(angle, vec3(axis.x, axis.y, axis.z)))
    );
}

void scene_obj::translate(vec3 translation) {
    setModel(
        getModel() * glm::translate(mat4(1.0f), vec3(translation.x, translation.y, translation.z))
    );
}

void scene_obj::scale(vec3 scale) {
    setModel(
        getModel() * glm::scale(mat4(1.0f), vec3(scale.x, scale.y, scale.z))
    );
}

void scene_obj::setModel(mat4 model) {
    this->model = model;
}

void scene_obj::draw() {
    if (gb->sp) {
        gb->sp->use();
        gb->sp->setUniform("material.ambientStrength", getMaterialProperties().ambientStrength);
        gb->sp->setUniform("material.diffuseStrength", getMaterialProperties().diffuseStrength);
        gb->sp->setUniform("material.specularStrength", getMaterialProperties().specularStrength);
        gb->sp->setUniform("mModel", getModel());

        if (static_cast<textured_geometry_buffer*>(gb)) {
            textured_geometry_buffer* tgb = static_cast<textured_geometry_buffer*>(gb);
            tgb->texture.use();
            gb->sp->setUniform("textureSampler", 0);
        }
    }
    gb->draw();
    glUseProgram(0);
    if (static_cast<textured_geometry_buffer*>(gb)) {
        textured_geometry_buffer* tgb = static_cast<textured_geometry_buffer*>(gb);
        tgb->texture.unuse();
    }
}


void scene_obj::setMaterialProperties(material_props& materialProperties) {
    this->materialProperties = materialProperties;
}

const material_props& scene_obj::getMaterialProperties() const {
    return materialProperties;
}

inline float min(float a, float b) {
    return a < b ? a : b;
}
inline float max(float a, float b) {
    return a > b ? a : b;
}


