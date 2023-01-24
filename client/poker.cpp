#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <thread>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "include/glad/glad.h"
#include <GLFW/glfw3.h>
#include "include/classes/shader_class.hpp"
#include "include/classes/model_class.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <vector>
#include <string>

#include "../net/client/include/client.hpp"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

vector<Model> objects;
vector<glm::vec3> object_axis;

int width = 800;
int height = 600;

bool connected = false;


int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "Poker Game", NULL, NULL);

    if (window == NULL) {
        std::cout << "Window could not be created" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char *glsl_version = "#version 460";
    ImGui_ImplOpenGL3_Init(glsl_version);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Glad could not be loaded" << std::endl;
        return -1;
    }


    stbi_set_flip_vertically_on_load(false);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    Shader ourShader("../include/shader.vs", "../include/shader.fs");

    Shader stencilShader("../include/stencilSh.vs", "../include/stencilSh.fs");

    float returnVal = 0.0f;
    float returnVal2 = 0.0f;

    asio::io_context ioc;
    std::thread ioContextThread;
    net::client::Client client(ioc);

    while (!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClearColor(0.1f, 0.0f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                GL_STENCIL_BUFFER_BIT);

        ImGui::Begin("Hello Imgui");
        ImGui::Text("hello, this is ImGui");

        ImGui::SliderFloat("float", &returnVal, -1.0f, 1.0f);

        if (ImGui::Button("Add stool")) {
            Model ourModel("../include/objects/Bar_Stool.obj");
            objects.push_back(ourModel);
            object_axis.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        }
        if (ImGui::Button("Add card")) {
            Model ourModel("../include/objects/Diamonds.obj");
            objects.push_back(ourModel);
            object_axis.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        }
        if (ImGui::Button("Add bad chair")) {
            Model ourModel("../include/objects/Bad_Chair.obj");
            objects.push_back(ourModel);
            object_axis.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        }
        if (ImGui::Button("Add table")) {
            Model ourModel("../include/objects/Table.obj");
            objects.push_back(ourModel);
            object_axis.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        }
        ImGui::End();

        ImGui::Begin("Client");
        if (!client.isConnected())
        {
            static char serverAddress[17] = "";
            unsigned short serverPort = 1233;
            ImGui::InputTextWithHint("IP address", "Eg. 127.0.0.1",
                                      serverAddress,
                                      IM_ARRAYSIZE(serverAddress),
                                      ImGuiInputTextFlags_CharsNoBlank);
            ImGui::InputInt("Port", (int *)&serverPort, 0, 0, 0);

            if (ImGui::Button("Connect")) /* If the button was pressed: */
            {
                boost::system::error_code errorCode;
                asio::ip::address::from_string(serverAddress, errorCode);
                /* Don't accept invalid port numbers */
                if (serverPort <= 0 || serverPort > 65535)
                    ImGui::Text("Port is not valid!"); // TODO: add color
                else if (strlen(serverAddress) <= 0 || errorCode)
                    ImGui::Text("Address is not valid!"); // TODO: add color
                else
                {
                    /* Try to connect to the server */
                    client.connect(serverAddress, serverPort);
                    if (!client.isConnected())
                    { // TODO: add color to text
                        ImGui::Text("Failed to connect to server");
                    }

                    /* Run the client in a separate thread */
                    ioContextThread = std::thread([&ioc] { ioc.run(); });
                }
            }
        }
        else
        {
            ImGui::Text("Connected to server");

            while (client.messages())
            {
                net::common::Message message = client.lastMessage();
                if (message.getRecvMt() == net::common::messageType::Pong)
                    ImGui::Text("Received pong from server!");
            }

            if (ImGui::Button("Ping"))
            {
                client.sendPing();
            }
        }
        ImGui::End();

        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        ourShader.use();

        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        projection = glm::perspective(glm::radians(45.0f),
                                      (float) 800 / (float) 600, 0.1f, 100.0f);
        view = glm::translate(view, glm::vec3(0.0f, -1.0f, -10.0f));
        view = glm::rotate(view, glm::radians(30.0f),
                           glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);


        for (int i = 0; i < objects.size(); i++) {
            std::string nameX = "x ";
            std::string nameY = "y ";
            std::string nameZ = "z ";
            nameX += std::to_string(i);
            nameY += std::to_string(i);
            nameZ += std::to_string(i);
            ImGui::Begin("Edit");
//        if(ImGui::Button(nameX.c_str()))
            //           object_axis[i].x += 0.1f;

            ImGui::SliderFloat(nameX.c_str(), &object_axis[i].x, -1.0f, 1.0f);
            ImGui::SliderFloat(nameY.c_str(), &object_axis[i].y, -1.0f, 1.0f);
            ImGui::SliderFloat(nameZ.c_str(), &object_axis[i].z, -1.0f, 1.0f);
            ImGui::End();

            glm::mat4 model = glm::mat4(1.0f);

            model = glm::translate(model, object_axis[i]);
            model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

            ourShader.setMat4("model", model);

            objects[i].Draw(ourShader);
        }

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        stencilShader.use();
        glUniform1f(glGetUniformLocation(stencilShader.ID, "stencil"), 1.08f);

        for (int i = 0; i < objects.size(); i++) {
            glm::mat4 model = glm::mat4(1.0f);

            model = glm::translate(model, object_axis[i]);
            model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

            stencilShader.setMat4("model", model);

            objects[i].Draw(stencilShader);
        }

        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glEnable(GL_DEPTH_TEST);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    /* Disconnect from the server */
    client.close();

    /* Wait for the client thread to exit */
    if (ioContextThread.joinable())
        ioContextThread.join();

    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

