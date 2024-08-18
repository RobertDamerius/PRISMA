#pragma once


#include <Common.hpp>


/**
 * @brief Class: Shader
 * @details Handles vertex + geometry (optional) + fragment shader.
 */
class Shader {
    public:
        /**
         * @brief Create a shader object.
         */
        Shader(): programID(0){}

        /**
         * @brief Get the GLSL-version string to be used to generate shader.
         * @return The version string, e.g. "450".
         * @details Make sure that the OpenGL context is initialized to obtain correct version information.
         */
        static std::string GetShadingLanguageVersion(void);

        /**
         * @brief Generate the shader from a shader source file. The #include directive is used to insert the source of other shader source files (each
         * include file MUST be included only once).
         * @param[in] filename The filename of the shader source file.
         * @param[in] replacements Text replacement data containing from-to-pairs.
         * @param[in] version The version string number, e.g. "450". If this string is empty, the version is obtained by @ref GetShadingLanguageVersion.
         * @return True if success, false otherwise.
         * @details A zero terminator is added to the shader source code.
         */
        bool Generate(std::string filename, std::vector<std::pair<std::string, std::string>> replacements = {}, std::string version = std::string(""));

        /**
         * @brief Generate the shader from a specified list of files.
         * @param[in] filenames The filenames of the shader source files. All sources are concatenated in the given order and must not include a version.
         * @param[in] replacements Text replacement data containing from-to pairs.
         * @param[in] version The version string number, e.g. "450". If this string is empty, the version is obtained by @ref GetShadingLanguageVersion.
         * @return True if success, false otherwise.
         * @details A zero terminator is added to the shader source code.
         */
        bool GenerateFromFiles(std::vector<std::string> filenames, std::vector<std::pair<std::string, std::string>> replacements = {}, std::string version = std::string(""));

        /**
         * @brief Generate the shader.
         * @param[in] shaderSource The shader source code exluding version.
         * @param[in] replacements Text replacement data containing from-to-pairs.
         * @param[in] version The version string number, e.g. "450". If this string is empty, the version is obtained by @ref GetShadingLanguageVersion.
         * @return True if success, false otherwise.
         * @details A zero terminator is added to the shader source code.
         */
        bool GenerateFromSource(std::string shaderSource, std::vector<std::pair<std::string, std::string>> replacements = {}, std::string version = std::string(""));

        /**
         * @brief Delete the shader program.
         */
        void Delete(void);

        /**
         * @brief Use the shader.
         */
        void Use(void) const { DEBUG_GLCHECK( glUseProgram(programID); ); }

        /**
         * @brief Get the uniform location.
         * @param[in] name Name of the uniform.
         * @return Location of the uniform.
         */
        GLint GetUniformLocation(std::string name){ return glGetUniformLocation(programID, name.c_str()); }

