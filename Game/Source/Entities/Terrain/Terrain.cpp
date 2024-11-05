#include <precomp.h>
#include "terrainData.h"
#include <Entities/Material.h>

#include "Terrain.h"

float _defaultHeightFunction(float h) { return h; }
namespace Entities
{
    Terrain::Terrain(float scale) /* This texture will be on higher points */
        : Entity("terrain"),
        m_scale(scale),
        m_grass("material1"),
        m_mud(  "material2")
    {
        m_grass.Init(&shader, terrainData::grassDir, 256.f);
        m_mud  .Init(&shader, terrainData::mudDir, 256.f);
        m_LoadHeightMap(terrainData::heightMapPath);

        shader.ActivateShader();
        m_SetModelMatrix();
        setUniformPVM();
        if( m_grass.Bind(0) && m_mud.Bind(3) )
            LOG_INFO("Terrain materials initialized");
        LIGHT.SetAllLights(shader);        
        shader.DeactivateShader();
    }

    Terrain::~Terrain()
    {
        glDeleteBuffers(1, &m_EBO);
        glDeleteBuffers(1, &m_bitangentVBO);
        glDeleteBuffers(1, &m_tangentVBO);
        glDeleteBuffers(1, &m_texCoordsVBO);
        glDeleteBuffers(1, &m_normalVBO);        
        glDeleteBuffers(1, &m_positionVBO);
        glDeleteVertexArrays(1, &m_VAO);
    }

    

    void Terrain::Update(float delta)
    {
        shader.ActivateShader();

        shader.setMat4("PV", projection()*view);
        
        LIGHT.SetAllLights(shader);
        
        shader.setVec3("viewPos", CAMERA.GetCameraPos());

        glBindVertexArray(m_VAO);
        int index = m_grass.Activate(GL_TEXTURE0);
        index = m_mud.Activate(index);

        glDrawElements(GL_TRIANGLES, m_indicesNum, GL_UNSIGNED_INT, nullptr);

        Material::UnbindTextures(index);

        shader.DeactivateShader();
    }
    
    glm::vec3 Terrain::GetGradient(float x, float z) const
    {
        float h = GetHeight(x, z);

        constexpr static glm::vec2 positions[9] = {glm::vec2{0, 0},  glm::vec2{0, -1}, glm::vec2{1, -1},
                                                   glm::vec2{1, 0},  glm::vec2{1, 1},  glm::vec2{0, 1},
                                                   glm::vec2{-1, 1}, glm::vec2{-1, 0}, glm::vec2{-1, -1}};

        auto it = std::min_element(positions, positions + 9, 
            [&](const glm::vec2 &e1, const glm::vec2 &e2) {
            return GetHeight(x + e1[0], z + e1[1]) < GetHeight(x + e2[0], z + e2[1]);
        });

        float deltaH = h - GetHeight(x + it->x, z + it->y);

        if (deltaH < ms_gValue)
            return glm::vec3{0.f, 0.f, 0.f};

        return glm::normalize(glm::vec3{it->x, -deltaH, it->y});
    }


    float Terrain::m_GetHeightRaw(const int x, const int z) const
    {
        if (x < 0 || x >= m_size || z < 0 || z >= m_size)
            return 0;
        return m_heightMap[z][x];
    }

    /* Calculating height as mean value of four adjacent points */
    float Terrain::GetHeight(float x, float z) const
    {
        // translate back to model world
        x += m_halfSize;
        z += m_halfSize;
        x /= m_scale;
        z /= m_scale;

        int x1 = static_cast<int>(x);
        int x2 = static_cast<int>(x + 0.5f);
        int z1 = static_cast<int>(z);
        int z2 = static_cast<int>(z + 0.5f);

        float y1 = m_GetHeightRaw(x1, z1);
        float y2 = m_GetHeightRaw(x2, z1);
        float y3 = m_GetHeightRaw(x1, z2);
        float y4 = m_GetHeightRaw(x2, z2);

        return (y1 + y2 + y3 + y4) / 4.0f;
    }


