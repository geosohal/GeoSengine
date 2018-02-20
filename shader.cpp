///////////////////////////////////////////////////////////////////////
// A slight encapsulation of a shader program. This contains methods
// to build a shader program from multiples files containing vertex
// and pixel shader code, and a method to link the result.  When
// loaded (method "Use"), its vertex shader and pixel shader will be
// invoked for all geometry passing through the graphics pipeline.
// When done, unload it with method "Unuse".
//
// Copyright 2013 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////


#include <fstream>
//#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_3_Core>
#include <string>



#include "openglfunctions.h"
#include <QtOpenGL/QtOpenGL>
#include "shader.h"

#define CHECKERRORNOX {GLenum err = GL::glGetError(); if (err != GL_NO_ERROR) { fprintf(stderr, "OpenGL Serror (at line %d): ", __LINE__); } }
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))



// Reads a specified file into a string and returns the string.
char* ReadFile(const char* name)
{
    std::ifstream f;
    f.open(name, std::ios_base::binary); // Open
    f.seekg(0, std::ios_base::end);      // Position at end
    int length = f.tellg();              //   to get the length

    char* content = new char[length + 1]; // Create buffer of needed length
    f.seekg(0, std::ios_base::beg);     // Position at beginning
    f.read(content, length);            //   to read complete file
    f.close();                           // Close

    content[length] = char(0);           // Finish with a NULL
    return content;
}

// Creates an empty shader program.
ShaderProgram::ShaderProgram()
{
    programId = GL::glCreateProgram();
}

// Use a shader program
void ShaderProgram::Use()
{
    GL::glUseProgram(programId);
    CHECKERRORNOX
}

// Done using a shader program
void ShaderProgram::Unuse()
{
    GL::glUseProgram(0);
}

// Read, send to OpenGL, and compile a single file into a shader program.
void ShaderProgram::AddShader(const char* fileName, GLenum type)
{
    // Read the source from the named file
    char* src = ReadFile(fileName);
    const char* psrc[1] = { src };

    // Create a shader and attach, hand it the source, and compile it.
    int shader = GL::glCreateShader(type);
    GL::glAttachShader(programId, shader);
    GL::glShaderSource(shader, 1, psrc, NULL);
    GL::glCompileShader(shader);
    delete src;

    // Get the compilation status
    int status;
    GL::glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    // If compilation status is not OK, get and print the log message.
    if (status != 1) {
        int length;
        GL::glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char* buffer = new char[length];
        GL::glGetShaderInfoLog(shader, length, NULL, buffer);
        printf("Compile log for %s:\n%s\n", fileName, buffer);
        delete buffer;
    }
}

void ShaderProgram::LinkProgram()
{
    // Link program and check the status
    GL::glLinkProgram(programId);
    int status;
    GL::glGetProgramiv(programId, GL_LINK_STATUS, &status);

    // If link failed, get and print log
    if (status != 1) {
        int length;
        GL::glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &length);
        char* buffer = new char[length];
        GL::glGetProgramInfoLog(programId, length, NULL, buffer);
        printf("Link log:\n%s\n", buffer);
        delete buffer;
    }
}

void ShaderProgram::SetUniformi(const char* uniform, int val)
{
    int loc = GL::glGetUniformLocation(programId, (GLchar*)uniform);
    GL::glUniform1i(loc, val);
    CHECKERRORNOX
}

void ShaderProgram::SetUniform2i(const char* uniform, int val1, int val2)
{
    int loc = GL::glGetUniformLocation(programId, (GLchar*)uniform);
    GL::glUniform2f(loc, val1, val2);
    CHECKERRORNOX
}
void ShaderProgram::SetUniformf(const char* uniform, float val)
{
    int loc = GL::glGetUniformLocation(programId, (GLchar*)uniform);
    GL::glUniform1f(loc, val);
    CHECKERRORNOX
}

void ShaderProgram::SetUniform4v(const char* uniform, const MAT4& matrix)
{
    int loc = GL::glGetUniformLocation(programId, (GLchar*)uniform);
    GL::glUniformMatrix4fv(loc, 1, GL_TRUE, (const GLfloat*)matrix.M);
}

