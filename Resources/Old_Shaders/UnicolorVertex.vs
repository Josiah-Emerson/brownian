#version 330 core

layout(location = 0) in vec3 vertexPosition_modelSpace;

uniform mat4 MVP;
uniform vec3 color;

out vec3 fragmentColor;

void main(){
   gl_Position = MVP * vec4(vertexPosition_modelSpace, 1);
   fragmentColor = color;
}
