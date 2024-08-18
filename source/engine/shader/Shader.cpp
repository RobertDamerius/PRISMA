#include <Shader.hpp>


std::string Shader::GetShadingLanguageVersion(void){
    // get string that looks like "4.40 - Build 21.20.16.4727"
    std::string strGLSL((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

    // only use substring to first " "
    std::size_t pos = strGLSL.find_first_of(" ");
    if(std::string::npos != pos){
        strGLSL = strGLSL.substr(0, pos);
    }

    // only use numbers
    std::string result;
    for(auto c : strGLSL){
        if((c >= '0') && (c <= '9'))
            result.push_back(c);
    }
    return result;
}

bool Shader::Generate(std::string filename, std::vector<std::pair<std::string, std::string>> replacements, std::string version){
    std::unordered_set<std::string> alreadyIncludedFiles;
    alreadyIncludedFiles.insert(filename);
    std::string shaderSource;
    if(!AppendSourceFromFile(shaderSource, filename)){
        return false;
    }
    if(!AppendAllAdditionalSources(shaderSource, GetIncludePath(filename), alreadyIncludedFiles)){
        return false;
    }
    return GenerateFromSource(shaderSource, replacements, version);
}

bool Shader::GenerateFromFiles(std::vector<std::string> filenames, std::vector<std::pair<std::string, std::string>> replacements, std::string version){
    if(filenames.empty()){
        PrintE("No files are given to generate the shader!\n");
        return false;
    }
    std::string shaderSource;
    for(auto&& filename : filenames){
        if(!AppendSourceFromFile(shaderSource, filename)){
            return false;
        }
    }
    return GenerateFromSource(shaderSource, replacements, version);
}

bool Shader::GenerateFromSource(std::string shaderSource, std::vector<std::pair<std::string, std::string>> replacements, std::string version){
    ReplaceText(shaderSource, replacements);
    shaderSource.push_back(0x00);

    // prefix management
    if(version.empty()){
        version = GetShadingLanguageVersion();
    }
    std::string prefixVersion = std::string("#version ") + version + std::string("\n");
    std::string prefixVS = prefixVersion + std::string("#define VERTEX_SHADER\n");
    std::string prefixGS = prefixVersion + std::string("#define GEOMETRY_SHADER\n");
    std::string prefixFS = prefixVersion + std::string("#define FRAGMENT_SHADER\n");

    // create vertex, geometry and fragment shader
    bool error = false;
    GLuint idShaderVS, idShaderGS = 0, idShaderFS;
    idShaderVS = CreateShader(prefixVS + shaderSource, GL_VERTEX_SHADER);
    error |= !idShaderVS;
    if(std::string::npos != shaderSource.find("GEOMETRY_SHADER")){
        idShaderGS = CreateShader(prefixGS + shaderSource, GL_GEOMETRY_SHADER);
        error |= !idShaderGS;
    }
    idShaderFS = CreateShader(prefixFS + shaderSource, GL_FRAGMENT_SHADER);
    error |= !idShaderFS;

    // create shader program
    if(!error){
        programID = CreateProgram({idShaderVS, idShaderGS, idShaderFS});
        error |= !programID;
    }

    // flag attached shaders for deletion
    DEBUG_GLCHECK( glDeleteShader(idShaderVS); );
    if(idShaderGS){
        DEBUG_GLCHECK( glDeleteShader(idShaderGS); );
    }
    DEBUG_GLCHECK( glDeleteShader(idShaderFS); );
    return !error;
}

void Shader::Delete(void){
    if(programID){
        DEBUG_GLCHECK( glDeleteProgram(programID); );
        programID = 0;
    }
}

bool Shader::AppendAllAdditionalSources(std::string& shaderSource, std::filesystem::path includePath, std::unordered_set<std::string>& alreadyIncludedFiles){
    std::string result;
    size_t pos = 0;
    while(std::string::npos != pos){
        auto [a, b, includeFilename] = FindNextIncludeDirective(shaderSource, pos);
        if(std::string::npos == a){
            result += shaderSource.substr(pos);
            break;
        }
        std::string absoluteIncludeFilename = (includePath / includeFilename).string();
        auto [tmp, inserted] = alreadyIncludedFiles.insert(absoluteIncludeFilename);
        if(!inserted){
            PrintE("Detected circular dependency or multiple includes of shader source file \"%s\"!\n",absoluteIncludeFilename.c_str());
            return false;
        }
        std::string newShaderSource;
        if(!AppendSourceFromFile(newShaderSource, absoluteIncludeFilename)){
            return false;
        }
        if(!AppendAllAdditionalSources(newShaderSource, GetIncludePath(absoluteIncludeFilename), alreadyIncludedFiles)){
            return false;
        }
        result += shaderSource.substr(pos, a - pos);
        result += newShaderSource;
        pos = b;
    }
    shaderSource.swap(result);
    return true;
}

std::tuple<size_t, size_t, std::string> Shader::FindNextIncludeDirective(const std::string& str, size_t pos){
    size_t posStart = std::string::npos;
    size_t posEnd = std::string::npos;
    std::string includeFilename;
    while(pos != std::string::npos){
        pos = str.find_first_of("/#", pos);
        if(pos == std::string::npos){
            break;
        }
        if(('/' == str[pos]) && ((pos + 1) < str.size())){
            pos += 2;
            if(('/' == str[pos-2]) && ('/' == str[pos-1])){
                while(pos != std::string::npos){
                    pos = str.find_first_of("\\\n", pos);
                    if(pos != std::string::npos){
                        pos++;
                        if(('\\' == str[pos - 1]) && ((pos + 1) < str.size()) && ('\n' == str[pos])){
                            pos++;
                            continue;
                        }
                        break;
                    }
                }
            }
            else if(('/' == str[pos-2]) && ('*' == str[pos-1])){
                pos = str.find("*/", pos);
                if(pos != std::string::npos){
                    pos += 2;
                }
            }
        }
        else if('#' == str[pos]){
            if(0 == str.substr(pos, 8).compare("#include")){
                size_t a = str.find_first_of("\"", pos + 8);
                size_t b = str.find_first_of("\"", a + 1);
                if((a == std::string::npos) || (b == std::string::npos)){
                    pos = std::string::npos;
                }
                else{
                    posStart = pos;
                    posEnd = b + 1;
                    includeFilename = str.substr(a + 1, b - a - 1);
                    break;
                }
            }
            else{
                pos++;
            }
        }
    }
    return std::make_tuple(posStart, posEnd, includeFilename);
}

std::filesystem::path Shader::GetIncludePath(std::string filename){
    std::filesystem::path includePath;
    try {
        includePath = std::filesystem::canonical(filename);
        includePath.remove_filename();
    }
    catch(...){ }
    return includePath;
}

bool Shader::AppendSourceFromFile(std::string& outSource, const std::string filename){
    std::ifstream file(filename);
    if(!file.is_open()){
        PrintE("Could not open shader source file \"%s\"\n", filename.c_str());
        return false;
    }
    std::string shaderSource((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    outSource += shaderSource;
    return true;
}

void Shader::ReplaceText(std::string& text, const std::vector<std::pair<std::string, std::string>>& replacement){
    for(auto&& fromToPair : replacement){
        size_t start_pos = 0;
        while((start_pos = text.find(fromToPair.first, start_pos)) != std::string::npos){
            text.replace(start_pos, fromToPair.first.length(), fromToPair.second);
            start_pos += fromToPair.second.length();
        }
    }
}

GLuint Shader::CreateShader(std::string code, GLenum shaderType){
    std::string verboseName;
    switch(shaderType){
        case GL_VERTEX_SHADER:     verboseName = "vertex";     break;
        case GL_GEOMETRY_SHADER:   verboseName = "geometry";   break;
        case GL_FRAGMENT_SHADER:   verboseName = "fragment";   break;
    }
    GLuint shaderID = glCreateShader(shaderType);
    if(shaderID){
        GLint success;
        const GLchar* src = static_cast<const GLchar*>(code.c_str());
        const GLint len = static_cast<GLint>(code.length());
        DEBUG_GLCHECK( glShaderSource(shaderID, 1, &src, &len); );
        DEBUG_GLCHECK( glCompileShader(shaderID); );
        DEBUG_GLCHECK( glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success); );
        if(!success){
            GLint logSize = 0;
            DEBUG_GLCHECK( glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logSize); );
            if(logSize){
                GLchar *infoLog = new GLchar[logSize];
                DEBUG_GLCHECK( glGetShaderInfoLog(shaderID, logSize, nullptr, infoLog); );
                PrintE("Could not compile %s shader:\n%s\n", verboseName.c_str(), (const char*)infoLog);
                delete[] infoLog;
            }
            else{
                PrintE("Could not get log size for shader information!\n");
            }
            DEBUG_GLCHECK( glDeleteShader(shaderID); );
            shaderID = 0;
        }
    }
    return shaderID;
}

GLuint Shader::CreateProgram(std::vector<GLuint> shaderIDs){
    GLuint progID = glCreateProgram();
    if(progID){
        GLint success;
        for(auto&& shaderID : shaderIDs){
            if(shaderID){
                DEBUG_GLCHECK( glAttachShader(progID, shaderID); );
            }
        }
        DEBUG_GLCHECK( glLinkProgram(progID); );
        DEBUG_GLCHECK( glGetProgramiv(progID, GL_LINK_STATUS, &success); );
        if(!success){
            GLint logSize = 0;
            DEBUG_GLCHECK( glGetProgramiv(progID, GL_INFO_LOG_LENGTH, &logSize); );
            if(logSize){
                GLchar *infoLog = new GLchar[logSize];
                DEBUG_GLCHECK( glGetShaderInfoLog(progID, logSize, nullptr, infoLog); );
                PrintE("\nCould not link shader program:\n%s\n", (const char*)infoLog);
                delete[] infoLog;
            }
            else{
                PrintE("Could not get log size for shader program information!\n");
            }
            DEBUG_GLCHECK( glDeleteProgram(progID); );
            progID = 0;
        }
    }
    return progID;
}

