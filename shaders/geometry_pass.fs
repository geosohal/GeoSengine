#version 330
               
const float MIN_DEPTH = .1f;
const float MAX_DEPTH = 100f;

in vec2 TexCoord0;                                                                  
in vec3 Normal0;                                                                    
in vec3 WorldPos0;   
                                                               
layout (location = 0) out vec3 NormalOut;   
layout (location = 1) out vec3 TexCoordOut; 
layout (location = 2) out vec3 DiffuseOut;   
layout (location = 3) out vec3 WorldPosOut;   

uniform sampler2D depthMap;

  
   
										
uniform sampler2D gColorMap;                
											
void main()									
{											
	WorldPosOut     = WorldPos0; //100.0;					
        DiffuseOut      =  texture(gColorMap, TexCoord0).xyz;
	NormalOut       = normalize(Normal0);		
        TexCoordOut     = vec3(TexCoord0, 0);
}
