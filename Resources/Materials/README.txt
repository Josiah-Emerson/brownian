The .fmat stands for fig material and literally has two keywords:
Vert: the path to the vertex shader
Frag: the path to the fragment shader 

The order does not matter, however each keyword must have one (and only one) path
The path should be relative to 'Resources/Shaders'. For example a shader at path 
Resources/Shaders/Vertex.vs and Resources/Shaders/Fragment.fs could be written as:

Frag Fragment.fs
Vert Vertex.vs
