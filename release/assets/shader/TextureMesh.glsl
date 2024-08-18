#include "CameraUniform.glsl"


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
    vec3 position;
    vec3 normal;
    vec3 tangent;
    vec2 texCoord;
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
    vsOut.tangent = normalMatrix * tangent;
    vsOut.texCoord = texCoord;

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
    vec3 tangent;
    vec2 texCoord;
} gsIn[];


// geometry shader output
out GS_OUT {
    vec3 position;
    vec3 normal;
    vec3 tangent;
    vec2 texCoord;
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
        gsOut.tangent = gsIn[i].tangent;
        gsOut.texCoord = gsIn[i].texCoord;
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
    vec3 tangent;
    vec2 texCoord;
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
layout (binding = 0) uniform sampler2D textureDiffuseMap;
layout (binding = 1) uniform sampler2D textureNormalMap;
layout (binding = 2) uniform sampler2D textureEmissionMap;
layout (binding = 3) uniform sampler2D textureSpecularMap;
uniform vec3 diffuseColor;
uniform vec3 emissionColor;
uniform vec3 specularColor;
uniform float shininess;


// constants
const float normalMapCenterRG = 128.0f / 255.0f;


// fragment shader main
void main(){
    // get texture maps
    vec4 diffuseMap = texture(textureDiffuseMap, fsIn.texCoord);
    vec3 normalMap = texture(textureNormalMap, fsIn.texCoord).rgb;
    vec3 emissionMap = texture(textureEmissionMap, fsIn.texCoord).rgb;
    vec3 specularMap = texture(textureSpecularMap, fsIn.texCoord).rgb;

    // alpha testing
    if(diffuseMap.a < 0.5){
        discard;
    }

    // calculate tangent-bitangent-normal matrix (TBN)
    vec3 normal = normalize(fsIn.normal);
    vec3 tangent = normalize(fsIn.tangent);
    vec3 bitangent = normalize(cross(normal, tangent));
    mat3 TBN = mat3(tangent, bitangent, normal);

    // transform normal from texture space to view space
    vec3 normalTextureSpace = normalize(vec3((normalMap.rg - vec2(normalMapCenterRG)) * 2.0f, normalMap.b));
    vec3 normalViewSpace = TBN * normalTextureSpace;
    if(!gl_FrontFacing){
        normalViewSpace = -normalViewSpace;
    }

    // set G-buffer outputs
    gBufferPosition = fsIn.position;
    gBufferDiffuse = diffuseColor * diffuseMap.rgb;
    gBufferNormal = normalViewSpace;
    gBufferEmissionReflection = vec4(emissionColor * emissionMap, 0.0f);
    gBufferSpecularShininess = vec4(specularColor * specularMap, shininess);
    gBufferFaceNormal = normalize(fsIn.faceNormal);
}


#endif /* FRAGMENT_SHADER */

