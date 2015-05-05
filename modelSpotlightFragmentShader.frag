//Fragment shader computing lighting for a directional light
#version 330 core
uniform vec4 color;
uniform vec3 ambientColor;  //around 0.2
uniform vec3 lightColor;    //around 1.0
uniform vec3 lightPosition;
//uniform vec3 HalfVector;    //surface orientation for shiniess spots
uniform float shininess;    //exponent for sharping highlights , around 20
uniform float strength;     //extra factor to adjust shininess, around 10

uniform float constantAttenuation; //attenuation coefficents
uniform float linearAttenuation;
uniform float quadraticAttenuation;

uniform vec3 coneDirection;
uniform float spotCosCutoff;    //how wide the spot is, as a cosin
uniform float spotExponent; //control light fall-off in the sopt


in vec3 normal;
in vec3 viewerDirection;
in vec4 position;

void main(void)
{
    //find the direction and distance of the light,
    //which changes fragment to fragment for a local light
    vec3 lightDirection = lightPosition - vec3(position);
    float lightDistance = length(lightDirection);

    //normalize the light direction vector, so that a do products give consines
    lightDirection = lightDirection / lightDistance;

    //model how much light is available for this fragment
    float attenuation = 1.0 / (constantAttenuation + linearAttenuation*lightDistance + quadraticAttenuation*lightDistance*lightDistance);
    //how close are we to being in the spot?
    float spotCos = dot(lightDirection, -coneDirection);

    //attenuate more, based on spot-relative position
    if (spotCos < spotCosCutoff)
        attenuation = 0.0;
    else
        attenuation *= pow(spotCos, spotExponent);

    vec3 halfVector = normalize(lightDirection + viewerDirection);

    float diffuse = max(0.0, dot(normal, lightDirection));
    float specular = max(0.0, dot(normal, halfVector));

    if(diffuse == 0.0)
        specular = 0.0;
    else
        specular = pow(specular, shininess) * strength;

    vec3 scatteredLight = ambientColor + lightColor*diffuse*attenuation;
    vec3 reflectedLight = lightColor*specular*attenuation;
    vec3 rgb = min(color.rgb * scatteredLight + reflectedLight, vec3(1.0));


    gl_FragColor = vec4(rgb, color.a);
}

