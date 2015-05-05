#version 330

uniform vec4 ambientColor;
uniform vec4 diffuseColor;
uniform vec4 specularColor;
uniform vec4 color;
uniform float ambientReflection;
uniform float diffuseReflection;
uniform float specularReflection;
uniform float shininess;

in vec3 varyingNormal;
in vec3 varyingLightDirection;
in vec3 varyingViewerDirection;

out vec4 FragColor;

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
    FragColor = color * (ambientIllumination + diffuseIllumination
                          ) + specularIllumination;
}
