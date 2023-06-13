
#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec3 vNormal;
layout(location = 3) in vec2 vTexCoord;
layout (location = 4) in mat4 instanceTransform;

out vec2 TexCoordOut;
out vec3 FragColor;
out vec3 FragNormal;
out vec3 FragPos;

uniform mat4 mModel;
uniform mat4 mView;
uniform mat4 mProjection;


void main(void) {
    gl_Position = mProjection * mView * mModel * instanceTransform * vec4(vPos, 1.0f);
    FragPos = vec3(mModel * instanceTransform * vec4(vPos, 1.0f));
    FragColor = vColor;
    TexCoordOut = vTexCoord;
    FragNormal = transpose(inverse(mat3(mModel * instanceTransform))) * vNormal;
    gl_PointSize = 20.0f;
}