    TerrainMesh Terrain::m_LoadMesh()
    {
        TerrainMesh mesh; // result                        

        for (int z = 0; z < m_size - 1; ++z)
        {
            for (int x = 0; x < m_size - 1; ++x) 
            {                
                m_LoadPNT(mesh, x, z);
                m_LoadTangents(mesh, x, z);
                m_LoadIndices(mesh, x, z);                                                
            }
            m_LoadPNT(mesh, m_size-1, z);
        }

        for(int x = 0; x < m_size; x++)
            m_LoadPNT(mesh, x, m_size - 1);

        m_indicesNum = mesh.indices.size();

        return mesh;
    }

    void Terrain::m_LoadPNT(TerrainMesh &mesh, int x, int z) const
    {
        mesh.position.push_back(static_cast<float>(x));
        mesh.position.push_back(m_heightMap[z][x]);
        mesh.position.push_back(static_cast<float>(z));

        glm::vec3 normal{m_CalculateNormal(x, z)};
        mesh.normal.push_back(normal.x);
        mesh.normal.push_back(normal.y);
        mesh.normal.push_back(normal.z);

        mesh.texCoords.push_back(x / ms_absH);
        mesh.texCoords.push_back(z / ms_absH);
    }

    void Terrain::m_LoadTangents(TerrainMesh &mesh, int x, int z) const
    {
        // positions
        glm::vec3 pos1(x, m_heightMap[z][x], z);
        glm::vec3 pos2(x, m_heightMap[z+1][x], z+1);
        glm::vec3 pos3(x+1, m_heightMap[z+1][x+1], z+1);
        glm::vec3 pos4(x+1, m_heightMap[z][x+1], z);

        // texture coordinates
        glm::vec2 uv1(x / ms_absH, z / ms_absH);
        glm::vec2 uv2(x / ms_absH, (z+1) / ms_absH);
        glm::vec2 uv3((x+1) / ms_absH, (z+1) / ms_absH);
        glm::vec2 uv4((x+1) / ms_absH, z / ms_absH);
        
        
        // calculate tangent/bitangent vectors of both triangles
        glm::vec3 tangent1, bitangent1;
        glm::vec3 tangent2, bitangent2;
        // triangle 1
        // ----------
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);            

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent1 = glm::normalize(tangent1);

        mesh.tangent.push_back(tangent1.x);
        mesh.tangent.push_back(tangent1.y);
        mesh.tangent.push_back(tangent1.z);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent1 = glm::normalize(bitangent1);

        mesh.bitangent.push_back(bitangent1.x);
        mesh.bitangent.push_back(bitangent1.y);
        mesh.bitangent.push_back(bitangent1.z);

        // triangle 2
        // ----------
        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent2 = glm::normalize(tangent2);

        mesh.tangent.push_back(tangent2.x);
        mesh.tangent.push_back(tangent2.y);
        mesh.tangent.push_back(tangent2.z);

        bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent2 = glm::normalize(bitangent2);

