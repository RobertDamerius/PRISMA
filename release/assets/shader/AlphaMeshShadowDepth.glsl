// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// VERTEX SHADER
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef VERTEX_SHADER


// vertex shader input
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec2 texCoord;


// vertex shader output
out VS_OUT {
    vec2 texCoord;
} vsOut;


// uniforms
uniform mat4 modelMatrix;


// vertex shader main
void main(void){
    vsOut.texCoord = texCoord;
    gl_Position = modelMatrix * vec4(position, 1.0f);
}


#endif /* VERTEX_SHADER */


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// GEOMETRY SHADER
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef GEOMETRY_SHADER
#include "CameraUniform.glsl"
#include "LightMatricesUniform.glsl"


// layout specification
layout(triangles, invocations = $NUMBER_OF_SHADOW_CASCADES$) in;
layout(triangle_strip, max_vertices = 3) out;


// geometry shader input
in VS_OUT {
    vec2 texCoord;
} gsIn[];


// geometry shader output
out GS_OUT {
    vec2 texCoord;
} gsOut;


// geometry shader main
void main(void){
    for(int i = 0; i < 3; ++i){
        gsOut.texCoord = gsIn[i].texCoord;
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


// uniforms
layout (binding = 0) uniform sampler2D textureDiffuseMap;


// fragment shader input
in GS_OUT {
    vec2 texCoord;
} fsIn;


// fragment shader main
void main(void){
    // alpha testing
    float diffuseMapAlpha = texture(textureDiffuseMap, fsIn.texCoord).a;
    if(diffuseMapAlpha < 0.5){
        discard;
    }
}


#endif /* FRAGMENT_SHADER */

