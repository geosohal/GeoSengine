#version 430
// ambient occlusion using Alchemy method and deferred shading

#define pi   3.1415926535897932384626433832795
#define pi2  6.2831853071795864769252867665590
const float MIN_DEPTH = .3f;
const float MAX_DEPTH = 4.0f;

// ambient occlusion constants
const float ROI = 10.5f;	// range of influence
const float AOC = .15;	// .1 * ROI, const for falloff function
const float AOCSQ = .0225f;
const float DELTA = 0.0025f;

uniform sampler2D positionMap;// from gbuffer
uniform sampler2D normalMap;	// from gbuffer
uniform sampler2D depthMap;	//from gbuffer
//uniform sampler2D aoMap;

uniform int screenWidth;
uniform int screenHeight;

layout (location = 0) out float aoOut;

in vec2 TexCoord0;
in vec3 Normal0;
in vec3 WorldPos0;

vec2 CalcTexCoord()
{
   return gl_FragCoord.xy / vec2(screenWidth,screenHeight);
}

// pixelDepth is in camera space depth
float CalcAOFactor(vec3 N, vec3 P, float pixelDepth)
{
        int numSamples = 20;
        float occlSum = 0;
        for (int i = 0; i < numSamples; i++)
        {
            float alpha = (i + 0.5f) / numSamples;
            float h = alpha * ROI / pixelDepth;

            uint x = uint(gl_FragCoord.x);
            uint y = uint(gl_FragCoord.y);
            float phi = (30u* x ^ y + 10u * x * y);// rand angle
            float theta = pi2 * alpha * (7*numSamples/9) + phi;

            vec2 pointiCoord = vec2(x,y) + vec2(cos(theta), sin(theta));
            vec3 pointi = texture(positionMap, pointiCoord).xyz;
            vec3 omegai = pointi - P;

            // heaviside step function
            if (ROI - length(omegai) < 0)
                continue;
            else
            {
                float NoW = dot(N, omegai);
                occlSum += max(0, NoW-DELTA*pixelDepth) / max(AOCSQ, dot(omegai, omegai));
            }
        }
        occlSum *= (pi2*AOC)/numSamples;
        float scale =8.f;
        float contrast = 1.55f;
        return clamp( pow(1-scale*occlSum, contrast), 0, 1 );
}

void main()
{
        vec2 TexCoord = CalcTexCoord();
        float depth = texture(depthMap, TexCoord).x ;
        vec3 N = texture(normalMap, TexCoord).xyz;
        vec3 pos = texture(positionMap, TexCoord).xyz;
        float AOfactor = CalcAOFactor(N, pos, depth);
        aoOut = AOfactor;
}
