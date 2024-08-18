#include "CameraUniform.glsl"


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// VERTEX SHADER
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef VERTEX_SHADER


// vertex shader input
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;


// vertex shader output
out VS_OUT {
    vec3 position;
    vec3 normal;
    vec3 color;
} vsOut;


// uniforms
uniform mat4 modelMatrix;
uniform vec4 waterClipPlane;


// vertex shader main
void main(void){
    // transformation matrix for normals
    mat3 normalMatrix = transpose(inverse(mat3(cameraViewMatrix * modelMatrix)));

    // vertex shader output to fragment shader
    vec4 worldPosition = modelMatrix * vec4(position, 1.0f);
    vec4 viewSpacePosition = cameraViewMatrix * worldPosition;
    vsOut.position = viewSpacePosition.xyz;
    vsOut.normal = normalMatrix * normal;
    vsOut.color = color;

    // fragment position on screen
    gl_Position = cameraProjectionMatrix * viewSpacePosition;
    gl_ClipDistance[0] = dot(waterClipPlane, worldPosition);
}


#endif /* VERTEX_SHADER */


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// GEOMETRY SHADER
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef GEOMETRY_SHADER


// layout specification
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;


// geometry shader input
in VS_OUT {
    vec3 position;
    vec3 normal;
    vec3 color;
} gsIn[];


// geometry shader output
out GS_OUT {
    vec3 position;
    vec3 normal;
    vec3 color;
    vec3 faceNormal;
} gsOut;


// geometry shader main
void main(void){
    // calculate face normal
    vec3 edgeA = gsIn[1].position - gsIn[0].position;
    vec3 edgeB = gsIn[2].position - gsIn[1].position;
    vec3 faceNormal = normalize(cross(edgeA, edgeB));

    // forward triangle including face normal
    for(int i = 0; i < 3; ++i){
        gl_Position = gl_in[i].gl_Position;
        gl_ClipDistance[0] = gl_in[i].gl_ClipDistance[0];
        gsOut.position = gsIn[i].position;
        gsOut.normal = gsIn[i].normal;
        gsOut.color = gsIn[i].color;
        gsOut.faceNormal = faceNormal;
        EmitVertex();
    }
    EndPrimitive();
}


#endif /* GEOMETRY_SHADER */


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FRAGMENT SHADER
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef FRAGMENT_SHADER


// fragment shader input
in GS_OUT {
    vec3 position;
    vec3 normal;
    vec3 color;
    vec3 faceNormal;
} fsIn;


// fragment shader output
layout (location = 0) out vec3 gBufferPosition;
layout (location = 1) out vec3 gBufferNormal;
layout (location = 2) out vec3 gBufferDiffuse;
layout (location = 3) out vec4 gBufferEmissionReflection;
layout (location = 4) out vec4 gBufferSpecularShininess;
layout (location = 5) out vec3 gBufferFaceNormal;


// uniforms
uniform vec3 diffuseColorMultiplier;
uniform vec3 specularColorMultiplier;
uniform vec3 emissionColorMultiplier;
uniform float shininessMultiplier;


// fragment shader main
void main(void){
    // calculate albedo color
    vec3 albedo = diffuseColorMultiplier * fsIn.color;

    // set G-buffer outputs
    gBufferPosition = fsIn.position;
    gBufferDiffuse = albedo;
    gBufferNormal = normalize(fsIn.normal);
    gBufferEmissionReflection = vec4(emissionColorMultiplier, 0.0f);
    gBufferSpecularShininess = vec4(albedo*specularColorMultiplier, shininessMultiplier);
    gBufferFaceNormal = normalize(fsIn.faceNormal);
}


#endif /* FRAGMENT_SHADER */

