#version 460 core
layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float stencil;

void main()
{
    crntPos = vec3(projection * view * model * vec4(aPos, 1.0f);
    gl_Position = projection * view * model * vec4(crntPos, 1.0f);
}