void ShaderProgram::SetUniform4v(const char* uniform, const float* matrix)
{
    int loc = GL::glGetUniformLocation(programId, (GLchar*)uniform);
    GL::glUniformMatrix4fv(loc, 1, GL_TRUE, (const GLfloat*)matrix);
}
void IBLProgram::Initialize()
{
    AddShader("/home/geo5/GeoSengine/shaders/IBL.vs", GL_VERTEX_SHADER);
    AddShader("/home/geo5/GeoSengine/shaders/IBL.fs", GL_FRAGMENT_SHADER);
    LinkProgram();

    envMapTextUnitLoc = GL::glGetUniformLocation(programId, "environment");
    CHECKERRORNOX
    irradTextUnitLoc = GL::glGetUniformLocation(programId, "irradiance");
    CHECKERRORNOX
    gVPLoc = GL::glGetUniformLocation(programId, "gVP");
    CHECKERRORNOX
    worldMatrixLoc = GL::glGetUniformLocation(programId, "gWorld");
    CHECKERRORNOX
    eyePosLoc = GL::glGetUniformLocation(programId, "eyePos");
    CHECKERRORNOX
    screenDimLoc = GL::glGetUniformLocation(programId, "screenDim");
    CHECKERRORNOX
    metallicLoc = GL::glGetUniformLocation(programId, "metallicness");
    CHECKERRORNOX
    roughnessLoc = GL::glGetUniformLocation(programId, "roughness");
    CHECKERRORNOX
        randomnessLoc = GL::glGetUniformLocation(programId, "randomness");
    normalBufferLoc = GL::glGetUniformLocation(programId, "normalMap");
    posBufferLoc = GL::glGetUniformLocation(programId, "positionMap");
    depthMapLoc = GL::glGetUniformLocation(programId, "depthMap");
    aoMapLoc = GL::glGetUniformLocation(programId, "aoMap");
}

void IBLProgram::SetRandomness(float r)
{
    GL::glUniform1f(randomnessLoc, r);
}
void IBLProgram::SetRoughness(float r)
{
    GL::glUniform1f(roughnessLoc, r);
    CHECKERRORNOX
}

void IBLProgram::SetScreenDim(int width, int height)
{
    GL::glUniform2f(screenDimLoc, (float)width, (float)height);
    CHECKERRORNOX
}
void IBLProgram::setMetallic(float m)
{
    GL::glUniform1f(metallicLoc, m);
    CHECKERRORNOX
}

void IBLProgram::SetEyePos(const vec3& eye)
{
    GL::glUniform3fv(eyePosLoc, 1, &eye[0]);
    CHECKERRORNOX
}

void IBLProgram::SetEnvMapTU(unsigned int textureUnit)
{
    GL::glUniform1i(envMapTextUnitLoc, textureUnit);
    CHECKERRORNOX
}

void IBLProgram::SetIrradTU(unsigned int textureUnit)
{
    GL::glUniform1i(irradTextUnitLoc, textureUnit);
    CHECKERRORNOX
}

void IBLProgram::SetWorldMatrix(const MAT4& w)
{
    GL::glUniformMatrix4fv(worldMatrixLoc, 1, GL_TRUE, (const GLfloat*)w.M);
    CHECKERRORNOX
}

void IBLProgram::SetVP(const MAT4& transform)
{
    GL::glUniformMatrix4fv(gVPLoc, 1, GL_TRUE, (const GLfloat*)transform.M);
    CHECKERRORNOX
}

void IBLProgram::SetVP(const float* transform)
{
    GL::glUniformMatrix4fv(gVPLoc, 1, GL_TRUE, (const GLfloat*)transform);
    CHECKERRORNOX
}

void IBLProgram::SetPosTU(unsigned int TU)
{
    GL::glUniform1i(posBufferLoc, TU);
    CHECKERRORNOX
}
void IBLProgram::SetNormalTU(unsigned int TU)
{
    GL::glUniform1i(normalBufferLoc, TU);
    CHECKERRORNOX
}

void IBLProgram::SetDepthMap(unsigned int TU)
{
    GL::glUniform1i(depthMapLoc, TU);
    CHECKERRORNOX
}

void IBLProgram::SetAOMap(unsigned int TU)
{
    GL::glUniform1i(aoMapLoc, TU);
    CHECKERRORNOX
}
void ShadowProgram::Initialize()
{
    AddShader("/home/geo5/GeoSengine/shaders/shadowmap.vs", GL_VERTEX_SHADER);
    AddShader("/home/geo5/GeoSengine/shaders/shadowmap.fs", GL_FRAGMENT_SHADER);
    CHECKERRORNOX
    LinkProgram();

    wvpLocation = GL::glGetUniformLocation(programId, "gWVP");
    textureUnitLoc = GL::glGetUniformLocation(programId, "gShadowMap");
}

