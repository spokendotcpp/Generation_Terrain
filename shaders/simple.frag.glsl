#version 130

in vec3 color_from_vshader;
out vec4 FragColor;

void main()
{
    FragColor = vec4(color_from_vshader, 1.0f);
}
