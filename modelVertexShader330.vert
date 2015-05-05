#version 330

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat3 normalMatrix;
uniform vec3 lightPosition;
uniform vec3 viewerPosition;


out vec3 varyingNormal;
out vec3 varyingLightDirection;
out vec3 varyingViewerDirection;

in vec4 vertex;
in vec3 normal;

void main(void)
{
    vec4 eyeVertex = mvMatrix * vertex;
    eyeVertex /= eyeVertex.w;
    varyingNormal = normalMatrix * normal;
    //varyingLightDirection = lightPosition - eyeVertex.xyz;
    varyingLightDirection = lightPosition;
    varyingViewerDirection = viewerPosition - eyeVertex.xyz;
    gl_Position = mvpMatrix * vertex;
}
