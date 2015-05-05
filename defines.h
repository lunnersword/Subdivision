#ifndef DEFINES_H
#define DEFINES_H
#define MAXSUBTIME 7 // actually is 6
#define PI 3.141592654
#define MultipleLights 0
#define OldGLSLVersion 1

enum LimitOrNot{LIMIT, NORLIMIT};
typedef struct _CheckStatus
{
    bool ANTIALIALIASING_CHECKED;
    bool LIGHTING_CHECKED;
    bool SHADING_CHECKED;
    bool ROTATE_CHECKED;
    bool CAT;
    bool CAT_LIMIT;
    bool CAT_LIMIT_CHECKED;
    LimitOrNot limitOrNot;
    bool DOO;
    bool LOOP;
    bool LOOP_LIMIT;
    bool LOOP_LIMIT_CHECKED;
    bool PREVIOUS;
    bool NEXT;
    bool VERTEX_CHECKED;
    bool WIREFRAME_CHECKED;
    bool FACE_CHECKED;
    bool CONTROL_CHECKED;
    bool SharpFeature_CHECKED;
    bool GENERAL;
    bool DirectionalLight_Checked;
    bool PointLight_Checked;
    bool SpotLight_Checked;
    bool MultipleLights_Checked;
}CheckStatus;
#endif // DEFINES_H
