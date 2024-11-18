#include <precomp.h>


void FrameBufferResizeCallback(GLFWwindow *glfwWindow, int width, int height)
{
    glViewport(0, 0, width, height);
    WindowData::W = width;
    WindowData::H = height;
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
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    GLFWwindow *window =
        glfwCreateWindow(1920, 1080, WindowData::windowTitle, nullptr, nullptr);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(WindowData::vsync ? 1 : 0);  

    m_Window.m_glfwWindow = window;

    SMASSERT( gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)), "Glad failed to load gl" );
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);

    glViewport( 0, 0, 1920, 1080 );   
    glfwSetFramebufferSizeCallback( window, FrameBufferResizeCallback); 

    Render::Renderer renderer(window);  

    Input input;

    if (!input.Init(window))
    {
        return 1;
    }
    
    while (!glfwWindowShouldClose(window))
    {
        input.ProcessInput();
        renderer.Update();

        glfwSwapInterval(WindowData::vsync ? 1 : 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);   
    glfwTerminate();
    return 0;
}
