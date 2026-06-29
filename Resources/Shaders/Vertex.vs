#version 420 core

layout(location = 0) in vec3 vertexPosition_modelSpace;

layout(binding = 0, std140) uniform CameraData{
   mat4 MVP;
   vec3 color;
} camera;

// uniform mat4 MVP;

out vec3 fragmentColor;

void main(){
   gl_Position = camera.MVP * vec4(vertexPosition_modelSpace, 1);
   // gl_Position = MVP * vec4(vertexPosition_modelSpace, 1);
   fragmentColor = vec3(.2, 0, 1.f);
   // fragmentColor = vertexPosition_modelSpace;
   // fragmentColor = camera.color;
}
