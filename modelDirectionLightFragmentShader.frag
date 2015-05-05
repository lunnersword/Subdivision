//Fragment shader computing lighting for a directional light
#version 330 core
uniform vec4 color;
uniform vec3 ambientColor;  //around 0.2

uniform vec3 lightPosition;
uniform vec3 lightColor;    //around 1.0
uniform vec3 lightDirection;    //direction toward the light
//uniform vec3 HalfVector;    //surface orientation for shiniess spots
uniform float shininess;    //exponent for sharping highlights , around 20
uniform float strength;     //extra factor to adjust shininess, around 10


in vec3 normal;
//in vec3 lightDirection;
in vec3 viewerDirection;

void main(void)
{
    //compute cosine of the directions, using dot products, to see how much light would be reflected
    //halfVector is the half vector between lightDirection and viewerDirection
    vec3 halfVector = normalize(lightDirection+viewerDirection);

    float diffuse = max(0.0, dot(normal, lightDirection));
    float specular = max(0.0, dot(normal, halfVector));

    //surfaces facing away from the light (negative dot products)
    // won't be lit by the directional light
    if (diffuse == 0.0)
        specular = 0.0;
    else
        specular = pow(specular, shininess); //sharpen the highlight

    vec3 scatteredLight = ambientColor + lightColor * diffuse;
    vec3 reflectedLight = lightColor * specular * strength;
    //don't modulate the underlying color with reflected light, only with scattered light

    vec3 rgb = min(color.rgb * scatteredLight + reflectedLight, vec3(1.0));

    gl_FragColor = vec4(rgb, color.a);
}

