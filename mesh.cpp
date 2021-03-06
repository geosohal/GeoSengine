/*

Copyright 2011 Etay Meiri

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



#include "mesh.h"
//#include <glbinding/Binding.h>


using namespace std;

#define POSITION_LOCATION 0
#define TEX_COORD_LOCATION 1
#define NORMAL_LOCATION 2
#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)
#define GLCheckError() (glGetError() == GL_NO_ERROR)

BasicMesh::BasicMesh()
{
    m_VAO = 0;
    ZERO_MEM(m_Buffers);
}


BasicMesh::~BasicMesh()
{
    Clear();
}


void BasicMesh::Clear()
{
    for (unsigned int i = 0; i < m_Textures.size(); i++) {
        SAFE_DELETE(m_Textures[i]);
    }

    if (m_Buffers[0] != 0) {
        GL::glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
    }

    if (m_VAO != 0) {
        GL::glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
}


bool BasicMesh::LoadMesh(const string& Filename)
{
    textureUnit = GL_TEXTURE0;
    // Release the previously loaded mesh (if it exists)
    Clear();

    GL::glBindVertexArray(0);//test
    // Create the VAO
    GL::glGenVertexArrays(1, &m_VAO);
    GL::glBindVertexArray(m_VAO);

    // Create the buffers for the vertices attributes
    GL::glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

    bool Ret = false;
    Assimp::Importer Importer;

    const aiScene* pScene = Importer.ReadFile(Filename.c_str(), ASSIMP_LOAD_FLAGS);

                                              /*aiProcess_GenSmoothNormals |
                                              aiProcess_Triangulate |
                                              aiProcess_JoinIdenticalVertices |
                                              aiProcess_SortByPType
                                              );*/
    if (pScene) {
        Ret = InitFromScene(pScene, Filename);
        Ret = true;
    }
    else {
        printf("Error parsing '%s': '%s'\n", Filename.c_str(), Importer.GetErrorString());
        Ret = false;
    }

    // Make sure the VAO is not changed from the outside
    GL::glBindVertexArray(0);

    return Ret;
}

bool BasicMesh::InitFromScene(const aiScene* pScene, const string& Filename)
{
    m_Entries.resize(pScene->mNumMeshes);
    m_Textures.resize(pScene->mNumMaterials);

    vector<vec3> Positions;
    vector<vec3> Normals;
    vector<vec2> TexCoords;
    vector<unsigned int> Indices;

    unsigned int NumVertices = 0;
    unsigned int NumIndices = 0;

    // Count the number of vertices and indices
    for (unsigned int i = 0; i < m_Entries.size(); i++) {
        m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
        m_Entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
        m_Entries[i].BaseVertex = NumVertices;
        m_Entries[i].BaseIndex = NumIndices;

        NumVertices += pScene->mMeshes[i]->mNumVertices;
        NumIndices += m_Entries[i].NumIndices;
    }

    // Reserve space in the vectors for the vertex attributes and indices
    Positions.reserve(NumVertices);
    Normals.reserve(NumVertices);
    TexCoords.reserve(NumVertices);
    Indices.reserve(NumIndices);

    // Initialize the meshes in the scene one by one
    for (unsigned int i = 0; i < m_Entries.size(); i++) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitMesh(paiMesh, Positions, Normals, TexCoords, Indices);
    }

    if (!InitMaterials(pScene, Filename)) {
        return false;
    }

    // Generate and populate the buffers with vertex attributes and the indices
    GL::glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    GL::glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
    GL::glEnableVertexAttribArray(POSITION_LOCATION);
    GL::glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GL::glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
    GL::glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
    GL::glEnableVertexAttribArray(TEX_COORD_LOCATION);
    GL::glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

    GL::glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
    GL::glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
    GL::glEnableVertexAttribArray(NORMAL_LOCATION);
    GL::glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
    GL::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

    return GLCheckError();
}

