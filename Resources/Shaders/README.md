// NOTE: This is by no means a set bit
Shaders Format for variables:

VERTEX ATTRIBUTES:
layout(location = 0) in vec3: the position of the vertex in 3D model space 

STANDARD UNIFORM BLOCKS:
Fig provides several (j.k. right now it is just 1) uniform blocks for shaders to 
use. A shader can choose to include or not include the block. If the shader would 
like to access the data within the block it can do so by including the definition 
following the table description. All blocks are provided with the layout std140. 
The blocks provided are: 

FIG_CAMERA_DATA
| data member name | type | description |
| --- | --- | --- |
| MVP | mat4 | The MVP matrix to transform vertices in model coordinates to screen coordinates |

Definition: 
layout(std140) uniform FIG_CAMERA_DATA{
   mat4 MVP;
} camera;

CUSTOM UNIFORM VARIABLES:
Fig supports some custom uniform blocks to be displayed, controlled in the editor.
A custom .json file can also be supplied to control things such as label for the 
blocks/variables, the GUI Widget type, and default values, and other metadata.

Supported GUI Widget Types & valid corresponding GLSL data type: 
  - RGBSelector
    - vec3 
  - slider
    - any supported numeric type (and vector version of type)

Supported GLSL Uniform Variable Types & default widget: 
  - vec3
    - Default widget: 3 sliders


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
| uniforms | array of ProperyArrayObject  | No | Metadata about custom uniform variables |

PropertyArrayObject Schema:
| Paramater | Type | Required | Descripton |
| :--- | :---: | :---: | :---: | 
| name | string | Yes | The name of the custom uniform variable as it appears in the shader program |
| label | string | No | A description of what the uniform variable is |
| widget | enum (GUIWidget) | No | What widget the user should see in the GUI |
| default | array | No | Array of data to be the default for the variable. The data entries in the defaults array will be treated as whatever the fundamental data type is. If the variable has N values (i.e. vec3 has 3 values of type float), then the array should have N default values. If the defaults array has less than N values, it will initialize those values and then default initialize the rest of the values. If it has more than N values, then the first N values will be used. |

GUIWidget enum accepted values: 
RGBSelector, slider,

Example:
Resources/Shaders/Vertex.vs has a uniform variable defined as: 

'uniform vec3 color;' Resources/Shaders/Vertex.vs.json might look like: 
{
   "uniforms": [
      {
         "name": "color",
         "label": "Base Color",
         "widget": "RGBSelector",
         "default": [1.0, 1.0, 1.0]
      },
   ]
}
Asset manager loads it. And gets all of the above information
Asset manager gets the reflection data from the RenderDevice (?)
Assset manager checks the uniforms and any corresponding ones it matches up
Where should the GUIWidget enum go ?
