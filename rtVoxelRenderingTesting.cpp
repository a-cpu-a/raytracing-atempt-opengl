#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include "ven/stb_image.h"

#include <GLFW/glfw3.h>


#include "Texture.h"
#include "Shader.h"
#include "FrameBuffer.h"


#define PI 6.28318531f

float mod(float in, float dev)
{
    return in - (floor(in / dev) * dev);
}


//debug message
#ifdef _DEBUG
void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length,
    const GLchar* msg, const void* data)
{
    std::string _source;
    std::string _type;
    std::string _severity;

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        _source = "API";
        break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = "WINDOW SYSTEM";
        break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = "SHADER COMPILER";
        break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = "THIRD PARTY";
        break;

    case GL_DEBUG_SOURCE_APPLICATION:
        _source = "APPLICATION";
        break;

    case GL_DEBUG_SOURCE_OTHER:
        _source = "UNKNOWN";
        break;

    default:
        _source = "UNKNOWN";
        break;
    }

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        _type = "ERROR";
        break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type = "DEPRECATED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type = "UDEFINED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_PORTABILITY:
        _type = "PORTABILITY";
        break;

    case GL_DEBUG_TYPE_PERFORMANCE:
        _type = "PERFORMANCE";
        break;

    case GL_DEBUG_TYPE_OTHER:
        _type = "OTHER";
        break;

    case GL_DEBUG_TYPE_MARKER:
        _type = "MARKER";
        break;

    default:
        _type = "UNKNOWN";
        break;
    }

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        _severity = "HIGH";
        break;

    case GL_DEBUG_SEVERITY_MEDIUM:
        _severity = "MEDIUM";
        break;

    case GL_DEBUG_SEVERITY_LOW:
        _severity = "LOW";
        break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
        _severity = "NOTIFICATION";
        break;

    default:
        _severity = "UNKNOWN";
        break;
    }

    //printf("%d: %s of %s severity, raised from %s: %s\n",
    //    id, _type, _severity, _source, msg);
    std::cout << _type << ": " << msg << " of " << _severity << " severity, rasied from " << _source << "\n";
}
#endif // _DEBUG
//


FrameBuffer pastFrameBuffer;
FrameBuffer nowFrameBuffer;

size_t scrW;
size_t scrH;

void resizeCallBack(GLFWwindow* window, int x, int y)
{
    scrW = x;
    scrH = y;

    pastFrameBuffer.resize(scrW, scrH);
    nowFrameBuffer.resize(scrW, scrH);

    glViewport(0, 0, x, y);

}
vec3 pos = vec3(0.0f);

vec3 camRight = vec3(0.0f);
vec3 camUp = vec3(0.0f);
vec3 camFront = vec3(0.0f);

Texture world;
Texture preCompute;

Shader mainShader;
Shader postShader;
Shader displayShader;

void reloadAll()
{
    world.reload();
    preCompute.reload();
    mainShader.reload();
    postShader.reload();
}

void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int modifierBitField)
{
    if (key == GLFW_KEY_F5)
    {
        reloadAll();
    }
    if (key == GLFW_KEY_W)
    {
        pos = pos + (camFront*0.05);
    }
    if (key == GLFW_KEY_D)
    {
        pos = pos + (camRight * 0.05);
    }
    if (key == GLFW_KEY_SPACE)
    {
        pos.y += 0.05f;
    }
    if (key == GLFW_KEY_S)
    {
        pos = pos + (camFront * -0.05);
    }
    if (key == GLFW_KEY_A)
    {
        pos = pos + (camRight * -0.05);
    }
    if (key == GLFW_KEY_LEFT_SHIFT)
    {
        pos.y -= 0.05f;
    }
}

int main()
{
    float vertexData[] = {
        -1.0f,-1.0f,
        -1.0f,1.0f,
        1.0f,1.0f,

        1.0f,1.0f,
        1.0f,-1.0f,
        -1.0f,-1.0f
    };

    stbi_set_flip_vertically_on_load(1);
    std::cout << "Hello World!\n";

    //init

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);


    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(1000, 1000, "r-tree-x", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "ERROR: GLEW SETUP";
        glfwTerminate();
        return -1;

    }

    //debug msg
