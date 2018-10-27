#version 130

in vec3 position;
in vec3 inColor;
uniform mat4 MVP;

out vec3 color_from_vshader;

void main()
{
    gl_Position = MVP * vec4(position, 1.0);
    color_from_vshader = inColor;
}
