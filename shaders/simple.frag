#version 330


in vec3 vNormal;
in vec3 vWorldPos;
in vec3 lightVec;
in vec2 vTexCoord;

uniform vec3 diffuse = vec3(1,0,1);

out vec4 FragColor;

void main()
{
   vec3 N = normalize(vNormal);
   vec3 L = normalize(lightVec);
   vec3 kd = diffuse;

   // phong lighting
   FragColor = vec4(vec3(.5,.5,.5)*kd + kd*max(dot(L,N), 0.0),1.0);//vec4(gl_FragDepth+.2f,0,0,1);
}
