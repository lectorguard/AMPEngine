#VERTEX
#version 320 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 uvCoords;
out vec3 Color;
out vec2 UV;
out flat int ColorType;
 
void main()
{
	gl_Position = vec4(aPos.xy,0,1.0);
	UV = uvCoords;
	ColorType = int(aPos.z);
}
#FRAGMENT
#version 320 es
uniform sampler2D FontTexSampler;
uniform highp vec3 DefaultColor;
uniform highp vec3 PressedColor;
in lowp vec2 UV;
in flat int ColorType;
out lowp vec4 PixelColor;
					
void main()
{
     lowp vec4 tempColor = texture(FontTexSampler,UV);
	 highp vec3 disColor;
	 if(ColorType == 0)disColor = DefaultColor;
	 else disColor = PressedColor;
	 PixelColor = vec4(disColor,1.0-tempColor.x);
	 if(PixelColor.a == 0.0) discard;
}
