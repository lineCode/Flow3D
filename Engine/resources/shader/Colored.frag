#version 330 core

uniform vec4 color;
out vec4 FragColor;

struct Material {
    bool hasSpecularTexture;
    vec3 specularVector;
    float shininess;
}; 

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define MAX_NR_POINT_LIGHTS 100
#define MAX_NR_SPOT_LIGHTS 25

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_NR_POINT_LIGHTS];
uniform int numberOfPointLights;
uniform SpotLight spotLights[MAX_NR_SPOT_LIGHTS];
uniform int numberOfSpotLights;
uniform Material material;

// function prototypes
vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	// properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================
    // phase 1: directional lighting
    vec4 result = CalcDirLight(dirLight, norm, viewDir);
    // phase 2: point lights
    for(int i = 0; i < numberOfPointLights; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    // phase 3: spot light
    for(int i = 0; i < numberOfSpotLights; i++)
        result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir); 

    if (result.a == 0)
        discard;
    FragColor = vec4(result);
}

// calculates the color when using a directional light.
vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec4 ambient = vec4(light.ambient, 1.0) * color;
    vec4 diffuse = vec4(light.diffuse, 1.0) * diff * color;
    vec4 specular;
    if (material.hasSpecularTexture)
        specular = vec4(light.specular, 1.0) * spec * color;
    else
        specular = vec4(light.specular, 1.0) * (spec * vec4(material.specularVector, 0.0f));

    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec4 ambient = vec4(light.ambient, 1.0) * color;
    vec4 diffuse = vec4(light.diffuse, 1.0) * diff * color;
    vec4 specular;
    if (material.hasSpecularTexture)
        specular = vec4(light.specular, 1.0) * spec * color;
    else
        specular = vec4(light.specular, 1.0) * (spec * vec4(material.specularVector, 0.0f));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec4 ambient = vec4(light.ambient, 1.0) * color;
    vec4 diffuse = vec4(light.diffuse, 1.0) * diff * color;
    vec4 specular;
    if (material.hasSpecularTexture)
        specular = vec4(light.specular, 1.0) * spec * color;
    else
        specular = vec4(light.specular, 1.0) * (spec * vec4(material.specularVector, 0.0f));
        
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}