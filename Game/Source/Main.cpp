#include <precomp.h>


void FrameBufferResizeCallback(GLFWwindow *glfwWindow, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    Log::Init();
    Light::Init();
    
    Window::Window m_Window;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
    GLFWwindow *m_glfwWindow =
        glfwCreateWindow(WindowData::width, WindowData::height, WindowData::windowTitle, nullptr, nullptr);
    SMASSERT(m_glfwWindow != nullptr, "Failed to init glfwWindow");

    glfwMakeContextCurrent(m_glfwWindow);
    glfwSwapInterval(WindowData::vsync ? 1 : 0);  

    m_Window.m_glfwWindow = m_glfwWindow;
    SMASSERT( gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)), "Glad failed to load gl" );
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);

    glViewport( 0, 0, WindowData::width, WindowData::height );   
    glfwSetFramebufferSizeCallback( m_glfwWindow, FrameBufferResizeCallback); 

    Render::Renderer renderer(m_glfwWindow);  

    Input input;

    if (!input.Init(m_glfwWindow))
    {
        return 1;
    }

    while (!glfwWindowShouldClose(m_glfwWindow))
    {
        input.ProcessInput();
        renderer.Update();

        glfwSwapInterval(WindowData::vsync ? 1 : 0);
        glfwSwapBuffers(m_glfwWindow);
    }

    glfwDestroyWindow(m_glfwWindow);   
    glfwTerminate();
    return 0;
}
