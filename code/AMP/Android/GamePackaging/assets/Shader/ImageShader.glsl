#VERTEX
#version 320 es
layout (location = 0) in vec2 aPos;
layout(location = 1) in vec2 uvCoords;
out vec3 Color;
out vec2 UV;
 
void main()
{
	gl_Position = vec4(aPos,0,1.0);
	UV = uvCoords;
}
#FRAGMENT
#version 320 es
uniform sampler2D FontTexSampler;
in lowp vec2 UV;
out lowp vec4 PixelColor;
					
void main()
{		
	 PixelColor = texture(FontTexSampler,UV);
	 if(PixelColor.a==0.0) discard;
}
