#VERTEX
#version 320 es

// Attributes
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in uvec4 joint;
layout (location = 3) in vec4 weight;

const int MAX_BONES = 64;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 bone_matrix[MAX_BONES];


// Interpolated outputs
out Varying {
   vec2  texCoord;
} vertexOutput;

void main() {
	mat4 bond_transform = bone_matrix[joint[0]] * weight[0];
	bond_transform += bone_matrix[joint[1]] * weight[1];
	bond_transform += bone_matrix[joint[2]] * weight[2];
	bond_transform += bone_matrix[joint[3]] * weight[3];
	

	vec4 bonePosition = bond_transform * vec4(position,1.0);
	//bonePosition = bonePosition.xyzw;
	vec4 pos = model * bonePosition;
	gl_Position = projection * view * pos;
	vertexOutput.texCoord = texCoord;
}

#FRAGMENT
#version 320 es

in Varying{
   lowp vec2 texCoord;
} interpolated;

uniform sampler2D TextureSampler;

//uniform sampler2D colorTexture;
out lowp vec3 pixelColor;

void main() {
    //pixelColor = texture(colorTexture, interpolated.texCoord).rgb;
    //pixelColor = vec3(interpolated.texCoord.xy, 0);
	pixelColor = texture(TextureSampler,interpolated.texCoord).rgb;
    //pixelColor = vec3(1.0f,1.0f,1.0f);
}