#include <precomp.h>

#include <Render/Shader.h>

#include "Framebuffer.h"
#include "fbData.h"

namespace Render
{
    bool Framebuffer::Init()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Data.vertices), &Data.vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)nullptr);        

        shader = new Shader("screen");
        SASSERT(shader->Compile());

        shader->ActivateShader();
        shader->SetValue("screenTexture", 0);

        glGenFramebuffers(1, &nativeFb);
        glBindFramebuffer(GL_FRAMEBUFFER, nativeFb);

        glGenTextures(1, &textureColorBuffer);
        glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WindowData::width, WindowData::height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                     nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WindowData::width, WindowData::height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
        SMASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
                 "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return true;
    }

    Framebuffer::~Framebuffer()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteRenderbuffers(1, &RBO);
        delete shader;
    }

    void Framebuffer::BindSceneBegin()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, nativeFb);
        glEnable(GL_DEPTH_TEST);
    }

    void Framebuffer::BindSceneEnd()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);        

        shader->ActivateShader();
        shader->setVec2("frameBufSize", WindowData::W, WindowData::H);
        shader->SetValue("AA", WindowData::fxaa);

        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
        glDrawArrays(GL_TRIANGLES, 0, Data.n_triangles);
    }
} // namespace Render