void ShadowProgram::SetWVP(const MAT4& transform)
{
    GL::glUniformMatrix4fv(wvpLocation, 1, GL_TRUE, (const GLfloat*)transform.M);
    CHECKERRORNOX
}

void ShadowProgram::SetTextureUnit(unsigned int textureUnit)
{
    GL::glUniform1i(textureUnitLoc, textureUnit);
    CHECKERRORNOX
}

SkinProgram::SkinProgram()
{
    programId = GL::glCreateProgram();
}
void SkinProgram::Initialize()
{
    AddShader("/home/geo5/GeoSengine/shaders/skinningShader.vs", GL_VERTEX_SHADER);
    AddShader("/home/geo5/GeoSengine/shaders/skinningShader.fs", GL_FRAGMENT_SHADER);
    CHECKERRORNOX
    LinkProgram();
    for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(boneLocation); i++)
    {
        char name[128];
        memset(name, 0, sizeof(name));
        sprintf(name, "gBones[%d]", i);
        boneLocation[i] = GL::glGetUniformLocation(programId, name);
    }
    eyeWorldPosLocation = GL::glGetUniformLocation(programId, "gEyeWorldPos");
    colorTextureLocation = GL::glGetUniformLocation(programId, "gColorMap");
}

void SkinProgram::SetBoneTransform(unsigned int boneIndex, MAT4 transform)
{
    GL::glUniformMatrix4fv(boneLocation[boneIndex], 1, GL_TRUE, transform.Pntr());
}
void SkinProgram::SetColorTextureUnit(uint textureUnit)
{
    GL::glUniform1i(colorTextureLocation, textureUnit);
}


SpotLightProgram::SpotLightProgram()
{
    programId = GL::glCreateProgram();
}

void SpotLightProgram::Initialize()
{
    AddShader("/home/geo5/GeoSengine/shaders/spotlight.vs", GL_VERTEX_SHADER);
    AddShader("/home/geo5/GeoSengine/shaders/spotlight.fs", GL_FRAGMENT_SHADER);
    LinkProgram();
    CHECKERRORNOX
    spotLightLoc.Color = GL::glGetUniformLocation(programId, "gSpotLight.Base.Base.Color");
    CHECKERRORNOX
    spotLightLoc.AmbientIntensity = GL::glGetUniformLocation(programId, "gSpotLight.Base.Base.AmbientIntensity");
    spotLightLoc.Position = GL::glGetUniformLocation(programId, "gSpotLight.Base.Position");
    CHECKERRORNOX
    spotLightLoc.DiffuseIntensity = GL::glGetUniformLocation(programId, "gSpotLight.Base.Base.DiffuseIntensity");
    spotLightLoc.Atten.Constant = GL::glGetUniformLocation(programId, "gSpotLight.Base.Atten.Constant");
    spotLightLoc.Atten.Linear = GL::glGetUniformLocation(programId, "gSpotLight.Base.Atten.Linear");
    spotLightLoc.Atten.Exp = GL::glGetUniformLocation(programId, "gSpotLight.Base.Atten.Exp");
    spotLightLoc.Cutoff = GL::glGetUniformLocation(programId, "gSpotLight.Cutoff");
    spotLightLoc.Direction = GL::glGetUniformLocation(programId, "gSpotLight.Direction");
    eyePos = GL::glGetUniformLocation(programId, "gEyeWorldPos");
    worldMatrixLoc = GL::glGetUniformLocation(programId, "gWorld");
    shadowMapLoc = GL::glGetUniformLocation(programId, "gShadowMap");
    samplerLoc =  GL::glGetUniformLocation(programId, "gSampler");
    wvpLoc = GL::glGetUniformLocation(programId, "gWVP");
    gLightWVP = GL::glGetUniformLocation(programId, "gLightWVP");
    CHECKERRORNOX

    matSpecularIntensityLocation = GL::glGetUniformLocation(programId, "gMatSpecularIntensity");
    matSpecularPowerLocation = GL::glGetUniformLocation(programId, "gSpecularPower");
    screenSizeLocation = GL::glGetUniformLocation(programId, "gScreenSize");

    shadowMapLoc = GL::glGetUniformLocation(programId, "gShadowMap");;
    LightProgram::Initialize();
}