// This function is responsible for loading each aiMesh structure that is contained in the aiScene.
void BasicMesh::InitMesh(const aiMesh* paiMesh,
    vector<vec3>& Positions,
    vector<vec3>& Normals,
    vector<vec2>& TexCoords,
    vector<unsigned int>& Indices)
{
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    // Populate the vertex attribute vectors
    for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {
        const aiVector3D* pPos = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

        Positions.push_back(vec3(pPos->x, pPos->y, pPos->z));
        Normals.push_back(vec3(pNormal->x, pNormal->y, pNormal->z));
        TexCoords.push_back(vec2(pTexCoord->x, pTexCoord->y));
    }

    // Populate the index buffer
    for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
        const aiFace& Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }
}

bool BasicMesh::InitMaterials(const aiScene* pScene, const string& Filename)
{
    // Extract the directory part from the file name
    string::size_type SlashIndex = Filename.find_last_of("/");
    string Dir;

    if (SlashIndex == string::npos) {
        Dir = ".";
    }
    else if (SlashIndex == 0) {
        Dir = "/";
    }
    else {
        Dir = Filename.substr(0, SlashIndex);
    }

    bool Ret = true;

    // Initialize the materials
    for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
        const aiMaterial* pMaterial = pScene->mMaterials[i];

        m_Textures[i] = NULL;

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString Path;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                string p(Path.data);

                if (p.substr(0, 2) == ".\\") {
                    p = p.substr(2, p.size() - 2);
                }

                string FullPath = Dir + "/" + p;

                //TODOG
                m_Textures[i] = new Texture(FullPath.c_str());

                if (!m_Textures[i]->Load()) {
                    printf("Error loading texture '%s'\n", FullPath.c_str());
                    delete m_Textures[i];
                    m_Textures[i] = NULL;
                    Ret = false;
                }
                else {
                    printf("Loaded texture '%s'\n", FullPath.c_str());
                }
				
            }
        }
//		else if (pMaterial->GetTextureCount(aiTextureType_SPECULAR))
//		{
//			float t;
//		}
//		else if (pMaterial->GetTextureCount(aiTextureType_SHININESS))
//		{
//			float ts;
//		}
    }

    return Ret;
}


void BasicMesh::Render()
{
    GL::glBindVertexArray(m_VAO);

    for (unsigned int i = 0; i < m_Entries.size(); i++) {
        const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;

        assert(MaterialIndex < m_Textures.size());

        if (m_Textures[MaterialIndex]) {
            m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
        }

        GL::getInstance()->glDrawElementsBaseVertex(GL_TRIANGLES,
            m_Entries[i].NumIndices,
            GL_UNSIGNED_INT,
            (void*)(sizeof(unsigned int) * m_Entries[i].BaseIndex),
            m_Entries[i].BaseVertex);
    }

    // Make sure the VAO is not changed from the outside
    GL::glBindVertexArray(0);
}

void BasicMesh::SetTextureUnit(GLenum tu)
{
    textureUnit = tu;
}

// for instanced rendering, note we need to change or add a new initfromscene function to use this.
void BasicMesh::Render(unsigned int NumInstances, const MAT4* WVPMats, const MAT4* WorldMats)
{
//    GL::glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[WVP_MAT_VB]);
//    GL::glBufferData(GL_ARRAY_BUFFER, sizeof(MAT4) * NumInstances, WVPMats, GL_DYNAMIC_DRAW);

//    GL::glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[WORLD_MAT_VB]);
//    GL::glBufferData(GL_ARRAY_BUFFER, sizeof(MAT4) * NumInstances, WorldMats, GL_DYNAMIC_DRAW);

//    GL::glBindVertexArray(m_VAO);

//    for (unsigned int i = 0; i < m_Entries.size(); i++) {
//        const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;

//        assert(MaterialIndex < m_Textures.size());

//        if (m_Textures[MaterialIndex]) {
//            m_Textures[MaterialIndex]->Bind(textureUnit);
//        }

//        GL::getInstance()->glDrawElementsInstancedBaseVertex(GL_TRIANGLES,
//            m_Entries[i].NumIndices,
//            GL_UNSIGNED_INT,
//            (void*)(sizeof(unsigned int) * m_Entries[i].BaseIndex),
//            NumInstances,
//            m_Entries[i].BaseVertex);
//    }

//    // Make sure the VAO is not changed from the outside
//    GL::glBindVertexArray(0);
}