        /* The uniforms */
        void UniformMatrix4fv(std::string name, GLboolean transpose, glm::mat4 matrix) const { DEBUG_GLCHECK( glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, transpose, glm::value_ptr(matrix)); ); }
        void UniformMatrix4fv(GLint location, GLboolean transpose, glm::mat4 matrix) const { DEBUG_GLCHECK( glUniformMatrix4fv(location, 1, transpose, glm::value_ptr(matrix)); ); }
        void UniformMatrix4fv(std::string name, GLboolean transpose, GLfloat* matrix) const { DEBUG_GLCHECK( glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, transpose, matrix); ); }
        void UniformMatrix4fv(GLint location, GLboolean transpose, GLfloat* matrix) const { DEBUG_GLCHECK( glUniformMatrix4fv(location, 1, transpose, matrix); ); }
        void UniformMatrix3fv(std::string name, GLboolean transpose, glm::mat3 matrix) const { DEBUG_GLCHECK( glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, transpose, glm::value_ptr(matrix)); ); }
        void UniformMatrix3fv(GLint location, GLboolean transpose, glm::mat3 matrix) const { DEBUG_GLCHECK( glUniformMatrix3fv(location, 1, transpose, glm::value_ptr(matrix)); ); }
        void UniformMatrix3fv(std::string name, GLboolean transpose, GLfloat* matrix) const { DEBUG_GLCHECK( glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, transpose, matrix); ); }
        void UniformMatrix3fv(GLint location, GLboolean transpose, GLfloat* matrix) const { DEBUG_GLCHECK( glUniformMatrix3fv(location, 1, transpose, matrix); ); }
        void UniformMatrix2fv(std::string name, GLboolean transpose, glm::mat2 matrix) const { DEBUG_GLCHECK( glUniformMatrix2fv(glGetUniformLocation(programID, name.c_str()), 1, transpose, glm::value_ptr(matrix)); ); }
        void UniformMatrix2fv(GLint location, GLboolean transpose, glm::mat2 matrix) const { DEBUG_GLCHECK( glUniformMatrix2fv(location, 1, transpose, glm::value_ptr(matrix)); ); }
        void UniformMatrix2fv(std::string name, GLboolean transpose, GLfloat* matrix) const { DEBUG_GLCHECK( glUniformMatrix2fv(glGetUniformLocation(programID, name.c_str()), 1, transpose, matrix); ); }
        void UniformMatrix2fv(GLint location, GLboolean transpose, GLfloat* matrix) const { DEBUG_GLCHECK( glUniformMatrix2fv(location, 1, transpose, matrix); ); }
        void Uniform4f(std::string name, glm::vec4 value) const { DEBUG_GLCHECK( glUniform4f(glGetUniformLocation(programID, name.c_str()), value.x, value.y, value.z, value.w); ); }
        void Uniform4f(GLint location, glm::vec4 value) const { DEBUG_GLCHECK( glUniform4f(location, value.x, value.y, value.z, value.w); ); }
        void Uniform4fv(std::string name, GLsizei count, const GLfloat* value) const { DEBUG_GLCHECK( glUniform4fv(glGetUniformLocation(programID, name.c_str()), count, value); ); }
        void Uniform4fv(GLint location, GLsizei count, const GLfloat* value) const { DEBUG_GLCHECK( glUniform4fv(location, count, value); ); }
        void Uniform3f(std::string name, glm::vec3 value) const { DEBUG_GLCHECK( glUniform3f(glGetUniformLocation(programID, name.c_str()), value.x, value.y, value.z); ); }
        void Uniform3f(GLint location, glm::vec3 value) const { DEBUG_GLCHECK( glUniform3f(location, value.x, value.y, value.z); ); }
        void Uniform3fv(std::string name, GLsizei count, const GLfloat* value) const { DEBUG_GLCHECK( glUniform3fv(glGetUniformLocation(programID, name.c_str()), count, value); ); }
        void Uniform3fv(GLint location, GLsizei count, const GLfloat* value) const { DEBUG_GLCHECK( glUniform3fv(location, count, value); ); }
        void Uniform2f(std::string name, glm::vec2 value) const { DEBUG_GLCHECK( glUniform2f(glGetUniformLocation(programID, name.c_str()), value.x, value.y); ); }
        void Uniform2f(GLint location, glm::vec2 value) const { DEBUG_GLCHECK( glUniform2f(location, value.x, value.y); ); }
        void Uniform2fv(std::string name, GLsizei count, const GLfloat* value) const { DEBUG_GLCHECK( glUniform2fv(glGetUniformLocation(programID, name.c_str()), count, value); ); }
        void Uniform2fv(GLint location, GLsizei count, const GLfloat* value) const { DEBUG_GLCHECK( glUniform2fv(location, count, value); ); }
        void Uniform1f(std::string name, GLfloat value) const { DEBUG_GLCHECK( glUniform1f(glGetUniformLocation(programID, name.c_str()), value); ); }
        void Uniform1f(GLint location, GLfloat value) const { DEBUG_GLCHECK( glUniform1f(location, value); ); }
        void Uniform1fv(std::string name, GLsizei count, const GLfloat* value) const { DEBUG_GLCHECK( glUniform1fv(glGetUniformLocation(programID, name.c_str()), count, value); ); }
        void Uniform1fv(GLint location, GLsizei count, const GLfloat* value) const { DEBUG_GLCHECK( glUniform1fv(location, count, value); ); }
        void Uniform4i(std::string name, GLint value0, GLint value1, GLint value2, GLint value3) const { DEBUG_GLCHECK( glUniform4i(glGetUniformLocation(programID, name.c_str()), value0, value1, value2, value3); ); }
        void Uniform4i(GLint location, GLint value0, GLint value1, GLint value2, GLint value3) const { DEBUG_GLCHECK( glUniform4i(location, value0, value1, value2, value3); ); }
        void Uniform4iv(std::string name, GLsizei count, const GLint* values) const { DEBUG_GLCHECK( glUniform4iv(glGetUniformLocation(programID, name.c_str()), count, values); ); }
        void Uniform4iv(GLint location, GLsizei count, const GLint* values) const { DEBUG_GLCHECK( glUniform4iv(location, count, values); ); }
        void Uniform3i(std::string name, GLint value0, GLint value1, GLint value2) const { DEBUG_GLCHECK( glUniform3i(glGetUniformLocation(programID, name.c_str()), value0, value1, value2); ); }
        void Uniform3i(GLint location, GLint value0, GLint value1, GLint value2) const { DEBUG_GLCHECK( glUniform3i(location, value0, value1, value2); ); }
        void Uniform3iv(std::string name, GLsizei count, const GLint* values) const { DEBUG_GLCHECK( glUniform3iv(glGetUniformLocation(programID, name.c_str()), count, values); ); }
        void Uniform3iv(GLint location, GLsizei count, const GLint* values) const { DEBUG_GLCHECK( glUniform3iv(location, count, values); ); }
        void Uniform2i(std::string name, GLint value0, GLint value1) const { DEBUG_GLCHECK( glUniform2i(glGetUniformLocation(programID, name.c_str()), value0, value1); ); }
        void Uniform2i(GLint location, GLint value0, GLint value1) const { DEBUG_GLCHECK( glUniform2i(location, value0, value1); ); }
        void Uniform2iv(std::string name, GLsizei count, const GLint* values) const { DEBUG_GLCHECK( glUniform2iv(glGetUniformLocation(programID, name.c_str()), count, values); ); }
        void Uniform2iv(GLint location, GLsizei count, const GLint* values) const { DEBUG_GLCHECK( glUniform2iv(location, count, values); ); }
        void Uniform1i(std::string name, GLint value) const { DEBUG_GLCHECK( glUniform1i(glGetUniformLocation(programID, name.c_str()), value); ); }
        void Uniform1i(GLint location, GLint value) const { DEBUG_GLCHECK( glUniform1i(location, value); ); }
        void Uniform1iv(std::string name, GLsizei count, const GLint* values) const { DEBUG_GLCHECK( glUniform1iv(glGetUniformLocation(programID, name.c_str()), count, values); ); }
        void Uniform1iv(GLint location, GLsizei count, const GLint* values) const { DEBUG_GLCHECK( glUniform1iv(location, count, values); ); }
        void UniformBlockBinding(std::string name, GLuint value) const { DEBUG_GLCHECK( glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, name.c_str()), value); ); }
        void UniformBlockBinding(GLint location, GLuint value) const { DEBUG_GLCHECK( glUniformBlockBinding(programID, location, value); ); }
        void Uniform1ui(std::string name, GLuint value) const { DEBUG_GLCHECK( glUniform1ui(glGetUniformLocation(programID, name.c_str()), value); ); }
        void Uniform1ui(GLint location, GLuint value) const { DEBUG_GLCHECK( glUniform1ui(location, value); ); }
        void Uniform2ui(std::string name, GLuint value0, GLuint value1) const { DEBUG_GLCHECK( glUniform2ui(glGetUniformLocation(programID, name.c_str()), value0, value1); ); }
        void Uniform2ui(GLint location, GLuint value0, GLuint value1) const { DEBUG_GLCHECK( glUniform2ui(location, value0, value1); ); }

    private:
        GLuint programID;   // The program ID.

        /**
         * @brief Append the source code of all additional sources that are found via the include directive.
         * @param[inout] shaderSource The string to which to append the source code to.
         * @param[in] includePath The current include path for file search.
         * @param[inout] alreadyIncludedFiles The set of all include files that have already been included.
         * @return True if success, false otherwise.
         */
        bool AppendAllAdditionalSources(std::string& shaderSource, std::filesystem::path includePath, std::unordered_set<std::string>& alreadyIncludedFiles);

        /**
         * @brief Find the next include directive (#include "something") from a source string.
         * @param[in] str The string to be searched.
         * @param[in] pos The starting index of the string from where to start the search.
         * @return A tuple containing the following values:
         * [0] Index to the starting point of the include directive or std::string::npos if no include directive was found.
         * [1] Index to the next character after the include directive.
         * [2] A string representing the content of the include directive, e.g. the filename to be included.
         */
        std::tuple<size_t, size_t, std::string> FindNextIncludeDirective(const std::string& str, size_t pos);

        /**
         * @brief Get the include path of a given include filename.
         * @param[in] filename The include filename for which to obtain the include path.
         * @return The include path.
         */
        std::filesystem::path GetIncludePath(std::string filename);

        /**
         * @brief Read a shader file and append the source to the output.
         * @param[out] outSource The output where to append the source code to.
         * @param[in] filename The name of the file to be read.
         * @return True if success, false otherwise.
         */
        bool AppendSourceFromFile(std::string& outSource, const std::string filename);

        /**
         * @brief Replace text in a string.
         * @param[inout] text The text that should be modified.
         * @param[in] replacement The replacements containing from-to pairs.
         */
        void ReplaceText(std::string& text, const std::vector<std::pair<std::string, std::string>>& replacement);

        /**
         * @brief Create a shader object.
         * @param[in] code The source code for the shader object.
         * @param[in] shaderType The shader type.
         * @return The ID of the shader object or 0 if the shader object could not be created.
         */
        GLuint CreateShader(std::string code, GLenum shaderType);

        /**
         * @brief Create a shader program.
         * @param[in] shaderIDs The list of shader objects to be attached to the program.
         * @return The ID of the shader program or 0 if the program could not be linked.
         */
        GLuint CreateProgram(std::vector<GLuint> shaderIDs);
};

