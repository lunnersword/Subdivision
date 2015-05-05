#define highp
#define lowp
#define mediump

uniform vec4 ambientColor;
uniform vec4 diffuseColor;
uniform vec4 specularColor;
uniform vec4 color;
uniform float ambientReflection;
uniform float diffuseReflection;
uniform float specularReflection;
uniform float shininess;

varying vec3 varyingNormal;
varying vec3 varyingLightDirection;
varying vec3 varyingViewerDirection;



void main(void)
{
    vec3 normal = normalize(varyingNormal);
    vec3 lightDirection = normalize(varyingLightDirection);
    vec3 viewerDirection = normalize(varyingViewerDirection);
    vec4 ambientIllumination = ambientReflection * ambientColor;
    vec4 diffuseIllumination = diffuseReflection * max(0.0, dot(lightDirection, normal)) * diffuseColor;
    vec4 specularIllumination = 10.0 * specularReflection * pow(max(0.0,
                                                              dot(-reflect(lightDirection, normal), viewerDirection)
                                                              ), shininess) * specularColor;
    gl_FragColor = color * (ambientIllumination + diffuseIllumination
                          ) + specularIllumination;
}
