#VERTEX
#version 320 es
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
    TexCoords = aTexCoords;
}  
#FRAGMENT
#version 320 es
out lowp vec4 FragColor;
  
in highp vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int ShadingType;  

void main()
{ 
	if(ShadingType == 0){
		FragColor = texture(screenTexture, TexCoords);
		if(FragColor == vec4(0.0,0.0,0.0,1.0))FragColor = vec4(0.0,0.0,0.0,0.0);
	}else {
		FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)),1.0);
	}
}