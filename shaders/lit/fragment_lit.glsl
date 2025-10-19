#version 330 core
out vec4 FragColor;

uniform vec3 uAlbedo;
uniform float uMetallic;
uniform float uRoughness;
uniform vec3 uViewPos;     // Camera position in world
uniform vec4 uSelectionTint; // Additive tint for selection

// Lights
const int MAX_DIR_LIGHTS = 4;
const int MAX_POINT_LIGHTS = 16;
uniform int uDirLightCount;
uniform vec3 uDirLightDirs[MAX_DIR_LIGHTS]; // direction TO light (i.e., -lightDir)
uniform vec3 uDirLightColors[MAX_DIR_LIGHTS];

uniform int uPointLightCount;
uniform vec3 uPointLightPos[MAX_POINT_LIGHTS];
uniform vec3 uPointLightColors[MAX_POINT_LIGHTS];
uniform float uPointLightRange[MAX_POINT_LIGHTS];
// Ambient lights
uniform vec3 uAmbientColor;  // accumulated ambient color from Ambient lights
uniform float uAmbientStrength; // per-draw multiplier (0 for grid)

in vec3 vWorldPos;

// Approximate normal from derivatives of world position
vec3 computeNormal() {
    vec3 dp1 = dFdx(vWorldPos);
    vec3 dp2 = dFdy(vWorldPos);
    vec3 n = normalize(cross(dp1, dp2));
    return n;
}

void main()
{
    vec3 N = computeNormal();
    vec3 V = normalize(uViewPos - vWorldPos);
    vec3 baseColor = uAlbedo;
    float shininess = mix(64.0, 8.0, uRoughness); // smoother -> higher shininess
    vec3 F0 = mix(vec3(0.04), baseColor, uMetallic);

    vec3 ambient = baseColor * (uAmbientColor * uAmbientStrength);
    vec3 lighting = ambient;

    // Directional lights
    for (int i = 0; i < uDirLightCount; ++i) {
        vec3 L = normalize(uDirLightDirs[i]);
        vec3 H = normalize(L + V);
        float NdotL = max(dot(N, L), 0.0);
        float spec = pow(max(dot(N, H), 0.0), shininess);
        vec3 diffuse = baseColor * NdotL;
        vec3 specular = F0 * spec;
        lighting += (diffuse + specular) * uDirLightColors[i];
    }

    // Point lights
    for (int i = 0; i < uPointLightCount; ++i) {
        vec3 Lvec = uPointLightPos[i] - vWorldPos;
        float dist2 = dot(Lvec, Lvec);
        float range = max(uPointLightRange[i], 0.001);
        float att = 1.0 / (1.0 + dist2 / (range * range));
        vec3 L = normalize(Lvec);
        vec3 H = normalize(L + V);
        float NdotL = max(dot(N, L), 0.0);
        float spec = pow(max(dot(N, H), 0.0), shininess);
        vec3 diffuse = baseColor * NdotL;
        vec3 specular = F0 * spec;
        lighting += (diffuse + specular) * uPointLightColors[i] * att;
    }

    vec3 finalRgb = lighting;
    // Apply selection tint additively but subtle
    finalRgb += uSelectionTint.rgb;
    FragColor = vec4(finalRgb, 1.0);
}
