#pragma once


#include <Common.hpp>
#include <PrismaState.hpp>


/**
 * @brief Represents a G-buffer for deferred rendering.
 */
class GBuffer {
    public:
        /**
         * @brief Construct a G-buffer.
         */
        GBuffer(): width(0), height(0), cboPosition(0), cboNormal(0), cboDiffuse(0), cboEmissionReflection(0), cboSpecularShininess(0), cboFaceNormal(0), rbo(0), fbo(0){}

        /**
         * @brief Generate the G-buffer by generating framebuffer, colorbuffer and renderbuffer objects.
         * @param[in] width Width of the framebuffer in pixels.
         * @param[in] height Height of the framebuffer in pixels.
         * @return True if success, false otherwise.
         */
        bool Generate(GLsizei width, GLsizei height){
            return GenerateFramebuffer(width, height);
        }

        /**
         * @brief Delete the G-buffer.
         */
        void Delete(void){
            DeleteFramebuffer();
        }

        /**
         * @brief Resize the G-buffer.
         * @param[in] width Width of the framebuffer in pixels.
         * @param[in] height Height of the framebuffer in pixels.
         */
        void Resize(GLsizei width, GLsizei height){
            DeleteFramebuffer();
            (void) GenerateFramebuffer(width, height);
        }

        /**
         * @brief Use the G-buffer by binding the framebuffer and setting viewport and OpenGL states.
         */
        void Use(void){
            DEBUG_GLCHECK( glBindFramebuffer(GL_FRAMEBUFFER, fbo); );
            DEBUG_GLCHECK( glViewport(0, 0, width, height); );
            DEBUG_GLCHECK( glEnable(GL_CULL_FACE); );
            DEBUG_GLCHECK( glCullFace(GL_BACK); );
            DEBUG_GLCHECK( glEnable(GL_DEPTH_TEST); );
            DEBUG_GLCHECK( glDepthMask(GL_TRUE); );
            DEBUG_GLCHECK( glDepthFunc(GL_LEQUAL); );
            DEBUG_GLCHECK( glDisable(GL_STENCIL_TEST); );
            DEBUG_GLCHECK( glDisable(GL_BLEND); );
        }

