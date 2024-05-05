#VERTEX
#version 320 es

// Attributes
layout (location = 0) in vec3 position;
uniform mat4 view;
uniform mat4 projection;


// Interpolated outputs
out lowp vec3 TexCoord;
  

void main() {
	
	TexCoord = position;
	vec4 pos = projection * vec4(mat3(view) * position,1.0);
    gl_Position = pos.xyww;
}

#FRAGMENT
#version 320 es

in lowp vec3 TexCoord;
uniform samplerCube cubeMap;

out highp vec4 FragColor;

void main() {
	FragColor = texture(cubeMap,TexCoord);
}