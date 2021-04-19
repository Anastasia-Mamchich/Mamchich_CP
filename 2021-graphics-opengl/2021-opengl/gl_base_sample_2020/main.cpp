#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

//internal includes
#include "common.h"
#include "ShaderProgram.h"
#include "Camera.h"

//External dependencies
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <random>

float theta,theta2;

static const GLsizei WIDTH = 1080, HEIGHT = 720; //размеры окна
static int filling = 0;
static bool keys[1024]; //массив состояний кнопок - нажата/не нажата
static GLfloat lastX = 400, lastY = 300; //исходное положение мыши
static bool firstMouse = true;
static bool g_captureMouse = true;  // Мышка захвачена нашим приложением или нет?
static bool g_capturedMouseJustNow = false;
static int g_shaderProgram = 0;


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

Camera camera(float3(0.0f, 0.0f, 5.0f));

//функция для обработки нажатий на кнопки клавиатуры
void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    switch (key)
    {
    case GLFW_KEY_ESCAPE: //на Esc выходим из программы
        if (action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
        break;
    case GLFW_KEY_SPACE: //на пробел переключение в каркасный режим и обратно
        if (action == GLFW_PRESS)
        {
            if (filling == 0)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                filling = 1;
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                filling = 0;
            }
        }
        break;
    case GLFW_KEY_1:
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        break;
    case GLFW_KEY_2:
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        break;
    default:
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

//функция для обработки клавиш мыши
void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        g_captureMouse = !g_captureMouse;


    if (g_captureMouse)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        g_capturedMouseJustNow = true;
    }
    else
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

//функция для обработки перемещения мыши
void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = float(xpos);
        lastY = float(ypos);
        firstMouse = false;
    }

    GLfloat xoffset = float(xpos) - lastX;
    GLfloat yoffset = lastY - float(ypos);

    lastX = float(xpos);
    lastY = float(ypos);

    if (g_captureMouse)
        camera.ProcessMouseMove(xoffset, yoffset);
}


void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(GLfloat(yoffset));
}

void doCameraMovement(Camera& camera, GLfloat deltaTime)
{
    if (keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

float3 CreateNormal(std::vector<float> vertex, int i, int k, int s)
{
    //расчет нормали к боковой
    float3 a, b;
    //вектор а
    a.x = vertex[i * 4] - vertex[s * 4];
    a.y = vertex[i * 4 + 1] - vertex[s * 4 + 1];
    a.z = vertex[i * 4 + 2] - vertex[s * 4 + 2];

    //вектор б 
    b.x = vertex[k * 4] - vertex[s * 4];
    b.x = vertex[k * 4 + 1] - vertex[s * 4 + 1];
    b.x = vertex[k * 4 + 2] - vertex[s * 4 + 2];

    //нормаль как векторное произведение
    return cross(a, b);
}
GLsizei CreatePlane(GLuint& vao)
{
    std::vector <float> vertex = { -50.0f,0.0f,-50.0f,1.0f, 50.0f,0.0f,-50.0f,1.0f,
    -50.0f,0.0f,50.0f,1.0f, 50.0f,0.0f,50.0f,1.0f };
    std::vector <int> faces = { 0,1,2, 1,2,3 };
    std::vector <float> normal = { 0.0f,1.0f,0.0f, 0.0f,1.0f,0.0f };


    GLuint vboVertices, vboIndices, vboNormals;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vboIndices);

    glBindVertexArray(vao);

    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(GLfloat), vertex.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof(GLfloat), normal.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(int), faces.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    return faces.size();
}
GLsizei CreateCube(float lenght, float width, float height, GLuint& vao)
{
    std::vector <float> vertex = { 0.0f,0.0f,0.0f,1.0f,  0.0f,lenght,0.0f, 1.0f,  width,lenght,0.0f,1.0f,   width,0.0f,0.0f,1.0f,
                                   width,0.0f,height,1.0f,  width,lenght,height,1.0f,  0.0f,lenght,height,1.0f,  0.0f,0.0f,height,1.0f };
    std::vector <int> faces = { 0,1,2, 0,2,3, 2,3,4, 2,4,5, 4,5,6, 4,6,7, 7,0,4, 0,4,3, 6,7,1, 0,1,7, 1,2,6, 5,2,6 };
    std::vector <float> normal = { 0.0f,0.0f,-1.0f, 0.0f,0.0f,-1.0f, 1.0f,0.0f,0.0f, 1.0f,0.0f,0.0f,
                                  0.0f,0.0f,1.0f, 0.0f,0.0f,1.0f, 0.0f,-1.0f,0.0f, 0.0f,-1.0f,0.0f,
                                  -1.0f,0.0f,0.0f, -1.0f,0.0f,0.0f, 1.0f,0.0f,0.0f, 1.0f,0.0f,0.0f };


    GLuint vboVertices, vboIndices, vboNormals;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vboIndices);

    glBindVertexArray(vao);

    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(GLfloat), vertex.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof(GLfloat), normal.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(int), faces.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    return faces.size();
}

