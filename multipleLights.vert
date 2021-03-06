

#define highp
#define mediump
#define lowp

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat3 normalMatrix;
uniform vec3 viewerPosition;
uniform vec4 VertexColor;


//uniform int MaterialType;


//out vec3 normal;
//out vec3 eyeDirection;
//out vec4 position;
//out vec4 color;
varying vec3 normal;
varying vec3 eyeDirection;
varying vec4 position;
varying vec4 color;
//flat out int MatIndex;


//attribute highp vec4 VertexPosition;
//attribute highp vec3 Normal;
attribute highp vec4 VertexPosition;
attribute highp vec3 Normal;

void main(void)
{

   // MatIndex = MaterialType;
    vec4 eyeVertex = mvMatrix * VertexPosition;
    eyeVertex /= eyeVertex.w;
    normal = normalize(normalMatrix * Normal);
    eyeDirection = normalize(viewerPosition - eyeVertex.xyz);
    position = mvMatrix * VertexPosition;
    color = VertexColor;
    gl_Position = mvpMatrix * VertexPosition;
}
