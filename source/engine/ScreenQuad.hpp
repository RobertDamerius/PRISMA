#pragma once


#include <Common.hpp>


class ScreenQuad {
    public:
        /**
         * @brief Construct a new screen quad.
         */
        ScreenQuad(): vao(0), vbo(0){}

        /**
         * @brief Generate vertex data.
         */
        void Generate(void){
            constexpr GLfloat quadVertices[] = {
                // Position   // TexCoord
                -1.0f,  1.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f,
                 1.0f,  1.0f, 1.0f, 1.0f,
                 1.0f, -1.0f, 1.0f, 0.0f
            };
            DEBUG_GLCHECK( glGenVertexArrays(1, &vao); );
            DEBUG_GLCHECK( glGenBuffers(1, &vbo); );
            DEBUG_GLCHECK( glBindVertexArray(vao); );
                DEBUG_GLCHECK( glBindBuffer(GL_ARRAY_BUFFER, vbo); );
                DEBUG_GLCHECK( glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices[0], GL_STATIC_DRAW); );
                DEBUG_GLCHECK( glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0); );
                DEBUG_GLCHECK( glEnableVertexAttribArray(0); );
                DEBUG_GLCHECK( glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat))); );
                DEBUG_GLCHECK( glEnableVertexAttribArray(1); );
            DEBUG_GLCHECK( glBindVertexArray(0); );
        }

        /**
         * @brief Delete vertex data.
         */
        void Delete(void){
            if(vbo){
                DEBUG_GLCHECK( glDeleteBuffers(1, &vbo); );
                vbo = 0;
            }
            if(vao){
                DEBUG_GLCHECK( glDeleteVertexArrays(1, &vao); );
                vao = 0;
            }
        }

        /**
         * @brief Draw the screen quad.
         */
        void Draw(void){
            DEBUG_GLCHECK( glBindVertexArray(vao); );
            DEBUG_GLCHECK( glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); );
        }

    private:
        GLuint vao;   // VAO for screen quad.
        GLuint vbo;   // VBO for screen quad.
};

