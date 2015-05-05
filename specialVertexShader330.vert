#version 330

uniform mat4 mvpMatrix;
uniform float pointSize;
in vec4 vertex;
in vec3 normal;


void main(void)
{
    gl_Position = mvpMatrix * vertex;
    gl_PointSize = pointSize;
}
