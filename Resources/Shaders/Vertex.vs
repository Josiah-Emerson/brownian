#version 420 core

layout(location = 0) in vec3 vertexPosition_modelSpace;

layout(std140) uniform FIG_CAMERA_DATA{
   mat4 MVP;
} camera;

uniform vec3 color;

// uniform mat4 MVP;

out vec3 fragmentColor;

void main(){
   gl_Position = camera.MVP * vec4(vertexPosition_modelSpace, 1);
   // gl_Position = MVP * vec4(vertexPosition_modelSpace, 1);
   // fragmentColor = vec3(.2, 0, 1.f);
   // fragmentColor = vertexPosition_modelSpace;
   fragmentColor = vec3(0.2, 0.5, 0.8);
}
