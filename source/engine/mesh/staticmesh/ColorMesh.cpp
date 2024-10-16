#include <ColorMesh.hpp>
#include <StringHelper.hpp>
#include <PrismaConfiguration.hpp>


ColorMesh::ColorMesh(){
    vao = 0;
    vbo = 0;
    ebo = 0;
    numIndices = 0;
}

bool ColorMesh::Generate(void){
    if(vertices.empty() || indices.empty()){
        return false;
    }
    DEBUG_GLCHECK( glGenVertexArrays(1, &vao); );
    DEBUG_GLCHECK( glGenBuffers(1, &vbo); );
    DEBUG_GLCHECK( glGenBuffers(1, &ebo); );
    DEBUG_GLCHECK( glBindVertexArray(vao); );
        DEBUG_GLCHECK( glBindBuffer(GL_ARRAY_BUFFER, vbo); );
        DEBUG_GLCHECK( glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ColorMeshVertex), &(vertices[0]), GL_STATIC_DRAW); );
        DEBUG_GLCHECK( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); );
        DEBUG_GLCHECK( glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &(indices[0]), GL_STATIC_DRAW); );
        // Position
        DEBUG_GLCHECK( glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColorMeshVertex), (GLvoid*)0); );
        DEBUG_GLCHECK( glEnableVertexAttribArray(0); );
        // Normal
        DEBUG_GLCHECK( glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ColorMeshVertex), (GLvoid*)offsetof(ColorMeshVertex, normal)); );
        DEBUG_GLCHECK( glEnableVertexAttribArray(1); );
        // Color
        DEBUG_GLCHECK( glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ColorMeshVertex), (GLvoid*)offsetof(ColorMeshVertex, color)); );
        DEBUG_GLCHECK( glEnableVertexAttribArray(2); );
    DEBUG_GLCHECK( glBindVertexArray(0); );
    numIndices = static_cast<GLsizei>(indices.size());
    vertices.clear();
    indices.clear();
    return true;
}

