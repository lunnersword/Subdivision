

struct LightProperties {
    bool isEnabled;
    bool isLocal;
    bool isSopt;
    vec3 ambient;
    vec3 color;
    vec3 position;
    //vec3 halfVector;
    vec3 coneDirection;
    float spotCosCutoff;
    float spotExponent;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};
/*
struct MaterialProperties {
    vec3 emission;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    // light produced by the material
    // what part of ambient light is reflected
    // what part of diffuse light is scattered
    // what part of specular light is scattered
};
*/

//the set of lights to apply, per invocation of this shader
const int MaxLights = 3;
uniform LightProperties Lights[MaxLights];
/*
const int NumMaterials = 10;
uniform MaterialProperties Material[NumMaterials];
*/
//flat in int MatIndex; //input material index from vertex shader

uniform float Shininess;
uniform float Strength;

//in vec3 normal;
//in vec3 eyeDirection;
//in vec4 position;
//in vec4 color;

varying vec3 normal;
varying vec3 eyeDirection;
varying vec4 position;
varying vec4 color;

//out vec4 FragColor;

void main()
{
    vec3 scatteredLight = vec3(0.0);
    vec3 reflectedLight = vec3(0.0);

    //loop over all the lights
    for (int light = 0; light < MaxLights; ++light) {
        if (!Lights[light].isEnabled)
            continue;
        vec3 halfVector;
        vec3 lightDirection = Lights[light].position;
        float attenuation = 1.0;

        if (Lights[light].isLocal) {
            lightDirection = lightDirection - vec3(position);
            float lightDistance = length(lightDirection);
            lightDirection /= lightDistance;
            attenuation = 1.0 / (Lights[light].constantAttenuation
                                 + Lights[light].linearAttenuation * lightDistance
                                 + Lights[light].quadraticAttenuation * lightDistance * lightDistance);
            if (Lights[light].isSopt) {
                float spotCos = dot(lightDirection, -Lights[light].coneDirection);
                if (spotCos < Lights[light].spotCosCutoff)
                    attenuation = 0.0;
                else
                    attenuation *= pow(spotCos, Lights[light].spotExponent);
            }

        }
        halfVector = normalize(lightDirection + eyeDirection);
        float diffuse = max(0.0, dot(normal, lightDirection));
        float specular = max(0.0, dot(normal, halfVector));
        if (diffuse == 0.0)
            specular = 0.0;
        else
            specular = pow(specular, Shininess) * Strength;

        //accumulate all the lights' effects
        scatteredLight += Lights[light].ambient /** Material[MatIndex].ambient*/ * attenuation +
                Lights[light].color /** Material[MatIndex].diffuse*/ * diffuse * attenuation;
        reflectedLight += Lights[light].color * /*Material[MatIndex].specular **/ specular * attenuation;
        vec3 rgb = min(/*Material[MatIndex].emission*/ + color.rgb * scatteredLight + reflectedLight, vec3(1.0));
        gl_FragColor = vec4(rgb, color.a);

    }
}