GLsizei CreateSphere(float radius, int numberSlices, GLuint& vao)
{
    int i, j;

    int numberParallels = numberSlices;
    int numberVertices = (numberParallels + 1) * (numberSlices + 1);
    int numberIndices = numberParallels * numberSlices * 3;

    float angleStep = (2.0f * M_PI) / ((float)numberSlices);

    std::vector<float> pos(numberVertices * 4, 0.0f);//координаты
    std::vector<float> norm(numberVertices * 4, 0.0f);
    std::vector<float> texcoords(numberVertices * 2, 0.0f);//для текстур

    std::vector<int> indices(numberIndices, -1);

    for (i = 0; i < numberParallels + 1; i++)
    {
        for (j = 0; j < numberSlices + 1; j++)
        {
            int vertexIndex = (i * (numberSlices + 1) + j) * 4;
            int normalIndex = (i * (numberSlices + 1) + j) * 4;
            int texCoordsIndex = (i * (numberSlices + 1) + j) * 2;

            pos.at(vertexIndex + 0) = radius * sinf(angleStep * (float)i) * sinf(angleStep * (float)j);
            pos.at(vertexIndex + 1) = radius * cosf(angleStep * (float)i);
            pos.at(vertexIndex + 2) = radius * sinf(angleStep * (float)i) * cosf(angleStep * (float)j);
            pos.at(vertexIndex + 3) = 1.0f;

            norm.at(normalIndex + 0) = pos.at(vertexIndex + 0) / radius;
            norm.at(normalIndex + 1) = pos.at(vertexIndex + 1) / radius;
            norm.at(normalIndex + 2) = pos.at(vertexIndex + 2) / radius;
            norm.at(normalIndex + 3) = 1.0f;

            texcoords.at(texCoordsIndex + 0) = (float)j / (float)numberSlices;
            texcoords.at(texCoordsIndex + 1) = (1.0f - (float)i) / (float)(numberParallels - 1);
        }
    }

    int* indexBuf = &indices[0];

    for (i = 0; i < numberParallels; i++)
    {
        for (j = 0; j < numberSlices; j++)
        {
            *indexBuf++ = i * (numberSlices + 1) + j;
            *indexBuf++ = (i + 1) * (numberSlices + 1) + j;
            *indexBuf++ = (i + 1) * (numberSlices + 1) + (j + 1);

            *indexBuf++ = i * (numberSlices + 1) + j;
            *indexBuf++ = (i + 1) * (numberSlices + 1) + (j + 1);
            *indexBuf++ = i * (numberSlices + 1) + (j + 1);

            int diff = int(indexBuf - &indices[0]);
            if (diff >= numberIndices)
                break;
        }
        int diff = int(indexBuf - &indices[0]);
        if (diff >= numberIndices)
            break;
    }

    GLuint vboVertices, vboIndices, vboNormals, vboTexCoords;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vboIndices);

    glBindVertexArray(vao);

    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(GLfloat), &pos[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, norm.size() * sizeof(GLfloat), &norm[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &vboTexCoords);
    glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords);
    glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(GLfloat), &texcoords[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    return indices.size();
}

