// NOTE: This is by no means a set bit
Shaders Format for variables:

VERTEX ATTRIBUTES:
layout(location = 0) in vec3: the position of the vertex in 3D model space 

STANDARD UNIFORM BLOCKS:
Fig provides several (j.k. right now it is just 1) uniform blocks for shaders to 
use. A shader can choose to include or not include the block. If the shader would 
like to access the data within the block it can do so by including the definition 
following the table description. All blocks are provided with the layout std140, 
and have an associated binding location (TODO: transition away from specific binding).
The blocks provided are: 

FIG_CAMERA_DATA
| data member name | type | description |
| --- | --- | --- |
| MVP | mat4 | The MVP matrix to transform vertices in model coordinates to screen coordinates |

Definition: 
layout(binding = 0, std140) uniform FIG_CAMERA_DATA{
   mat4 MVP;
} camera;

CUSTOM UNIFORM VARIABLES:
Fig supports some custom uniform variables to be displayed, and controlled in the editor.
A custom .json file can also be supplied to control things such as label for the 
variables, the GUI Widget type, and default values, and other metadata.

Supported GUI Widget Types & valid corresponding GLSL data type: 
  - RGBSelector
    - vec3 
  - FSlider 
     - Any data type with base primitive of float

Supported GLSL Uniform Variable Types & default widget: 
  - vec3
    - Default widget: array of 3 FSlider
  - vec4
    - Default widget: array of 4 FSlider
  - mat4
    - Default widget: Not implemented yet


.json Expected Format:
When loading a shader, Fig looks for a json file that has the name (including file extension) to 
draw its metadata for the shader. For example, when loading a shader with the path 
Resources/Shaders/Vertex.vs, Fig looks for a file in the same folder (i.e. Resources/Shaders) titled 
Vertex.vs.json. If there is a custom uniform variable that is not described in the .json file, it given 
default values. If there is a custom uniform variable described in the .json file that is not in the 
shader, then it is ignored. It has the following format: 

Root Data Parameters: 
| Paramater | Type | Required | Descripton |
| :--- | :---: | :---: | :---: | 
| uniforms | array of PropertyArrayObject  | No | Metadata about custom uniform variables |

PropertyArrayObject Schema:
| Paramater | Type | Required | Descripton |
| :--- | :---: | :---: | :---: | 
| name | string | Yes | The name of the custom uniform variable as it appears in the shader program |
| labels | array of strings | No | A list of label(s) for the widget(s). If the widget is made of multiple individual widgets, then each entry of the array would describe on of the widgets. For example if the widget was a slider3 this would have 3 corresponding labels |
| widgets | array of enum (GUIWidget) | No | What widget the user should see in the GUI |
| defaults | array | No | Array of data to be the default for the variable. The data entries in the defaults array will be treated as whatever the fundamental data type is. If the variable has N values (i.e. vec3 has 3 values of type float), then the array should have N default values. If the defaults array has less than N values, it will initialize those values and then default initialize the rest of the values. If it has more than N values, then the first N values will be used. |

GUIWidget enum accepted values: 
RGBSelector, FSlider,

Example:
Resources/Shaders/Vertex.vs has a uniform variable defined as: 

'uniform vec3 color;' Resources/Shaders/Vertex.vs.json might look like: 
{
   "uniforms": [
      {
         "name": "color",
         "label": ["Base Color"],
         "widget": ["RGBSelector"],
         "default": [1.0, 1.0, 1.0]
      },
   ]
},

OR:
{
   "uniforms": [
      {
         "name": "color",
         "label": ["R", "G", "B"],
      },
   ]
}
Would show its default widget (array of 3 FSliders) with the labels R, G, and B. They would be initialized to Fig's default values for floats (currently 0)

You may define a .json file for any, all, or none of the shader files. If a uniform variable is defined twice 
(i.e. in both the shader.vs.json and shader.fs.json) the engine will choose the file corresponding to the vertex 
shader. The outline for the uniform variable does not necessarily need to be in .json file corresponding to its 
shader stage. For example if a uniform is declared and used in shader.vs, but is given an outline in shader.fs.json,
the outline will still be found and used. NOTE: Consider the case where you have shader.vs and shader.fs. The file 
shader.fs declares and uses the uniform variable u_var. Both shader.vs.json and shader.fs.json provide an outline 
for the uniform variable u_var. If this definition is the same, the behavior will be as expected, however because 
the engine uses the outline provided in shader.vs.json over shader.fs.json for u_var, if shader.fs.json contains 
the desired outline, the results will nto be as expected