        mesh.bitangent.push_back(bitangent2.x);
        mesh.bitangent.push_back(bitangent2.y);
        mesh.bitangent.push_back(bitangent2.z);
    }

    void Terrain::m_LoadIndices(TerrainMesh &mesh, int x, int z) const
    {
        int topLeft     = z * m_size + x;
        int topRight    = topLeft + 1;
        int bottomLeft  = topLeft + m_size;
        int bottomRight = bottomLeft + 1;
        mesh.indices.push_back(topLeft);
        mesh.indices.push_back(bottomLeft);
        mesh.indices.push_back(topRight);
        mesh.indices.push_back(topRight);
        mesh.indices.push_back(bottomLeft);
        mesh.indices.push_back(bottomRight);
    }

    void Terrain::m_Smooth()
    {
        double start = glfwGetTime();
        /* Smoothing terrain surface using Gaussian blur */
        m_heightMap = std::move(Math::GaussianSmoothing(m_heightMap));
        LOG_TRACE("Finished in {}s", glfwGetTime() - start);
        m_size      = std::min(m_heightMap.size(), m_heightMap[0].size()); // Keeping map squared        
        m_halfSize  = static_cast<int>(m_scale * m_size / 2);


        // TODO: Delete
        float hmMin = 100.f;
        float hmMax = -100.f;
        for(int i = 0; i < m_size; i++)
            for(int j = 0; j < m_size; j++)
            {
                if(m_heightMap[i][j] > hmMax) 
                    hmMax = m_heightMap[i][j];
                if(m_heightMap[i][j] < hmMin)
                    hmMin = m_heightMap[i][j];
            }

        LOG_TRACE("Max: {}, Min: {}", hmMax, hmMin);
    }


    void Terrain::m_LoadHeightMap(const char *path)
    {
        auto [width, height, channels, data] = Render::Texture::LoadRawImage(path, STBI_grey);

        if (width != height)
        {
            LOG_ERROR("Bad dimensions for height map!");
            return;
        }

        m_size     = width;
        m_halfSize = static_cast<int>(m_scale * m_size / 2); // Recalculate middle

        m_heightMap = Math::Matrix<float>(m_size, Math::Array<float>(m_size, 0));


        for (int i = 0; i < m_size; ++i)
        {
            for (int j = 0; j < m_size; ++j)
            {
                m_heightMap[i][j] = terrainData::HeightFunction(data[i * m_size + j]);
            }
        }

        m_Smooth();
        
        m_SetModelMatrix();

        m_Init();
    }

    void Terrain::m_Init()
    {
        const auto& [position, 
                    normal, 
                    texCoords, 
                    tangent, 
                    bitangent, 
                    indices] {m_LoadMesh()};

        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);

        // Position
        glGenBuffers(1, &m_positionVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_positionVBO);
        glBufferData(GL_ARRAY_BUFFER, position.size() * sizeof(float), position.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        // Normal
        glGenBuffers(1, &m_normalVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_normalVBO);
        glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof(float), normal.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        // TexCoords
        glGenBuffers(1, &m_texCoordsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_texCoordsVBO);
        glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), texCoords.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        
        // Tangent
        glGenBuffers(1, &m_tangentVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_tangentVBO);
        glBufferData(GL_ARRAY_BUFFER, tangent.size() * sizeof(float), tangent.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        // Bitangent
        glGenBuffers(1, &m_bitangentVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_bitangentVBO);
        glBufferData(GL_ARRAY_BUFFER, bitangent.size() * sizeof(float), bitangent.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, nullptr);        

        // Indices
        glGenBuffers(1, &m_EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    }


    glm::vec3 Terrain::m_CalculateNormal(int x, int z) const
    {
        float heightL = x - 1 >= 0 ? m_heightMap[z][x - 1] : 0;
        float heightR = x + 1 < m_size ? m_heightMap[z][x + 1] : 0;
        float heightD = z - 1 >= 0 ? m_heightMap[z - 1][x] : 0;
        float heightU = z + 1 < m_size ? m_heightMap[z + 1][x] : 0;        

        return glm::normalize(glm::vec3{heightL - heightR, 2.0f, heightD - heightU});
    }

    void Terrain::m_SetModelMatrix()
    {
        model = glm::mat4(m_scale,            0.0f,   0.0f,              0.0f, 
                          0.0f,               1.0f,   0.0f,              0.0f, 
                          0.0f,               0.0f,   m_scale,           0.0f,
                          -1.0f * m_halfSize, 0.0f,   -1.0 * m_halfSize, 1.0f);        
    }

    void Terrain::CorrectPosition(float &x, float &z) const
    {
        static const float fHalfSize = static_cast<float>(m_halfSize);

        x = std::max(x, -fHalfSize + 10);
        x = std::min(x, fHalfSize - 10);
        z = std::max(z, -fHalfSize + 10);
        z = std::min(z, fHalfSize - 10);
    }

} // namespace Entities