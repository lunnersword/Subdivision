#define highp
#define mediump
#define lowp

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat3 normalMatrix;
uniform vec3 lightPosition;
uniform vec3 viewerPosition;


varying vec3 varyingNormal;
varying vec3 varyingLightDirection;
varying vec3 varyingViewerDirection;

attribute highp vec4 vertex;
attribute highp vec3 normal;

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
