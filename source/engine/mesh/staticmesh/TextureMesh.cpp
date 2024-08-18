#include <TextureMesh.hpp>
#include <StringHelper.hpp>
#include <ShaderTextureMesh.hpp>


bool TextureMesh::Generate(void){
    for(auto&& submesh : submeshes){
        if(!submesh.Generate()){
            Delete();
            return false;
        }
    }
    return true;
}

void TextureMesh::Delete(void){
    for(auto&& submesh : submeshes){
        submesh.Delete();
    }
}

void TextureMesh::Draw(const ShaderTextureMesh& shader, glm::vec3 diffuseColorMultiplier, glm::vec3 specularColorMultiplier, glm::vec3 emissionColorMultiplier, GLfloat shininessMultiplier){
    for(auto&& submesh : submeshes){
        submesh.Draw(shader, diffuseColorMultiplier, specularColorMultiplier, emissionColorMultiplier, shininessMultiplier);
    }
}

void TextureMesh::DrawWithoutMaterial(void){
    for(auto&& submesh : submeshes){
        submesh.DrawWithoutMaterial();
    }
}

void TextureMesh::DrawWithoutMaterialButDiffuseMap(void){
    for(auto&& submesh : submeshes){
        submesh.DrawWithoutMaterialButDiffuseMap();
    }
}

AABB TextureMesh::GetAABBOfVertices(void){
    return aabbVertices;
}

bool TextureMesh::IsTransparent(void){
    bool isTransparent = false;
    for(auto&& mesh : submeshes){
        isTransparent |= mesh.material.diffuseMap.IsTransparent();
    }
    return isTransparent;
}

