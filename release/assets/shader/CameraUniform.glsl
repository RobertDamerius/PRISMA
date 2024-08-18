// camera uniform block
layout (std140, binding = $UBO_CAMERA$) uniform UBOCamera {
    mat4 cameraViewMatrix;            // view matrix V
    mat4 cameraProjectionMatrix;      // projection matrix P
    mat4 cameraInvProjectionMatrix;   // inverse projection matrix inv(P)
    vec4 cameraClippingInfo;          // clipping plane information (x: near, y: far)
};