void ColorMesh::Delete(void){
    if(vao){
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
    if(vbo){
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
    if(ebo){
        glDeleteBuffers(1, &ebo);
        ebo = 0;
    }
    numIndices = 0;
}

void ColorMesh::Draw(void){
    DEBUG_GLCHECK( glBindVertexArray(vao); );
    DEBUG_GLCHECK( glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, (GLvoid*)0); );
}

AABB ColorMesh::GetAABBOfVertices(void){
    return aabbVertices;
}

bool ColorMesh::ReadFromFile(std::string filename){
    aabbVertices.Clear();
    vertices.clear();
    indices.clear();
    std::ifstream file(filename, std::ifstream::binary);
    if(!file.is_open()){
        PrintE("Could not open file \"%s\"!\n", filename.c_str());
        return false;
    }
    if(!ParsePlyHeader(file)){
        PrintE("Could not parse header of ply file \"%s\"!\n", filename.c_str());
        return false;
    }
    if(!ParsePlyData(file)){
        PrintE("Could not parse data of ply file \"%s\"!\n", filename.c_str());
        return false;
    }
    return true;
}

bool ColorMesh::ParsePlyHeader(std::ifstream& file){
    bool success = false;
    elements.clear();
    try{
        std::string headerMagicNumber = ReadNextLine(file);
        if(0 != headerMagicNumber.compare("ply")){
            throw(0);
        }
        std::string headerFormat = ReadNextLine(file);
        if(0 != headerFormat.compare("format ascii 1.0")){
            throw(0);
        }
        while(file.good()){
            std::string line = ReadNextLine(file);
            std::vector<std::string> words = StringHelper::SplitString(line);
            if(words.empty() || (0 == words[0].compare("comment"))){
                continue;
            }
            else if(0 == words[0].compare("element")){
                if(3 != words.size()){
                    throw(0);
                }
                PlyElement element;
                element.name = words[1];
                element.number = static_cast<uint32_t>(std::stoul(words[2]));
                elements.push_back(element);
            }
            else if(0 == words[0].compare("property")){
                if(elements.empty() || (words.size() < 3)){
                    throw(0);
                }
                if((0 == elements.back().name.compare("face")) && (0 == words.back().compare("vertex_indices")) && (0 != words[1].compare("list"))){
                    throw(0);
                }
                if((3 == words.size()) && ((0 == words[2].compare("red")) || (0 == words[2].compare("green")) || (0 == words[2].compare("blue")) || (0 == words[2].compare("alpha"))) && ((0 == words[1].compare("float")) || (0 == words[1].compare("double")))){ // color format must not be floating point
                    throw(0);
                }
                elements.back().propertyNames.push_back(words.back());
            }
            else if(0 == words[0].compare("end_header")){
                success = true;
                break;
            }
        }
    }
    catch(...){ }
    return success;
}

bool ColorMesh::ParsePlyData(std::ifstream& file){
    std::vector<ColorMeshVertex> plyVertices;
    std::vector<GLuint> plyIndices;
    try{
        for(auto&& element : elements){
            for(size_t n = 0; n < element.number; ++n){
                if(!file.good()){
                    throw(0);
                }
                std::string line = ReadNextLine(file);
                std::vector<std::string> words = StringHelper::SplitString(line);
                if(0 == element.name.compare("vertex")){
                    if(element.propertyNames.size() != words.size()){
                        throw(0);
                    }
                    int32_t ix = GetPropertyIndex(element.propertyNames, "x");
                    int32_t iy = GetPropertyIndex(element.propertyNames, "y");
                    int32_t iz = GetPropertyIndex(element.propertyNames, "z");
                    int32_t inx = GetPropertyIndex(element.propertyNames, "nx");
                    int32_t iny = GetPropertyIndex(element.propertyNames, "ny");
                    int32_t inz = GetPropertyIndex(element.propertyNames, "nz");
                    int32_t ired = GetPropertyIndex(element.propertyNames, "red");
                    int32_t igreen = GetPropertyIndex(element.propertyNames, "green");
                    int32_t iblue = GetPropertyIndex(element.propertyNames, "blue");
                    ColorMeshVertex v = GetDefaultVertex();
                    if(ix >= 0) v.position[0] = static_cast<GLfloat>(std::stof(words[ix]));
                    if(iy >= 0) v.position[1] = static_cast<GLfloat>(std::stof(words[iy]));
                    if(iz >= 0) v.position[2] = static_cast<GLfloat>(std::stof(words[iz]));
                    if(inx >= 0) v.normal[0] = static_cast<GLfloat>(std::stof(words[inx]));
                    if(iny >= 0) v.normal[1] = static_cast<GLfloat>(std::stof(words[iny]));
                    if(inz >= 0) v.normal[2] = static_cast<GLfloat>(std::stof(words[inz]));
                    if(ired >= 0) v.color[0] = static_cast<GLfloat>(std::stod(words[ired]) / 255.0);
                    if(igreen >= 0) v.color[1] = static_cast<GLfloat>(std::stod(words[igreen]) / 255.0);
                    if(iblue >= 0) v.color[2] = static_cast<GLfloat>(std::stod(words[iblue]) / 255.0);
                    EnsureCorrectNormal(v);
                    if(prismaConfiguration.engine.convertSRGBToLinearRGB){
                        SRGBToLinearRGB(v);
                    }
                    plyVertices.push_back(v);
                }
                else if(0 == element.name.compare("face")){
                    if((1 != element.propertyNames.size()) || (0 != element.propertyNames[0].compare("vertex_indices")) || (4 != words.size()) || (0 != words[0].compare("3"))){
                        throw(0);
                    }
                    plyIndices.push_back(static_cast<GLuint>(std::stoul(words[1])));
                    plyIndices.push_back(static_cast<GLuint>(std::stoul(words[2])));
                    plyIndices.push_back(static_cast<GLuint>(std::stoul(words[3])));
                }
            }
        }
    }
    catch(...){
        return false;
    }
    vertices.swap(plyVertices);
    indices.swap(plyIndices);
    UpdateAABB();
    return true;
}

std::string ColorMesh::ReadNextLine(std::ifstream& file){
    std::string line;
    std::getline(file, line);
    line.erase(std::remove_if(line.begin(), line.end(), [](char c){ return ('\r' == c) || ('\n' == c); }), line.end());
    return line;
}

int32_t ColorMesh::GetPropertyIndex(const std::vector<std::string>& propertyNames, std::string name){
    int32_t result = -1;
    for(int32_t i = 0; i < static_cast<int32_t>(propertyNames.size()); ++i){
        if(0 == propertyNames[i].compare(name)){
            result = i;
            break;
        }
    }
    return result;
}

ColorMeshVertex ColorMesh::GetDefaultVertex(void){
    ColorMeshVertex vertex;
    vertex.position[0] = vertex.position[1] = vertex.position[2] = 0.0f;
    vertex.normal[0] = vertex.normal[1] = vertex.normal[2] = 0.0f;
    vertex.color[0] = vertex.color[1] = vertex.color[2] = 0.0f;
    return vertex;
}

void ColorMesh::UpdateAABB(void){
    aabbVertices.Clear();
    size_t numVertices = vertices.size();
    if(numVertices){
        glm::vec3 maxPosition;
        aabbVertices.lowestPosition.x = maxPosition.x = vertices[0].position[0];
        aabbVertices.lowestPosition.y = maxPosition.y = vertices[0].position[1];
        aabbVertices.lowestPosition.z = maxPosition.z = vertices[0].position[2];
        for(size_t n = 1; n < numVertices; ++n){
            aabbVertices.lowestPosition.x = std::min(aabbVertices.lowestPosition.x, vertices[n].position[0]);
            aabbVertices.lowestPosition.y = std::min(aabbVertices.lowestPosition.y, vertices[n].position[1]);
            aabbVertices.lowestPosition.z = std::min(aabbVertices.lowestPosition.z, vertices[n].position[2]);
            maxPosition.x = std::max(maxPosition.x, vertices[n].position[0]);
            maxPosition.y = std::max(maxPosition.y, vertices[n].position[1]);
            maxPosition.z = std::max(maxPosition.z, vertices[n].position[2]);
        }
        aabbVertices.dimension = maxPosition - aabbVertices.lowestPosition;
    }
}

void ColorMesh::EnsureCorrectNormal(ColorMeshVertex& vertex){
    GLfloat sx = vertex.normal[0] * vertex.normal[0];
    GLfloat sy = vertex.normal[1] * vertex.normal[1];
    GLfloat sz = vertex.normal[2] * vertex.normal[2];
    GLfloat squaredLength = sx + sy + sz;
    if(squaredLength > std::numeric_limits<GLfloat>::epsilon()){
        GLfloat L = std::sqrt(squaredLength);
        vertex.normal[0] /= L;
        vertex.normal[1] /= L;
        vertex.normal[2] /= L;
    }
    else{
        vertex.normal[0] = 1.0f;
        vertex.normal[1] = 0.0f;
        vertex.normal[2] = 0.0f;
    }
}

void ColorMesh::SRGBToLinearRGB(ColorMeshVertex& vertex){
    constexpr double gamma = 2.2;
    vertex.color[0] = static_cast<GLfloat>(std::pow(static_cast<double>(vertex.color[0]), gamma));
    vertex.color[1] = static_cast<GLfloat>(std::pow(static_cast<double>(vertex.color[1]), gamma));
    vertex.color[2] = static_cast<GLfloat>(std::pow(static_cast<double>(vertex.color[2]), gamma));
}

