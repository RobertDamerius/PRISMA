// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// VERTEX SHADER
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef VERTEX_SHADER


// vertex shader input
layout (location = 0) in vec3 position;


// uniforms
uniform mat4 modelMatrix;


// vertex shader main
void main(void){
    gl_Position = modelMatrix * vec4(position, 1.0f);
}


#endif /* VERTEX_SHADER */


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// GEOMETRY SHADER
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef GEOMETRY_SHADER
#include "CameraUniform.glsl"
#include "LightMatricesUniform.glsl"


layout(triangles, invocations = $NUMBER_OF_SHADOW_CASCADES$) in;
layout(triangle_strip, max_vertices = 3) out;


// geometry shader main
void main(void){
    for(int i = 0; i < 3; ++i){
        gl_Position = lightSpaceMatrices[gl_InvocationID] * cameraViewMatrix * gl_in[i].gl_Position;
        gl_Layer = gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
}


#endif /* GEOMETRY_SHADER */


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FRAGMENT SHADER
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef FRAGMENT_SHADER


// fragment shader main
void main(void){}


#endif /* FRAGMENT_SHADER */

