#pragma once

#ifdef _DEBUG
namespace Window { class DebugMenu; }
#endif

namespace Entities
{
    struct TerrainMesh
    {        
        std::vector<float>    position;
        std::vector<float>    normal;
        std::vector<float>    texCoords;
        std::vector<float>    tangent;
        std::vector<float>    bitangent;
        std::vector<unsigned> indices;
    };

    /* Terrain global vars and helper functions */
    class terrainData
    {
        inline static constexpr const char* terrainDir    = "Resources/Textures/terrain";        
        inline static constexpr const char* heightMapPath = "Resources/Textures/terrain/height_map.jpg";
        inline static constexpr const char* grassDir      = "Resources/Textures/terrain/grass";        
        inline static constexpr const char* mudDir        = "Resources/Textures/terrain/mud";
        

        /* Tranforming pixel interval [0, 255] to [-25, 25] */
        inline static float HeightFunction(float h) { return h / 255.f * 50.f - 25.f; }

        

#ifdef _DEBUG
        friend Window::DebugMenu;
#endif
        friend class Terrain;        
    };    
}