bool TextureMesh::ReadFromFile(std::string filename){
    aabbVertices.Clear();
    submeshes.clear();
    std::vector<TextureSubmesh> objects;
    try{
        std::ifstream file(filename, std::ifstream::binary);
        if(!file.is_open()){
            PrintE("Could not open file \"%s\"!\n", filename.c_str());
            return false;
        }
        std::vector<TextureSubmeshMaterial> materials;
        std::vector<std::array<GLfloat,3>> v, vn;
        std::vector<std::array<GLfloat,2>> vt;
        GLuint indexCounter = 0;
        while(file.good()){
            std::string line = ReadNextLine(file);
            std::vector<std::string> words = StringHelper::SplitString(line);
            if((words.empty()) || ('#' == words[0][0])){
                continue;
            }
            if(0 == words[0].substr(0,6).compare("mtllib")){
                std::string mtlFilename = line.substr(7);
                std::filesystem::path p(filename);
                p.replace_filename(mtlFilename);
                if(!ReadMaterialLibrary(materials, p.string())){
                    throw(0);
                }
            }
            else if(0 == words[0].compare("o")){
                if(2 != words.size()){
                    throw(0);
                }
                objects.push_back(TextureSubmesh());
                indexCounter = 0;
            }
            else if(0 == words[0].compare("v")){
                if((4 != words.size()) || objects.empty()){
                    throw(0);
                }
                GLfloat x = static_cast<GLfloat>(std::stof(words[1]));
                GLfloat y = static_cast<GLfloat>(std::stof(words[2]));
                GLfloat z = static_cast<GLfloat>(std::stof(words[3]));
                v.push_back({x, y, z});
            }
            else if(0 == words[0].compare("vn")){
                if((4 != words.size()) || objects.empty()){
                    throw(0);
                }
                GLfloat x = static_cast<GLfloat>(std::stof(words[1]));
                GLfloat y = static_cast<GLfloat>(std::stof(words[2]));
                GLfloat z = static_cast<GLfloat>(std::stof(words[3]));
                EnsureCorrectNormal(x, y, z);
                vn.push_back({x, y, z});
            }
            else if(0 == words[0].compare("vt")){
                if((3 != words.size()) || objects.empty()){
                    throw(0);
                }
                GLfloat s = static_cast<GLfloat>(std::stof(words[1]));
                GLfloat t = static_cast<GLfloat>(std::stof(words[2]));
                vt.push_back({s, t});
            }
            else if(0 == words[0].compare("usemtl")){
                if((2 != words.size()) || objects.empty()){
                    throw(0);
                }
                int64_t i = GetMaterialIndex(materials, words[1]);
                if(i < 0){
                    throw(0);
                }
                if(!objects.back().vertices.empty()){ // multiple materials for same object: create a new object instead
                    objects.push_back(TextureSubmesh());
                    indexCounter = 0;
                }
                objects.back().material = materials[i];
            }
            else if(0 == words[0].compare("f")){
                if((4 != words.size()) || objects.empty()){
                    throw(0);
                }
                std::array<uint32_t,3> u1, u2, u3;
                int n1, n2, n3;
                if((3 != std::sscanf(words[1].c_str(),"%u/%u/%u%n",&u1[0],&u1[1],&u1[2],&n1)) || (3 != std::sscanf(words[2].c_str(),"%u/%u/%u%n",&u2[0],&u2[1],&u2[2],&n2)) || (3 != std::sscanf(words[3].c_str(),"%u/%u/%u%n",&u3[0],&u3[1],&u3[2],&n3))){
                    throw(0);
                }
                if((n1 != static_cast<int>(words[1].size())) || (n2 != static_cast<int>(words[2].size())) || (n3 != static_cast<int>(words[3].size()))){
                    throw(0);
                }
                TextureMeshVertex v1, v2, v3;
                v1.position[0] = v[u1[0] - 1][0];
                v1.position[1] = v[u1[0] - 1][1];
                v1.position[2] = v[u1[0] - 1][2];
                v1.texCoord[0] = vt[u1[1] - 1][0];
                v1.texCoord[1] = vt[u1[1] - 1][1];
                v1.normal[0] = vn[u1[2] - 1][0];
                v1.normal[1] = vn[u1[2] - 1][1];
                v1.normal[2] = vn[u1[2] - 1][2];
                v2.position[0] = v[u2[0] - 1][0];
                v2.position[1] = v[u2[0] - 1][1];
                v2.position[2] = v[u2[0] - 1][2];
                v2.texCoord[0] = vt[u2[1] - 1][0];
                v2.texCoord[1] = vt[u2[1] - 1][1];
                v2.normal[0] = vn[u2[2] - 1][0];
                v2.normal[1] = vn[u2[2] - 1][1];
                v2.normal[2] = vn[u2[2] - 1][2];
                v3.position[0] = v[u3[0] - 1][0];
                v3.position[1] = v[u3[0] - 1][1];
                v3.position[2] = v[u3[0] - 1][2];
                v3.texCoord[0] = vt[u3[1] - 1][0];
                v3.texCoord[1] = vt[u3[1] - 1][1];
                v3.normal[0] = vn[u3[2] - 1][0];
                v3.normal[1] = vn[u3[2] - 1][1];
                v3.normal[2] = vn[u3[2] - 1][2];
                glm::vec3 edge12(v2.position[0] - v1.position[0], v2.position[1] - v1.position[1], v2.position[2] - v1.position[2]);
                glm::vec3 edge13(v3.position[0] - v1.position[0], v3.position[1] - v1.position[1], v3.position[2] - v1.position[2]);
                glm::vec2 deltaUV12(v2.texCoord[0] - v1.texCoord[0], v2.texCoord[1] - v1.texCoord[1]);
                glm::vec2 deltaUV13(v3.texCoord[0] - v1.texCoord[0], v3.texCoord[1] - v1.texCoord[1]);
                GLfloat f = 1.0f / (deltaUV12.x * deltaUV13.y - deltaUV13.x * deltaUV12.y);
                glm::vec3 tangent;
                tangent.x = f * (deltaUV13.y * edge12.x - deltaUV12.y * edge13.x);
                tangent.y = f * (deltaUV13.y * edge12.y - deltaUV12.y * edge13.y);
                tangent.z = f * (deltaUV13.y * edge12.z - deltaUV12.y * edge13.z);
                tangent = glm::normalize(tangent);
                v1.tangent[0] = v2.tangent[0] = v3.tangent[0] = tangent.x;
                v1.tangent[1] = v2.tangent[1] = v3.tangent[1] = tangent.y;
                v1.tangent[2] = v2.tangent[2] = v3.tangent[2] = tangent.z;
                objects.back().vertices.push_back(v1);
                objects.back().vertices.push_back(v2);
                objects.back().vertices.push_back(v3);
                objects.back().indices.push_back(indexCounter++);
                objects.back().indices.push_back(indexCounter++);
                objects.back().indices.push_back(indexCounter++);
            }
        }
    }
    catch(...){
        PrintE("Could not parse data of obj file \"%s\"!\n", filename.c_str());
        return false;
    }
    submeshes.swap(objects);
    UpdateAABB();
    return true;
}

