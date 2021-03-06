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


#include "baselight.h"
#include "transform.h"

#define COLOR_TEXTURE_UNIT_INDEX        0

class ShaderProgram
{
public:
    int programId;
    
    ShaderProgram();
    void AddShader(const char* fileName, const GLenum type);
    void LinkProgram();
    void Use();
    void Unuse();
	void SetUniformi(const char* uniform, int val);
    void SetUniform2i(const char* uniform, int val1, int val2);
	void SetUniformf(const char* uniform, float val);
	void SetUniform4v(const char* uniform, const MAT4& matrix);
    void SetUniform4v(const char* uniform, const float* matrix);
};

// image based lighting also handles skybox
class IBLProgram : public ShaderProgram
{
public:
	void Initialize();
	void SetEnvMapTU(unsigned int textureUnit);
	void SetIrradTU(unsigned int textureUnit);
	void SetVP(const MAT4& wvp);
    void SetVP(const float* transform);
	void SetWorldMatrix(const MAT4& w);
    void SetEyePos(const vec3& eye);
	void SetRoughness(float r);
	void setMetallic(float m);
    void SetScreenDim(int width, int height);
	void SetRandomness(float r);
	void SetPosTU(unsigned int TU);
	void SetNormalTU(unsigned int TU);
	void SetDepthMap(unsigned int TU);
	void SetAOMap(unsigned int TU);

	GLuint envMapTextUnitLoc;
	GLuint irradTextUnitLoc;
	GLuint gVPLoc;
	GLuint worldMatrixLoc;
	GLuint eyePosLoc;
	GLuint roughnessLoc;
	GLuint metallicLoc;
	GLuint screenDimLoc;
	GLuint randomnessLoc;
	GLuint normalBufferLoc;
	GLuint posBufferLoc;
	GLuint depthMapLoc;
    GLuint aoMapLoc; //used for depth for AO
};

class ShadowProgram : public ShaderProgram
{
public:
	void Initialize();
	void SetWVP(const MAT4& transform);
	void SetTextureUnit(unsigned int textureUnit);

	GLuint wvpLocation;
	GLuint textureUnitLoc;

};

class SkinProgram : public ShaderProgram
{
public:
	SkinProgram();
	static const int MAX_BONES = 45;
	virtual void Initialize();
	void SetBoneTransform(unsigned int boneIndex, MAT4 transform);
	void SetColorTextureUnit(uint textureUnit);

	GLuint boneLocation[MAX_BONES];
	GLuint eyeWorldPosLocation;
	GLuint colorTextureLocation;
};

class LightProgram : public ShaderProgram
{
public:
	virtual void Initialize();
	void SetPosTextureUnit(unsigned int TextureUnit);
	void SetColorTextureUnit(unsigned int TextureUnit);
	void SetNormalTextureUnit(unsigned int TextureUnit);
	void SetScreenSize(int width, int height);
	void SetEyeWorldPos(float x, float y, float z);
	void SetWVP(const MAT4& wvp);
	GLuint posTextureUnitLocation;
	GLuint normalTextureUnitLocation;
	GLuint colorTextureUnitLocation;
	GLuint screenSizeLocation;
	GLuint eyeWorldPosLocation;
	GLuint WVPLocation;
};

class SpotLightProgram : public LightProgram
{
public:
	//glm::vec3  direction;
	//float cutoff;
	GLuint shadowMapLoc;
	GLuint textureLoc;
	GLuint matSpecularIntensityLocation;
	GLuint matSpecularPowerLocation;
	GLuint eyePos;
	GLuint worldMatrixLoc;
	GLuint samplerLoc;
	GLuint wvpLoc;
	GLuint gLightWVP;
	struct {
		GLuint Color;
		GLuint AmbientIntensity;
		GLuint DiffuseIntensity;
		GLuint Position;
		GLuint Direction;
		GLuint Cutoff;
		struct {
			GLuint Constant;
			GLuint Linear;
			GLuint Exp;
		} Atten;
	} spotLightLoc;


	SpotLightProgram();
	virtual void Initialize();
	void SetSpotLight(const SpotLight& light);
	void SetLightWVP(const MAT4& wvp);
	void SetTextUnit(int textUnit);
	void SetWorldMatrix(const MAT4& w);
	void SetShadowMapTextUnit(unsigned int textureUnit);

};
class DirLightProgram : public LightProgram
{
public:
	DirLightProgram();
	virtual void Initialize();
	void SetDirLight(const DirectionalLight& light);

private:
	struct {
		GLuint Color;
		GLuint AmbientIntensity;
		GLuint DiffuseIntensity;
		GLuint Direction;
	} dirLightLocation;


};


class PLightShaderProgram : public LightProgram
{
public:
	PLightShaderProgram();
	virtual void Initialize();


	void SetCurrLight(const PointLight& light);

private:


	GLuint matSpecularIntensityLocation;
	GLuint matSpecularPowerLocation;


	struct {
		GLuint Color;
		GLuint AmbientIntensity;
		GLuint DiffuseIntensity;
		GLuint Position;
		struct {
			GLuint Constant;
			GLuint Linear;
			GLuint Exp;
		} Atten;
	} pointLightLoc;
};