void SpotLightProgram::SetLightWVP(const MAT4& wvp)
{
    GL::glUniformMatrix4fv(gLightWVP, 1, GL_TRUE, (const GLfloat*)wvp.M);
    CHECKERRORNOX
}

void SpotLightProgram::SetWorldMatrix(const MAT4& w)
{
    GL::glUniformMatrix4fv(worldMatrixLoc, 1, GL_TRUE, (const GLfloat*)w.M);
}
void SpotLightProgram::SetSpotLight(const SpotLight& light)
{
    GL::glUniform3f(spotLightLoc.Color, light.color.x, light.color.y, light.color.z);
    CHECKERRORNOX
    GL::glUniform3f(spotLightLoc.Direction, light.direction.x, light.direction.y, light.direction.z);
    CHECKERRORNOX
    GL::glUniform1f(spotLightLoc.Cutoff, cosf(ToRadian(light.cutoff)));
    GL::glUniform1f(spotLightLoc.AmbientIntensity, light.ambientIntensity);
    GL::glUniform1f(spotLightLoc.DiffuseIntensity, light.diffuseIntensity);
    GL::glUniform3f(spotLightLoc.Position, light.pos.x, light.pos.y, light.pos.z);
    GL::glUniform1f(spotLightLoc.Atten.Constant, light.atten.constant);
    GL::glUniform1f(spotLightLoc.Atten.Linear, light.atten.linear);
    GL::glUniform1f(spotLightLoc.Atten.Exp, light.atten.exp);
    CHECKERRORNOX
}

void SpotLightProgram::SetShadowMapTextUnit(unsigned int textureUnit)
{
    GL::glUniform1i(shadowMapLoc, textureUnit);
}

void SpotLightProgram::SetTextUnit(int textUnit)
{
    GL::glUniform1i(samplerLoc, textUnit);
}

PLightShaderProgram::PLightShaderProgram()
{
    programId = GL::glCreateProgram();
}

void PLightShaderProgram::Initialize()
{


    CHECKERRORNOX
    AddShader("/home/geo5/GeoSengine/shaders/lightPass.vs", GL_VERTEX_SHADER);
    AddShader("/home/geo5/GeoSengine/shaders/pointLight.fs", GL_FRAGMENT_SHADER);
    CHECKERRORNOX
        LinkProgram();
    CHECKERRORNOX
    pointLightLoc.Color = GL::glGetUniformLocation(programId, "gPointLight.Base.Color");
    CHECKERRORNOX
    pointLightLoc.AmbientIntensity = GL::glGetUniformLocation(programId, "gPointLight.Base.AmbientIntensity");
    pointLightLoc.Position = GL::glGetUniformLocation(programId, "gPointLight.Position");
    CHECKERRORNOX
    pointLightLoc.DiffuseIntensity = GL::glGetUniformLocation(programId, "gPointLight.Base.DiffuseIntensity");
    pointLightLoc.Atten.Constant = GL::glGetUniformLocation(programId, "gPointLight.Atten.Constant");
    pointLightLoc.Atten.Linear = GL::glGetUniformLocation(programId, "gPointLight.Atten.Linear");
    pointLightLoc.Atten.Exp = GL::glGetUniformLocation(programId, "gPointLight.Atten.Exp");

    CHECKERRORNOX
    posTextureUnitLocation = GL::glGetUniformLocation(programId, "gPositionMap");
    CHECKERRORNOX
    colorTextureUnitLocation = GL::glGetUniformLocation(programId, "gColorMap");
    normalTextureUnitLocation = GL::glGetUniformLocation(programId, "gNormalMap");

    matSpecularIntensityLocation = GL::glGetUniformLocation(programId, "gMatSpecularIntensity");
    matSpecularPowerLocation = GL::glGetUniformLocation(programId, "gSpecularPower");
    screenSizeLocation = GL::glGetUniformLocation(programId, "gScreenSize");
    CHECKERRORNOX
    LightProgram::Initialize();
}