bool TextureMesh::ReadMaterialLibrary(std::vector<TextureSubmeshMaterial>& materials, std::string filename){
    try{
        std::ifstream file(filename, std::ifstream::binary);
        if(!file.is_open()){
            PrintE("Could not open file \"%s\"!\n", filename.c_str());
            return false;
        }
        bool assignedKd = false;
        bool assignedKe = false;
        bool assignedKs = false;
        while(file.good()){
            std::string line = ReadNextLine(file);
            std::vector<std::string> words = StringHelper::SplitString(line);
            if((words.empty()) || ('#' == words[0][0])){
                continue;
            }
            if(0 == words[0].compare("newmtl")){
                if(2 != words.size()){
                    throw(0);
                }
                materials.push_back(TextureSubmeshMaterial());
                materials.back().name = words[1];
                assignedKd = assignedKe = assignedKs = false;
            }
            else if(0 == words[0].compare("Ns")){ // shininess
                if(materials.empty() || (2 != words.size())){
                    throw(0);
                }
                materials.back().shininess = std::stof(words[1]);
            }
            else if(0 == words[0].compare("Ka")){ // ambient color: ignored
            }
            else if(0 == words[0].compare("Ks")){ // specular color
                if(materials.empty() || (4 != words.size())){
                    throw(0);
                }
                materials.back().specularColor.r = std::stof(words[1]);
                materials.back().specularColor.g = std::stof(words[2]);
                materials.back().specularColor.b = std::stof(words[3]);
                assignedKs = true;
            }
            else if(0 == words[0].compare("Ke")){ // emission color
                if(materials.empty() || (4 != words.size())){
                    throw(0);
                }
                materials.back().emissionColor.r = std::stof(words[1]);
                materials.back().emissionColor.g = std::stof(words[2]);
                materials.back().emissionColor.b = std::stof(words[3]);
                assignedKe = true;
            }
            else if(0 == words[0].compare("Kd")){ // diffuse color
                if(materials.empty() || (4 != words.size())){
                    throw(0);
                }
                materials.back().diffuseColor.r = std::stof(words[1]);
                materials.back().diffuseColor.g = std::stof(words[2]);
                materials.back().diffuseColor.b = std::stof(words[3]);
                assignedKd = true;
            }
            else if(0 == words[0].compare("d")){ // opacity: ignored
            }
            else if(0 == words[0].compare("map_Kd")){ // diffuse map
                if(materials.empty()){
                    throw(0);
                }
                std::string imageFilename = line.substr(7);
                std::filesystem::path p(filename);
                p.replace_filename(imageFilename);
                if(!materials.back().diffuseMap.ReadFromPngFile(p.string(), true)){
                    throw(0);
                }
                if(!assignedKd){
                    materials.back().diffuseColor = glm::vec3(1.0f);
                }
            }
            else if(0 == words[0].compare("map_Ke")){ // emission map
                if(materials.empty()){
                    throw(0);
                }
                std::string imageFilename = line.substr(7);
                std::filesystem::path p(filename);
                p.replace_filename(imageFilename);
                if(!materials.back().emissionMap.ReadFromPngFile(p.string(), false)){
                    throw(0);
                }
                if(!assignedKe){
                    materials.back().emissionColor = glm::vec3(1.0f);
                }
            }
            else if(0 == words[0].compare("map_Ks")){ // specular map
                if(materials.empty()){
                    throw(0);
                }
                std::string imageFilename = line.substr(7);
                std::filesystem::path p(filename);
                p.replace_filename(imageFilename);
                if(!materials.back().specularMap.ReadFromPngFile(p.string(), false)){
                    throw(0);
                }
                if(!assignedKs){
                    materials.back().specularColor = glm::vec3(1.0f);
                }
            }
            else if((0 == words[0].compare("norm")) || (0 == words[0].compare("map_Kn"))){ // normal map
                if(materials.empty()){
                    throw(0);
                }
                std::string imageFilename = line.substr(7);
                std::filesystem::path p(filename);
                p.replace_filename(imageFilename);
                if(!materials.back().normalMap.ReadFromPngFile(p.string(), false)){
                    throw(0);
                }
            }
        }
    }
    catch(...){
        PrintE("Could not parse data of mtl file \"%s\"!\n", filename.c_str());
        return false;
    }
    return true;
}

