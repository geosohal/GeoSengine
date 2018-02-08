
#version 330
layout (location = 0) in vec3 Position;   // mapping to defines in mesh.cpp
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;

out vec2 vTexCoord;
out vec3 vNormal, lightVec, vWorldPos;

uniform mat4 gWorld;
uniform mat4 gVP;
uniform vec3 lightPos = vec3(50,50,0);

void main()
{
  gl_Position = gVP * gWorld * vec4(Position, 1.0);
  //gl_Position.z = 1.0f;
  vWorldPos = (gWorld * vec4(Position, 1.0)).xyz;
  vNormal = (gWorld*vec4(Normal, 0.0)).xyz;
  vTexCoord = TexCoord;
  vTexCoord.x = gl_Position.w;
  lightVec = lightPos - vWorldPos;
}
