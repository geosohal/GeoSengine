#version 330

struct BaseLight
{
    vec3 Color;
    float AmbientIntensity;
    float DiffuseIntensity;
};

struct DirectionalLight
{
    BaseLight Base;
    vec3 Direction;
};

struct Attenuation
{
    float Constant;
    float Linear;
    float Exp;
};

struct PointLight
{
    BaseLight Base;
    vec3 Position;
    Attenuation Atten;
};

struct SpotLight
{
    PointLight Base;
    vec3 Direction;
    float Cutoff;
};

uniform sampler2D gPositionMap;
uniform sampler2D gColorMap;
uniform sampler2D gNormalMap;
uniform DirectionalLight gDirectionalLight;
uniform PointLight gPointLight;
uniform SpotLight gSpotLight;
uniform vec3 gEyeWorldPos;
uniform float gMatSpecularIntensity = 1.0f;
uniform float gSpecularPower = 36;
uniform int gLightType;
uniform vec2 gScreenSize;

vec4 CalcLightInternal(BaseLight Light,
                                           vec3 LightDirection,
                                           vec3 WorldPos,
                                           vec3 Normal)
{
    vec4 AmbientColor = vec4(Light.Color * Light.AmbientIntensity, 1.0);
    float DiffuseFactor = dot(Normal, -LightDirection);
        DiffuseFactor = .5;	// temp for debug

    vec4 DiffuseColor  = vec4(0, 0, 0, 0);
    vec4 SpecularColor = vec4(0, 0, 0, 0);

    if (DiffuseFactor > 0.0) {
        DiffuseColor = vec4(Light.Color * Light.DiffuseIntensity * DiffuseFactor, 1.0);

        vec3 VertexToEye = normalize(gEyeWorldPos - WorldPos);
        vec3 LightReflect = normalize(reflect(LightDirection, Normal));
        float SpecularFactor = 0; // temp for debug
    }

    return (AmbientColor + DiffuseColor + SpecularColor);
}

vec4 CalcDirectionalLight(vec3 WorldPos, vec3 Normal)
{
    return CalcLightInternal(gDirectionalLight.Base,
                                                         gDirectionalLight.Direction,
                                                         WorldPos,
                                                         Normal);
}



vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / gScreenSize;
}

out vec4 FragColor;

void main()
{
    vec2 TexCoord = CalcTexCoord();
        vec3 WorldPos = texture(gPositionMap, TexCoord).xyz;
        vec3 Color = texture(gColorMap, TexCoord).xyz;
        vec3 Normal = texture(gNormalMap, TexCoord).xyz;
        Normal = normalize(Normal);
         FragColor = vec4(1,1,1, 1.0)*.3 + vec4(Color, 1.0)* CalcDirectionalLight(WorldPos, Normal);
      //  FragColor = vec4(Color, 1.0) * CalcDirectionalLight(WorldPos, Normal);
}
