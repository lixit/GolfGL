#include <precomp.h>

#include "Skybox.h"
#include "skyboxData.h"

Entities::Skybox::Skybox() : Entity("skybox")
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyData.Vertices), &skyData.Vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    m_textureID = m_LoadCubemap();
    shader.ActivateShader();
    m_SetPV();    
    shader.SetValue("skybox", 0);
    shader.DeactivateShader();
}

Entities::Skybox::~Skybox()
{
    glDeleteTextures(1, &m_textureID);
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

void Entities::Skybox::Update(float delta)
{
    glDepthFunc(GL_LEQUAL);

    shader.ActivateShader();
    
    m_SetPV();

    glBindVertexArray(m_VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
    glDrawArrays(GL_TRIANGLES, 0, skyData.n_triangles);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
    shader.DeactivateShader();
}

unsigned Entities::Skybox::m_LoadCubemap() const
{
    using rTex = Render::Texture;

    unsigned id          = rTex::GenID(GL_TEXTURE_CUBE_MAP);
    unsigned textureXPos = GL_TEXTURE_CUBE_MAP_POSITIVE_X;

    for (auto &face : skyData.faces)
    {
        auto rawIm{rTex::LoadRawImage(face)};
        if (rawIm.data == nullptr)
        {
            std::cout << "Cubemap m_textureID failed to load at path: " << face << std::endl;
            return 0;
        }

        glTexImage2D(textureXPos++, 0, GL_RGB, rawIm.width, rawIm.height, 0, GL_RGB, GL_UNSIGNED_BYTE, rawIm.data);
    }

    rTex::SetTexParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR, GL_TEXTURE_CUBE_MAP);
    rTex::SetTexParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR, GL_TEXTURE_CUBE_MAP);
    rTex::SetTexParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE, GL_TEXTURE_CUBE_MAP);
    rTex::SetTexParam(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE, GL_TEXTURE_CUBE_MAP);
    rTex::SetTexParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE, GL_TEXTURE_CUBE_MAP);

    return id;
}

void Entities::Skybox::m_SetPV() const
{
    shader.setMat4("PV",
                   projection() * glm::mat4{   
                       view[0][0], view[0][1], view[0][2], 0.f, 
                       view[1][0], view[1][1], view[1][2], 0.f,
                       view[2][0], view[2][1], view[2][2], 0.f, 
                       0.f,        0.f,        0.f,        0.f});
}