GLsizei CreateCylinder(float radius, float height, int numberSlices, GLuint& vao)
{
    std::vector <float> vertex;
    std::vector <int> faces;
    std::vector <float> normal;

    float xpos = 0.0f;
    float ypos = 0.0f;

    float angle = M_PI * 2.f / float(numberSlices);

    //центр дна
    vertex.push_back(xpos);
    vertex.push_back(ypos);
    vertex.push_back(0.0f);
    vertex.push_back(1.0f); //w

    //центр крышки
    vertex.push_back(xpos);
    vertex.push_back(ypos);
    vertex.push_back(height);
    vertex.push_back(1.0f); //w

    //расчёт всех точек
    for (int i = 1; i <= numberSlices; i++)
    {
        float newX = radius * sinf(angle * i);
        float newY = -radius * cosf(angle * i);

        //для дна
        vertex.push_back(newX);
        vertex.push_back(newY);
        vertex.push_back(0.0f);
        vertex.push_back(1.0f); //w

        //для крышки
        vertex.push_back(newX);
        vertex.push_back(newY);
        vertex.push_back(height);
        vertex.push_back(1.0f); //w
    }
    //расчёт поверхностей крышки и дна
    for (int i = 0; i <= 2 * numberSlices - 1; i++)
    {
        //индексы с четными номерами принадлежат дну
        //с нечетными - крышке
        if (i % 2 == 0)//дно
        {
            faces.push_back(0);//центр дна

            //нормали смотрят вниз
            normal.push_back(0.0f);
            normal.push_back(0.0f);
            normal.push_back(-1.0f);
        }
        else//крышка
        {
            faces.push_back(1);//центр крышки

            //нормали смотрят вверх
            normal.push_back(0.0f);
            normal.push_back(0.0f);
            normal.push_back(1.0f);
        }

        faces.push_back(i + 2);//следующая вершина после центра

        //завершающая вершина
        if (((i + 4) % (2 * numberSlices) == 0.0f) || ((i + 4) % (2 * numberSlices) == 1.0f))
        {
            faces.push_back(i + 4);
        }
        else
        {
            faces.push_back((i + 4) % (2 * numberSlices));
        }
    }
    //боковые поверхности
    for (int i = 2; i <= 2 * numberSlices; i = i + 2)
    {
        if ((i != 2) && (i != 2 * numberSlices))
        {
            //первый треугольник
            faces.push_back(i);
            faces.push_back(i + 1);
            faces.push_back(i + 2);

            normal.push_back(CreateNormal(vertex, (i + 1), (i + 2), i).x);
            normal.push_back(CreateNormal(vertex, (i + 1), (i + 2), i).y);
            normal.push_back(CreateNormal(vertex, (i + 1), (i + 2), i).z);

            //каждая четная вершина учавствует в двух треугольниках
            //(кроме второй)
            //второй треугольник
            faces.push_back(i);
            faces.push_back(i - 1);
            faces.push_back(i + 1);

            normal.push_back(CreateNormal(vertex, (i + 1), (i - 1), i).x);
            normal.push_back(CreateNormal(vertex, (i + 1), (i - 1), i).y);
            normal.push_back(CreateNormal(vertex, (i + 1), (i - 1), i).z);
        }
        else if (i == 2)
        {
            //первый треугольник
            faces.push_back(i);
            faces.push_back(i + 1);
            faces.push_back(i + 2);

            normal.push_back(CreateNormal(vertex, (i + 1), (i + 2), i).x);
            normal.push_back(CreateNormal(vertex, (i + 1), (i + 2), i).y);
            normal.push_back(CreateNormal(vertex, (i + 1), (i + 2), i).z);
        }
        else if (i == 2 * numberSlices)
        {
            //первый треугольник
            faces.push_back(i);
            faces.push_back(i + 1);
            faces.push_back(3);

            normal.push_back(CreateNormal(vertex, (i + 1), 3, i).x);
            normal.push_back(CreateNormal(vertex, (i + 1), 3, i).y);
            normal.push_back(CreateNormal(vertex, (i + 1), 3, i).z);

            //второй треугольник
            faces.push_back(i);
            faces.push_back(2);
            faces.push_back(3);

            normal.push_back(CreateNormal(vertex, 2, 3, i).x);
            normal.push_back(CreateNormal(vertex, 2, 3, i).y);
            normal.push_back(CreateNormal(vertex, 2, 3, i).z);
        }

    }

    GLuint vboVertices, vboIndices, vboNormals;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vboIndices);

    glBindVertexArray(vao);

    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(GLfloat), vertex.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof(GLfloat), normal.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(int), faces.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    return faces.size();
}

