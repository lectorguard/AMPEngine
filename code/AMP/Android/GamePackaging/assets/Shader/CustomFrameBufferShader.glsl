#VERTEX
#version 320 es
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat2 transform;
uniform vec2 delta;
uniform int ShadingType;
out vec2 TexCoords;

void main()
{
	if(ShadingType == 0){
		gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
	}else{
		vec2 resPos = transform * aPos;
		gl_Position = vec4(resPos.x + delta.x, resPos.y+ delta.y, 0.0, 1.0); 
	}
    TexCoords = aTexCoords;
} 
#FRAGMENT
#version 320 es
out lowp vec4 FragColor;
  
in highp vec2 TexCoords;

uniform sampler2D screenTexture;
uniform highp int ShadingType; 

uniform highp vec2 direction;
uniform highp vec2 resolution;  

precision mediump float;

void main()
{ 
	if(ShadingType == 0){
		FragColor = texture(screenTexture, TexCoords);
		//if(FragColor == vec4(0.0,0.0,0.0,1.0))FragColor = vec4(0.0,0.0,0.0,0.0);

	}else if(ShadingType == 1){
		vec4 color = vec4(0.0);
		vec2 off1 = vec2(1.33333333333333333) * direction;
		color += texture(screenTexture, TexCoords) * 0.29411764705882354;
		color += texture(screenTexture, TexCoords + (off1 / resolution)) * 0.35294117647058826;
		color += texture(screenTexture, TexCoords - (off1 / resolution)) * 0.35294117647058826;
		FragColor = color;
		//if(FragColor == vec4(0.0,0.0,0.0,1.0))FragColor = vec4(0.0,0.0,0.0,0.0);
	}
}
