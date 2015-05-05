
#define highp
#define lowp
#define mediump

uniform vec4 color;

//out vec4 FragColor;

void main(void)
{
    //FragColor = color;
    gl_FragColor = color;
}