        /**
         * @brief Clear color and depth of the framebuffer.
         * @param[in] cameraClipFar Far clipping plane of the camera to be used to set a clear color for the position buffer.
         */
        void ClearFramebuffer(GLfloat cameraClipFar){
            DEBUG_GLCHECK( glClearColor(0.0f, 0.0f, 0.0f, 0.0f); );
            DEBUG_GLCHECK( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); );
            ClearPositionColorBuffer(glm::vec3(0.0f, 0.0f, -1.1f * cameraClipFar - 1.0f));
        }

        /**
         * @brief Draw the scene to the G-buffer.
         * @param[in] prismaState The PRISMA state that contains the scene to be rendered.
         * @param[in] waterClipPlane The water clipping plane vector (a,b,c,d), where (a,b,c) is the normal vector and d is the distance of the plane from the origin, e.g. a*x + b*y + c*z + d = 0. The default value is glm::vec4(0.0f).
         */
        void DrawScene(PrismaState& prismaState, glm::vec4 waterClipPlane = glm::vec4(0.0f)){
            prismaState.meshLibrary.DrawMeshObjects(prismaState.camera.position, prismaState.camera.GetProjectionViewMatrix(), waterClipPlane);
            prismaState.dynamicMeshLibrary.DrawMeshObjects(prismaState.camera.position, prismaState.camera.GetProjectionViewMatrix(), waterClipPlane);
        }

        /**
         * @brief Draw the water mesh to the G-buffer.
         * @param[in] prismaState The PRISMA state containing the water mesh.
         * @param[in] reflectionTextureID Texture ID of that texture to be used as water reflection texture.
         * @param[in] refractionTextureID Texture ID of that texture to be used as water refraction texture.
         */
        void DrawWaterMesh(PrismaState& prismaState, GLuint reflectionTextureID, GLuint refractionTextureID){
            prismaState.waterMeshRenderer.DrawMesh(reflectionTextureID, refractionTextureID);
        }

        /**
         * @brief Get the position texture of the G-buffer.
         * @return Texture ID of the position colorbuffer.
         */
        GLuint GetPositionTexture(void) const { return cboPosition; }

        /**
         * @brief Get the normal texture of the G-buffer.
         * @return Texture ID of the normal colorbuffer.
         */
        GLuint GetNormalTexture(void) const { return cboNormal; }

        /**
         * @brief Get the diffuse texture of the G-buffer.
         * @return Texture ID of the diffuse colorbuffer.
         */
        GLuint GetDiffuseTexture(void) const { return cboDiffuse; }

        /**
         * @brief Get the emission/reflection indicator texture of the G-buffer.
         * @return Texture ID of the emission colorbuffer.
         */
        GLuint GetEmissionReflectionTexture(void) const { return cboEmissionReflection; }

        /**
         * @brief Get the specular/shininess texture of the G-buffer.
         * @return Texture ID of the specular/shininess colorbuffer.
         */
        GLuint GetSpecularShininessTexture(void) const { return cboSpecularShininess; }

        /**
         * @brief Get the face normal texture of the G-buffer.
         * @return Texture ID of the face normal colorbuffer.
         */
        GLuint GetFaceNormalTexture(void) const { return cboFaceNormal; }

        /**
         * @brief Get the ID of the render buffer.
         * @return ID of the render buffer.
         */
        GLuint GetRenderBuffer(void) const { return rbo; };

    private:
        GLsizei width;                  // The width of the framebuffer in pixels.
        GLsizei height;                 // The height of the framebuffer in pixels.
        GLuint cboPosition;             // Colorbuffer (view space position).
        GLuint cboNormal;               // Colorbuffer (view space normal).
        GLuint cboDiffuse;              // Colorbuffer (diffuse color).
        GLuint cboEmissionReflection;   // Colorbuffer (emission + reflection indicator).
        GLuint cboSpecularShininess;    // Colorbuffer (specular + shininess).
        GLuint cboFaceNormal;           // Colorbuffer (view space faceNormal).
        GLuint rbo;                     // Renderbuffer object.
        GLuint fbo;                     // The actual framebuffer object.

        /**
         * @brief Generate colorbuffers, renderbuffer and framebuffer and set @ref width and @ref height.
         * @param[in] width Width of the framebuffer in pixels.
         * @param[in] height Height of the framebuffer in pixels.
         * @return True if success, false otherwise.
         */
        bool GenerateFramebuffer(GLsizei width, GLsizei height){
            DEBUG_GLCHECK( glGenFramebuffers(1, &fbo); );
            DEBUG_GLCHECK( glBindFramebuffer(GL_FRAMEBUFFER, fbo); );
            // colorbuffer 0: view-space position (RGB)
            DEBUG_GLCHECK( glGenTextures(1, &cboPosition); );
            DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, cboPosition); );
            DEBUG_GLCHECK( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); );
            DEBUG_GLCHECK( glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cboPosition, 0); );
            // colorbuffer 1: view-space normal (RGB)
            DEBUG_GLCHECK( glGenTextures(1, &cboNormal); );
            DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, cboNormal); );
            DEBUG_GLCHECK( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); );
            DEBUG_GLCHECK( glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, cboNormal, 0); );
            // colorbuffer 2: diffuse color (RGB)
            DEBUG_GLCHECK( glGenTextures(1, &cboDiffuse); );
            DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, cboDiffuse); );
            DEBUG_GLCHECK( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); );
            DEBUG_GLCHECK( glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, cboDiffuse, 0); );
            // colorbuffer 3: emission color (RGB) + reflection indicator (A)
            DEBUG_GLCHECK( glGenTextures(1, &cboEmissionReflection); );
            DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, cboEmissionReflection); );
            DEBUG_GLCHECK( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); );
            DEBUG_GLCHECK( glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, cboEmissionReflection, 0); );
            // colorbuffer 4: specular (RGB) + shininess (A)
            DEBUG_GLCHECK( glGenTextures(1, &cboSpecularShininess); );
            DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, cboSpecularShininess); );
            DEBUG_GLCHECK( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); );
            DEBUG_GLCHECK( glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, cboSpecularShininess, 0); );
            // colorbuffer 5: view-space face normal (RGB)
            DEBUG_GLCHECK( glGenTextures(1, &cboFaceNormal); );
            DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, cboFaceNormal); );
            DEBUG_GLCHECK( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); );
            DEBUG_GLCHECK( glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, cboFaceNormal, 0); );
            // set color attachments
            const GLenum colorAttachments[6] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5};
            DEBUG_GLCHECK( glDrawBuffers(6, &colorAttachments[0]); );
            // rendering buffer for depth
            DEBUG_GLCHECK( glGenRenderbuffers(1, &rbo); );
            DEBUG_GLCHECK( glBindRenderbuffer(GL_RENDERBUFFER, rbo); );
            DEBUG_GLCHECK( glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height); );
            DEBUG_GLCHECK( glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo); );
            // check status
            if(GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER)){
                PrintE("Framebuffer for G-Buffer is not complete!\n");
                DeleteFramebuffer();
                return false;
            }
            this->width = width;
            this->height = height;
            return true;
        }

        /**
         * @brief Delete colorbuffers, renderbuffer and framebuffer object.
         */
        void DeleteFramebuffer(void){
            if(cboPosition){
                DEBUG_GLCHECK( glDeleteTextures(1, &cboPosition); );
                cboPosition = 0;
            }
            if(cboNormal){
                DEBUG_GLCHECK( glDeleteTextures(1, &cboNormal); );
                cboNormal = 0;
            }
            if(cboDiffuse){
                DEBUG_GLCHECK( glDeleteTextures(1, &cboDiffuse); );
                cboDiffuse = 0;
            }
            if(cboEmissionReflection){
                DEBUG_GLCHECK( glDeleteTextures(1, &cboEmissionReflection); );
                cboEmissionReflection = 0;
            }
            if(cboSpecularShininess){
                DEBUG_GLCHECK( glDeleteTextures(1, &cboSpecularShininess); );
                cboSpecularShininess = 0;
            }
            if(cboFaceNormal){
                DEBUG_GLCHECK( glDeleteTextures(1, &cboFaceNormal); );
                cboFaceNormal = 0;
            }
            if(rbo){
                DEBUG_GLCHECK( glDeleteRenderbuffers(1, &rbo); );
                rbo = 0;
            }
            if(fbo){
                DEBUG_GLCHECK( glDeleteFramebuffers(1, &fbo); );
                fbo = 0;
            }
            width = 0;
            height = 0;
        }

        /**
         * @brief Clear the position color buffer.
         * @param[in] clearColor The clear color to be used. The default value is glm::vec3(0.0f).
         */
        void ClearPositionColorBuffer(glm::vec3 clearColor = glm::vec3(0.0f)){
            DEBUG_GLCHECK( glClearTexImage(cboPosition, 0, GL_RGB, GL_FLOAT, glm::value_ptr(clearColor)); );
        }
};