void PLightShaderProgram::SetCurrLight(const PointLight& light)
{
    GL::glUniform3f(pointLightLoc.Color, light.color.x, light.color.y, light.color.z);
    GL::glUniform1f(pointLightLoc.AmbientIntensity, light.ambientIntensity);
    GL::glUniform1f(pointLightLoc.DiffuseIntensity, light.diffuseIntensity);
    GL::glUniform3f(pointLightLoc.Position, light.pos.x, light.pos.y, light.pos.z);
    GL::glUniform1f(pointLightLoc.Atten.Constant, light.atten.constant);
    GL::glUniform1f(pointLightLoc.Atten.Linear, light.atten.linear);
    GL::glUniform1f(pointLightLoc.Atten.Exp, light.atten.exp);
    CHECKERRORNOX
}

DirLightProgram::DirLightProgram()
{
    programId = GL::glCreateProgram();
    CHECKERRORNOX
}

void DirLightProgram::Initialize()
{
    AddShader("/home/geo5/GeoSengine/shaders/dirLightPass.fs", GL_FRAGMENT_SHADER);

    {
        GLenum err = GL::glGetError();
        if (err != GL_NO_ERROR) {
            fprintf(stderr, "OpenGL Serror (at line %d): ", __LINE__);
        }
    }
    AddShader("/home/geo5/GeoSengine/shaders/lightPass.vs", GL_VERTEX_SHADER);
    CHECKERRORNOX
    LinkProgram();
    Use();
    CHECKERRORNOX
    dirLightLocation.Color = GL::glGetUniformLocation(programId, "gDirectionalLight.Base.Color");
    dirLightLocation.AmbientIntensity = GL::glGetUniformLocation(programId, "gDirectionalLight.Base.AmbientIntensity");
    dirLightLocation.Direction = GL::glGetUniformLocation(programId, "gDirectionalLight.Direction");
    dirLightLocation.DiffuseIntensity = GL::glGetUniformLocation(programId, "gDirectionalLight.Base.DiffuseIntensity");
    CHECKERRORNOX
		
        LightProgram::Initialize();
    CHECKERRORNOX

}

void DirLightProgram::SetDirLight(const DirectionalLight& light)
{
    GL::glUniform3f(dirLightLocation.Color, light.color.x, light.color.y,light.color.z);
    GL::glUniform1f(dirLightLocation.AmbientIntensity, light.ambientIntensity);
    vec3 Direction = light.direction;
    Direction == glm::normalize(Direction);
    GL::glUniform3f(dirLightLocation.Direction, Direction.x, Direction.y, Direction.z);
    GL::glUniform1f(dirLightLocation.DiffuseIntensity, light.diffuseIntensity);
    CHECKERRORNOX
}
void LightProgram::Initialize()
{

    posTextureUnitLocation = GL::glGetUniformLocation(programId, "gPositionMap");
    colorTextureUnitLocation = GL::glGetUniformLocation(programId, "gColorMap");
    normalTextureUnitLocation = GL::glGetUniformLocation(programId, "gNormalMap");
    eyeWorldPosLocation = GL::glGetUniformLocation(programId, "gEyeWorldPos");
    CHECKERRORNOX
    WVPLocation = GL::glGetUniformLocation(programId, "gWVP");
    CHECKERRORNOX
    screenSizeLocation = GL::glGetUniformLocation(programId, "gScreenSize");
    CHECKERRORNOX
}
void LightProgram::SetWVP(const MAT4& wvp)
{
    GL::glUniformMatrix4fv(WVPLocation, 1, GL_TRUE, (const GLfloat*)wvp.M);
    CHECKERRORNOX
}


void LightProgram::SetEyeWorldPos(float x, float y, float z)
{
    GL::glUniform3f(eyeWorldPosLocation, x, y, z);
    CHECKERRORNOX
}

void LightProgram::SetPosTextureUnit(unsigned int TextureUnit)
{
    GL::glUniform1i(posTextureUnitLocation, TextureUnit);
    CHECKERRORNOX
}

void LightProgram::SetColorTextureUnit(unsigned int TextureUnit)
{
    GL::glUniform1i(colorTextureUnitLocation, TextureUnit);
    CHECKERRORNOX
}

void LightProgram::SetNormalTextureUnit(unsigned int TextureUnit)
{
    GL::glUniform1i(normalTextureUnitLocation, TextureUnit);
    CHECKERRORNOX
}

void LightProgram::SetScreenSize(int width, int height)
{
    GL::glUniform2f(screenSizeLocation, (float)width, (float)height);
    CHECKERRORNOX
}

