 #version 330 core


#define MAX_LIGHTS 10
#define DIRECTIONAL_LIGHT 1
#define POINT_LIGHT 2
#define SPOT_LIGHT 3


in vec3 FragColor;
in vec3 FragNormal;
in vec3 FragPos;

struct light_props {
    vec3 position;
    vec3 color;
    int type;

    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    float ambientCoeff;
    float diffuseCoeff;
    float specularCoeff;
};

struct material_props {
    float ambientStrength;
    float diffuseStrength;
    float specularStrength;
};

out vec4 FragOutColor;

uniform light_props lights[MAX_LIGHTS];
uniform int numLights;
uniform material_props material;
uniform vec3 viewPos;  

vec3 ambient(light_props light) {
    vec3 amb = light.ambientCoeff * material.ambientStrength * FragColor * light.color;
    return amb;
}

vec3 diffuse(vec3 LightDir, vec3 NormalDir, light_props light) {
    float diffComponent = max(dot(LightDir, NormalDir),0.0f);
    return diffComponent * light.diffuseCoeff * material.diffuseStrength * FragColor * light.color;
}

vec3 specular(vec3 LightDir, vec3 NormalDir, vec3 ViewDir, light_props light) {
    vec3 ReflectDir = reflect(-LightDir, NormalDir);
    float specComponent = pow(max(dot(ReflectDir, ViewDir), 0.0f), 32);
    return specComponent * light.specularCoeff * material.specularStrength * light.color;
}


vec3 calcLight(vec3 LightDir, vec3 NormalDir, vec3 ViewDir, light_props light) {
    vec3 amb = ambient(light);
    vec3 diff = diffuse(LightDir, NormalDir, light);
    vec3 spec = specular(LightDir, NormalDir, ViewDir, light);
    return amb + diff + spec;
}

void main() {

 

    vec3 totalLight = vec3(0.0f);

    for (int i = 0; i < numLights; i++) {
        light_props light = lights[i];
        vec3 LightDir = normalize(light.position - FragPos);
        vec3 NormalDir = normalize(FragNormal);
        vec3 ViewDir = normalize(viewPos - FragPos);

        if (light.type == DIRECTIONAL_LIGHT) {
            totalLight += calcLight(LightDir, NormalDir, ViewDir,light);
        } else if (light.type == POINT_LIGHT) {
            float distance = length(light.position - FragPos);
            float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
            totalLight += attenuation * calcLight(LightDir, NormalDir, ViewDir,light);
        } else if (light.type == SPOT_LIGHT) {
            float distance = length(light.position - FragPos);
            float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
            float theta = dot(LightDir, normalize(-light.direction));
            float epsilon = (light.cutOff - light.outerCutOff);
            float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);
            totalLight += attenuation * intensity * calcLight(LightDir, NormalDir, ViewDir,light);
        }

    }

    FragOutColor = vec4 (totalLight, 1.0f);

}