#ifdef _DEBUG

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(GLDebugMessageCallback, NULL);

#endif // _DEBUG

    glfwSetKeyCallback(window, keyCallBack);
    glfwSetWindowSizeCallback(window, resizeCallBack);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //fb
    size_t texId = 0;
    const size_t fbNowId = texId++;
    const size_t fbPastId = texId++;

    GLuint fbIds[2];

    glGenFramebuffers(2, fbIds);

    nowFrameBuffer = FrameBuffer(1000, 1000, fbNowId, fbIds[0]);
    pastFrameBuffer = FrameBuffer(1000, 1000, fbPastId, fbIds[1]);
    //

    GLuint vertexArray;
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    //vb
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    //

    glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(float) * 2, 0);
    glEnableVertexArrayAttrib(vertexArray, 0);

    glBindVertexArray(0);

    //begin


    world = Texture("assets/world.png", texId++);
    preCompute = Texture("assets/pre_compute.png", texId++);

    {//main

        mainShader = Shader("assets/shader/main.vsh", "assets/shader/main.fsh");
    }

        GLint mainShaderPositionLoc = mainShader.getUniformLocation("cameraPosition");
        GLint mainShaderUpLoc = mainShader.getUniformLocation("upVec");
        GLint mainShaderRightLoc = mainShader.getUniformLocation("rightVec");
        GLint mainShaderFrontLoc = mainShader.getUniformLocation("frontVec");
        GLint mainShaderTickLoc = mainShader.getUniformLocation("tick");
    {//post
        postShader = Shader("assets/shader/post0.vsh", "assets/shader/post0.fsh");

        postShader.bind();
        postShader.setUniformINT(postShader.getUniformLocation("screenTex"), fbNowId);

        displayShader = Shader("assets/shader/display.vsh", "assets/shader/display.fsh");
        displayShader.bind();
        displayShader.setUniformINT(displayShader.getUniformLocation("pastTex"), fbPastId);
        displayShader.setUniformINT(displayShader.getUniformLocation("nowTex"), fbNowId);
    }
    




    float xRot = 0.0f;
    float yRot = 0.0f;

    double prevCurX = 0.0;
    double prevCurY = 0.0;
    glfwGetCursorPos(window, &prevCurX, &prevCurY);

    size_t tick = 0;

    while (!glfwWindowShouldClose(window))
    {



        double curX = 0.0;
        double curY = 0.0;

        glfwGetCursorPos(window, &curX, &curY);

        xRot -= (float)(prevCurX - curX) * 0.001f;
        yRot -= (float)(prevCurY - curY) * 0.001f;
        xRot = mod(xRot, 1.0f);
        yRot = mod(yRot, 1.0f);

        prevCurX = curX;
        prevCurY = curY;

        if (mainShader.failed || postShader.failed)//uh oh failed!
        {
            /* Swap front and back buffers */
            glfwSwapBuffers(window);
            /* Poll for and process events */
            glfwPollEvents();

            continue;
        }

        {
            float xPI = xRot * PI;
            float yPI = yRot * PI;

            camRight = vec3(cos(xPI), 0, -sin(xPI));
            camUp = vec3(sin(yPI) * sin(xPI), cos(yPI), sin(yPI) * cos(xPI));
            camFront = vec3(cos(yPI) * sin(xPI), -sin(yPI), cos(yPI) * cos(xPI));

            mainShader.bind();

            mainShader.setUniformVEC3(mainShaderUpLoc, camUp);
            mainShader.setUniformVEC3(mainShaderRightLoc, camRight);
            mainShader.setUniformVEC3(mainShaderFrontLoc, camFront);

            mainShader.setUniformUINT(mainShaderTickLoc, (uint32_t)tick);
        }

        mainShader.setUniformVEC3(mainShaderPositionLoc, pos);

        nowFrameBuffer.bind();

        //main
        glBindVertexArray(vertexArray);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        nowFrameBuffer.unBind();

        {
            //display
            displayShader.bind();

            //post
            glBindVertexArray(vertexArray);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }

        {
            //post
            postShader.bind();

            pastFrameBuffer.bind();

            glBindVertexArray(vertexArray);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);

            pastFrameBuffer.unBind();
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        tick++;
    }
    glfwTerminate();
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
