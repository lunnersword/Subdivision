#define highp
#define lowp
#define mediump

uniform mat4 mvpMatrix;
uniform float pointSize;
//in vec4 vertex;
//in vec3 normal;
attribute highp vec4 vertex;

void main(void)
{
    gl_Position = mvpMatrix * vertex;
    gl_PointSize = pointSize;
}
