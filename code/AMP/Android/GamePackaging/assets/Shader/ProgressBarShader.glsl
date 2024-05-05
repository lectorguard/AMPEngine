#VERTEX
#version 320 es
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 uvCoords;
layout(location = 2) in int texLoc;
out vec2 UV;
flat out int LocTex;
 
void main()
{
	gl_Position = vec4(aPos,0,1.0);
	UV = uvCoords;
	LocTex = texLoc;
}
#FRAGMENT
#version 320 es
uniform sampler2D progressTex;
uniform sampler2D frameTex;

uniform highp float progress;
uniform highp vec2 yRange;
uniform highp int isFrame;

flat in int LocTex;
in lowp vec2 UV;
out lowp vec4 PixelColor;

precision mediump float;
					
void main()
{		
	float pixelY = gl_FragCoord.y - yRange.x;
	pixelY = pixelY / yRange.y;
	vec4 progressColor = vec4(0,0,0,0);
	if(pixelY < progress)progressColor = texture(progressTex,UV);
	if(isFrame == 0)PixelColor = progressColor;
	else{
		vec4 frameColor = texture(frameTex,UV);
		if(frameColor[3] <= progressColor[3])frameColor = progressColor;
		PixelColor = frameColor;
		if(PixelColor.a < 0.3)discard;
	}
}