std::string TextureMesh::ReadNextLine(std::ifstream& file){
    std::string line;
    std::getline(file, line);
    line.erase(std::remove_if(line.begin(), line.end(), [](char c){ return ('\r' == c) || ('\n' == c); }), line.end());
    return line;
}

int64_t TextureMesh::GetMaterialIndex(const std::vector<TextureSubmeshMaterial>& materials, std::string name){
    int64_t result = -1;
    for(int64_t i = 0; i < static_cast<int64_t>(materials.size()); ++i){
        if(0 == materials[i].name.compare(name)){
            result = i;
            break;
        }
    }
    return result;
}

void TextureMesh::UpdateAABB(void){
    aabbVertices.Clear();
    glm::vec3 maxPosition(-std::numeric_limits<double>::infinity());
    glm::vec3 minPosition(std::numeric_limits<double>::infinity());
    for(auto&& submesh : submeshes){
        for(auto&& vertex : submesh.vertices){
            minPosition.x = std::min(minPosition.x, vertex.position[0]);
            minPosition.y = std::min(minPosition.y, vertex.position[1]);
            minPosition.z = std::min(minPosition.z, vertex.position[2]);
            maxPosition.x = std::max(maxPosition.x, vertex.position[0]);
            maxPosition.y = std::max(maxPosition.y, vertex.position[1]);
            maxPosition.z = std::max(maxPosition.z, vertex.position[2]);
        }
    }
    if(std::isfinite(minPosition.x) && std::isfinite(minPosition.y) && std::isfinite(minPosition.z) && std::isfinite(maxPosition.x) && std::isfinite(maxPosition.y) && std::isfinite(maxPosition.z)){
        aabbVertices.lowestPosition = minPosition;
        aabbVertices.dimension = maxPosition - minPosition; 
    }
}

void TextureMesh::EnsureCorrectNormal(GLfloat& nx, GLfloat& ny, GLfloat& nz){
    GLfloat sx = nx * nx;
    GLfloat sy = ny * ny;
    GLfloat sz = nz * nz;
    GLfloat squaredLength = sx + sy + sz;
    if(squaredLength > std::numeric_limits<GLfloat>::epsilon()){
        GLfloat L = std::sqrt(squaredLength);
        nx /= L;
        ny /= L;
        nz /= L;
    }
    else{
        nx = 1.0f;
        ny = 0.0f;
        nz = 0.0f;
    }
}

