#version 330 core
struct Material
{
    vec3 diffuse;
    vec3 specular;

    float opacity;
    float shininess;
};

struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;

    float innerCutoff;
    float outerCutoff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

#define NUM_GLOBAL_LIGHTS 0
#define NUM_POINT_LIGHTS 1
#define NUM_SPOT_LIGHTS 0

#if NUM_GLOBAL_LIGHTS == 1
uniform DirLight dirlight;
#elif NUM_GLOBAL_LIGHT > 1
uniform DirLight dirlights[NUM_GLOBAL_LIGHTS];
#endif

#if NUM_POINT_LIGHTS == 1
uniform PointLight pointlight;
#elif NUM_POINT_LIGHTS > 1
uniform PointLight pointlights[NUM_POINT_LIGHTS];
#endif

#if NUM_SPOT_LIGHTS == 1
uniform SpotLight spotlight;
#elif NUM_SPOT_LIGHTS > 1
uniform SpotLight spotlights[NUM_SPOT_LIGHTS];
#endif

uniform vec3 toView;

uniform Material material;

vec3 calcDirLight(DirLight light, vec3 diffMap, vec3 specMap, vec3 normal);
vec3 calcPointLight(PointLight light, vec3 diffMap, vec3 specMap, vec3 normal);
vec3 calcSpotLight(SpotLight light, vec3 diffMap, vec3 specMap, vec3 normal);

void main()
{
    vec3 result = vec3(0.0);

    vec3 dmap = material.diffuse;
    vec3 smap = material.specular;

    #if NUM_GLOBAL_LIGHTS == 1
        result += calcDirLight(dirlight, dmap, smap, Normal);
    #elif NUM_GLOBAL_LIGHTS > 1
        for(int i = 0; i < NUM_GLOBAL_LIGHTS; i++)
        {
            result += calcDirLight(dirlights[i], dmap, smap, Normal);
        }
    #endif

    #if NUM_POINT_LIGHTS == 1
        result += calcPointLight(pointlight, dmap, smap, Normal);
    #elif NUM_POINT_LIGHTS > 1
        for(int i = 0; i < NUM_POINT_LIGHTS; i++)
        {
            result += calcPointLight(pointlights[i], dmap, smap, Normal);
        }
    #endif

    #if NUM_SPOT_LIGHTS == 1
        result += calcSpotLight(spotlight, dmap, smap, Normal);
    #elif NUM_SPOT_LIGHTS > 1
        for(int i = 0; i < NUM_SPOT_LIGHTS; i++)
        {
            result += calcSpotLight(spotlights[i], dmap, smap, Normal);
        }
    #endif

    FragColor = vec4(result, material.opacity);
}

vec3 calcDirLight(DirLight light, vec3 diffMap, vec3 specMap, vec3 normal)
{
    vec3 ambient = light.ambient * diffMap;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(norm, lightDir), 0.0);

    vec3 diffuse = diff * light.diffuse * diffMap;

    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(toView, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * light.specular * specMap;

    return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight light, vec3 diffMap, vec3 specMap, vec3 normal)
{
    vec3 ambient = light.ambient * diffMap;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);

    vec3 diffuse = diff * light.diffuse * diffMap;

    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(toView, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * light.specular * specMap;

    float dist = length(FragPos - light.position);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);

    return attenuation * (ambient + diffuse + specular);
}

vec3 calcSpotLight(SpotLight light, vec3 diffMap, vec3 specMap, vec3 normal)
{
    vec3 lightDir = normalize(light.position - FragPos);
    float theta = dot(normalize(-light.direction), lightDir);
    float intensity = smoothstep(0.0, 1.0, (theta - light.outerCutoff) / (light.innerCutoff - light.outerCutoff));

    if (intensity == 0) return vec3(0.0);

    vec3 ambient = light.ambient * diffMap;

    vec3 norm = normalize(normal);

    float diff = max(dot(norm, lightDir), 0.0);

    vec3 diffuse = diff * light.diffuse * diffMap;

    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(toView, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * light.specular * specMap;

    return intensity * (ambient + diffuse + specular);
}