GLsizei CreateCone(float radius, float height, int numberSlices, GLuint& vao)
{
    std::vector <float> vertex;//вершины
    std::vector <int> faces;//треугольники по индексам
    std::vector <float> normal;//нормали

    float xpos = 0.0f;
    float ypos = 0.0f;

    float angle = M_PI * 2.f / float(numberSlices); //угол смещения

    //центр дна
    vertex.push_back(xpos); vertex.push_back(0.0f);
    vertex.push_back(ypos);
    vertex.push_back(1.0f); //w

    //расчёт всех точек дна
    for (int i = 1; i <= numberSlices; i++)
    {
        float newX = radius * sinf(angle * i);
        float newY = -radius * cosf(angle * i);

        //для дна
        vertex.push_back(newX); vertex.push_back(0.0f);
        vertex.push_back(newY);
        
        vertex.push_back(1.0f); //w
    }

    //координата вершины конуса
    vertex.push_back(xpos); vertex.push_back(height);
    vertex.push_back(ypos);
    
    vertex.push_back(1.0f); //w

    //ИТОГО: вершины: центр основания, точка основания 1, точка основания 2,
    // и т.д., точка-вершина (четыре координаты)

    //расчёт поверхности дна + нормали
    for (int i = 1; i <= numberSlices; i++)
    {
        faces.push_back(0); //центр основания
        faces.push_back(i); //текущая точка

        if (i != numberSlices) //если не крайняя точка основания
        {
            faces.push_back(i + 1);//то соединяем со следующей по индексу
        }
        else
        {
            faces.push_back(1);//замыкаем с 1ой
        }

        //нормали у дна смотрят вниз
        normal.push_back(0.0f);
        normal.push_back(0.0f);
        normal.push_back(-1.0f);
    }
    //боковые поверхности + нормали
    for (int i = 1; i <= numberSlices; i++)
    {
        int k = 0;//нужно для нормалей

        faces.push_back(i);//текущая

        if (i != numberSlices) //если не крайняя точка основания
        {
            faces.push_back(i + 1);//то соединяем со следующей по индексу
            k = i + 1;
        }
        else
        {
            faces.push_back(1);//замыкаем с 1ой
            k = 1;
        }

        faces.push_back(numberSlices + 1);//вершина

        //расчет нормали к боковой
        float3 a, b, normalVec;
        //вектор а = координаты текущей - координаты вершины
        a.x = vertex[i * 4 - 3] - vertex[vertex.size() - 1 - 3];
        a.y = vertex[i * 4 - 2] - vertex[vertex.size() - 1 - 2];;
        a.z = vertex[i * 4 - 1] - vertex[vertex.size() - 1 - 1];;

        //вектор б = координаты седующей текущей (или 1 при последней итерации)
        // - координаты вершины)
        b.x = vertex[k * 4 - 3] - vertex[vertex.size() - 1 - 3];
        b.x = vertex[k * 4 - 2] - vertex[vertex.size() - 1 - 2];
        b.x = vertex[k * 4 - 1] - vertex[vertex.size() - 1 - 1];

        //нормаль как векторное произведение
        normalVec = cross(a, b);

        //запись нормаль в вектор
        normal.push_back(normalVec.x);
        normal.push_back(normalVec.y);
        normal.push_back(normalVec.z);
    }


    GLuint vboVertices, vboIndices, vboNormals;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vboIndices);

    glBindVertexArray(vao);

    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(GLfloat), vertex.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof(GLfloat), normal.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(int), faces.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    return faces.size();
}


int initGL()
{
    int res = 0;

    //грузим функции opengl через glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    //выводим в консоль некоторую информацию о драйвере и контексте opengl
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    std::cout << "Controls: " << std::endl;
    std::cout << "press right mouse button to capture/release mouse cursor  " << std::endl;
    std::cout << "press spacebar to alternate between shaded wireframe and fill display modes" << std::endl;
    std::cout << "press ESC to exit" << std::endl;

    return 0;
}

