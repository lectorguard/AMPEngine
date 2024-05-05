#VERTEX
#version 320 es
layout (location = 0) in vec3 aPos;
layout(location = 1) in vec3 vertexColor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 fragmentColor;
 
void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	fragmentColor = vertexColor;
}
#FRAGMENT
#version 320 es
out lowp vec4 FragColor;
in lowp vec3 fragmentColor;
					
void main()
{
     FragColor = vec4(fragmentColor,1.0f);
}
