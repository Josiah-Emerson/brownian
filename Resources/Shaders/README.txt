// NOTE: This is by no means a set bit
Shaders Format for variables:

VERTEX ATTRIBUTES:
layout(location = 0) in vec3: the position of the vertex in 3D model space 

UNIFORM BLOCKS:
layout(binding = 0, std140) uniform Camera {
   mat4: the MVP matrix to translate vertex coordinates to world space
   vec3 : color
}
