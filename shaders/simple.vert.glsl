#version 130

in vec3 position;
in vec3 inColor;
out vec3 color_from_vshader;
uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(position, 1.0);
    color_from_vshader = inColor;
}
