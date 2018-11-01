#version 130

// Get it via Buffer Object
in vec3 position;
in vec3 inColor;
in vec3 normal;

// Not via Buffer Object
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// To Fragment Shader
out vec3 color_from_vshader;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);

// send colors to the Fragment Shader :
// make sure that a in variable named "color_from_vshader" exists into the Fragment shader.
    color_from_vshader = inColor;
}