int main(int argc, char** argv)
{
    if (!glfwInit())
        return -1;


    //запрашиваем контекст opengl версии 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL basic sample", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    //регистрируем коллбеки для обработки сообщений от пользователя - клавиатура, мышь..
    glfwSetKeyCallback(window, OnKeyboardPressed);
    glfwSetCursorPosCallback(window, OnMouseMove);
    glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
    glfwSetScrollCallback(window, OnMouseScroll);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (initGL() != 0)
        return -1;

    //Reset any OpenGL errors which could be present for some reason
    GLenum gl_error = glGetError();
    while (gl_error != GL_NO_ERROR)
        gl_error = glGetError();

    //создание шейдерной программы из двух файлов с исходниками шейдеров
    //используется класс-обертка ShaderProgram
    std::unordered_map<GLenum, std::string> shaders;
    shaders[GL_VERTEX_SHADER] = "../shaders/vertex.glsl";
    shaders[GL_FRAGMENT_SHADER] = "../shaders/lambert.frag";
    ShaderProgram lambert(shaders); GL_CHECK_ERRORS;

    GLuint vaoDragonFlyBody, vaoDragonFlyTail, vaoDragonFlyEye, vaoDragonFlyWing, vaoDragonFlyWingSmall;
    GLuint vaoPlane, vaoTree;


    GLsizei DragonFlyBody = CreateSphere(0.7f,  12, vaoDragonFlyBody);
    GLsizei DragonFlyTail = CreateCylinder(0.15f, 3.0f, 12, vaoDragonFlyTail);
    GLsizei DragonFlyEye = CreateSphere(0.1f, 12, vaoDragonFlyEye);
    GLsizei DragonFlyWing = CreateCube(0.15f, 3.5f, 0.5f, vaoDragonFlyWing);
    GLsizei DragonFlyWingSmall = CreateCube(0.05f, 2.0f, 0.5f, vaoDragonFlyWingSmall);
    GLsizei Plane = CreatePlane(vaoPlane);
    GLsizei Tree = CreateCone(3.0f,7.0f,12,vaoTree);

    glViewport(0, 0, WIDTH, HEIGHT);  GL_CHECK_ERRORS;
    glEnable(GL_DEPTH_TEST);  GL_CHECK_ERRORS;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //цикл обработки сообщений и отрисовки сцены каждый кадр
    while (!glfwWindowShouldClose(window))
    {
        //считаем сколько времени прошло за кадр
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        theta += 1000 * deltaTime;
        theta2 += 1.5*deltaTime;

        glfwPollEvents();
        doCameraMovement(camera, deltaTime);

        //очищаем экран каждый кадр
        glClearColor(0.9f, 0.95f, 0.97f, 1.0f); GL_CHECK_ERRORS;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;

        lambert.StartUseShader(); GL_CHECK_ERRORS;

        float4x4 view = camera.GetViewMatrix();
        float4x4 projection = projectionMatrixTransposed(camera.zoom, float(WIDTH) / float(HEIGHT), 0.1f, 1000.0f);
        float4x4 model;

        lambert.SetUniform("view", view);       GL_CHECK_ERRORS;
        lambert.SetUniform("projection", projection); GL_CHECK_ERRORS;

        
        //----------------------------------------
        //Стрекоза

        glBindVertexArray(vaoDragonFlyBody);
        {
            model = transpose(translate4x4(float3(0.0f, 0.0f, -theta2)));
            lambert.SetUniform("model", model); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLE_STRIP, DragonFlyBody, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
        }
        glBindVertexArray(0); GL_CHECK_ERRORS;

        glBindVertexArray(vaoDragonFlyTail);
        {
            model = transpose(translate4x4(float3(0.0f, 0.0f, -theta2)));
            lambert.SetUniform("model", model); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLE_STRIP, DragonFlyTail, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
        }
        glBindVertexArray(0); GL_CHECK_ERRORS;

        glBindVertexArray(vaoDragonFlyEye);
        {
            model = transpose(translate4x4(float3(0.35f, 0.0f, -0.6f- theta2)));
            lambert.SetUniform("model", model); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLE_STRIP, DragonFlyEye, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

            model = transpose(translate4x4(float3(-0.35f, 0.0f, -0.6f- theta2)));
            lambert.SetUniform("model", model); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLE_STRIP, DragonFlyEye, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
        }
        glBindVertexArray(0); GL_CHECK_ERRORS;

        glBindVertexArray(vaoDragonFlyWing); 
        {
            model = transpose(mul(translate4x4(float3(0.0f, 0.0f,  -theta2)),mul(rotate_Z_4x4((90) * LiteMath::DEG_TO_RAD), rotate_Z_4x4((theta)*LiteMath::DEG_TO_RAD))));
            //model = transpose(mul(rotate_Z_4x4((90)*LiteMath::DEG_TO_RAD), rotate_Z_4x4((theta)*LiteMath::DEG_TO_RAD)));
            lambert.SetUniform("model", model); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLE_STRIP, DragonFlyWing, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

            model = transpose(mul(translate4x4(float3(0.0f, 0.0f, -theta2)), mul(rotate_Z_4x4((90) * LiteMath::DEG_TO_RAD), rotate_Z_4x4((-theta)*LiteMath::DEG_TO_RAD))));
            lambert.SetUniform("model", model); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLE_STRIP, DragonFlyWing, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
        }
        glBindVertexArray(0); GL_CHECK_ERRORS;
        /*
        glBindVertexArray(vaoDragonFlyWingSmall); 
        {
            model = transpose(mul(rotate_Z_4x4((90) * LiteMath::DEG_TO_RAD), rotate_Z_4x4((90+theta2)*LiteMath::DEG_TO_RAD)));
            lambert.SetUniform("model", model); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLE_STRIP, DragonFlyWingSmall, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

            model = transpose(mul(rotate_Z_4x4((90) * LiteMath::DEG_TO_RAD), rotate_Z_4x4((90-theta2) * LiteMath::DEG_TO_RAD)));
            lambert.SetUniform("model", model); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLE_STRIP, DragonFlyWingSmall, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
        }
        glBindVertexArray(0); GL_CHECK_ERRORS;
        */
        

        //----------------------------------------
        // Поляна
        glBindVertexArray(vaoPlane);
        {
            model = transpose(translate4x4(float3(0.0f, -10.0f, 0.0f)));
            lambert.SetUniform("model", model); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLE_STRIP, Plane, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

        }
        glBindVertexArray(0); GL_CHECK_ERRORS;

        glBindVertexArray(vaoTree);
        {
            model = transpose(translate4x4(float3(6.0f, -10.0f, 3.0f)));
            lambert.SetUniform("model", model); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLE_STRIP, Tree, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

            model = transpose(translate4x4(float3(10.0f, -10.0f, 15.0f)));
            lambert.SetUniform("model", model); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLE_STRIP, Tree, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

            model = transpose(translate4x4(float3(14.0f, -10.0f, -32.0f)));
            lambert.SetUniform("model", model); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLE_STRIP, Tree, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

            model = transpose(translate4x4(float3(39.0f, -10.0f, 25.0f)));
            lambert.SetUniform("model", model); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLE_STRIP, Tree, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

            model = transpose(translate4x4(float3(-16.0f, -10.0f, 36.0f)));
            lambert.SetUniform("model", model); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLE_STRIP, Tree, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

            model = transpose(translate4x4(float3(26.0f, -10.0f, -24.0f)));
            lambert.SetUniform("model", model); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLE_STRIP, Tree, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

            model = transpose(translate4x4(float3(-0.0f, -10.0f, 13.0f)));
            lambert.SetUniform("model", model); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLE_STRIP, Tree, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

            model = transpose(translate4x4(float3(11.0f, -10.0f, -23.0f)));
            lambert.SetUniform("model", model); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLE_STRIP, Tree, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

            model = transpose(translate4x4(float3(-19.0f, -10.0f, -25.0f)));
            lambert.SetUniform("model", model); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLE_STRIP, Tree, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

            model = transpose(translate4x4(float3(16.0f, -10.0f, -11.0f)));
            lambert.SetUniform("model", model); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLE_STRIP, Tree, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

            model = transpose(translate4x4(float3(-26.0f, -10.0f, 24.0f)));
            lambert.SetUniform("model", model); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLE_STRIP, Tree, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

            model = transpose(translate4x4(float3(-0.0f, -10.0f, -13.0f)));
            lambert.SetUniform("model", model); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLE_STRIP, Tree, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

            model = transpose(translate4x4(float3(-11.0f, -10.0f, 23.0f)));
            lambert.SetUniform("model", model); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLE_STRIP, Tree, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
            model = transpose(translate4x4(float3(-13.0f, -10.0f, -43.0f)));
            lambert.SetUniform("model", model); GL_CHECK_ERRORS;
            glDrawElements(GL_TRIANGLE_STRIP, Tree, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;

        }
        glBindVertexArray(0); GL_CHECK_ERRORS;


        //----------------------------------------

        lambert.StopUseShader(); GL_CHECK_ERRORS;
        glfwSwapBuffers(window);
    }

    //glDeleteVertexArrays(1, &vaoSphere);

    glfwTerminate();
    return 0;
}
