#include <precomp.h>

#include <Render/PostProcessing/Framebuffer.h>


Render::Renderer::Renderer(GLFWwindow *m_glfwWindow)
 : m_glfwWindow(m_glfwWindow)
{
    m_FrameBuff = new Framebuffer();
    if (!m_FrameBuff->Init())
        exit(1);
    m_EntryManager = new Entities::EntityManager();
    if (!m_EntryManager->Init())
        exit(1);

#ifdef _DEBUG
    m_DebugWindow.Init(m_EntryManager, m_glfwWindow);
#endif
    
    lastTime = static_cast<float>( glfwGetTime() );

    LOG_INFO("Renderer initialized");
}

void Render::Renderer::Update()
{   
    float timeValue = static_cast<float>( glfwGetTime() );
    float delta = timeValue - lastTime;
    lastTime = timeValue;    

    m_FrameBuff->BindSceneBegin();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_EntryManager->Update(delta);

    m_FrameBuff->BindSceneEnd();

#ifdef _DEBUG
    m_DebugWindow.Update();
#endif
}

Render::Renderer::~Renderer()
{   
    delete m_FrameBuff;
    delete m_EntryManager;
#ifdef _DEBUG        
    m_DebugWindow.Destroy();    
#endif
}