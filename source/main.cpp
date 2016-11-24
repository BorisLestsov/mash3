#include <iostream>
#include <vector>
#include <cstring>
#include <functional>

#include "SOIL.h"
#include "Utility.h"

using namespace std;

const uint GRASS_INSTANCES = 8000;  // 10000 Количество травинок
const uint GROUND_X = 10.0f; // 5.0
const uint GROUND_Y = 10.0f; //5.0
const GLfloat GRASS_HEIGHT = 7.0f;  //5.0
const GLfloat GRASS_WIDTH = 1.0f;   //0.5

const float SPEED = 0.08;

const uint LOD = 7;    //10

float g = 0.08f; //9.8f
float k = 5.0f;
float dt = 1.0f/60;
const VM::vec4 wind(1.2f, 0.0f, 1.2f, 0.0f);
const VM::vec4 init_variance = wind/4;

GLuint gr_texture;
GLuint grass_texture;


GL::Camera camera;               // Мы предоставляем Вам реализацию камеры. В OpenGL камера - это просто 2 матрицы. Модельно-видовая матрица и матрица проекции. // ###
                                 // Задача этого класса только в том чтобы обработать ввод с клавиатуры и правильно сформировать эти матрицы.
                                 // Вы можете просто пользоваться этим классом для расчёта указанных матриц.


GLuint grassPointsCount; // Количество вершин у модели травинки
GLuint grassShader;      // Шейдер, рисующий траву
GLuint grassVAO;         // VAO для травы (что такое VAO почитайте в доках)
GLuint grassVariance;    // Буфер для смещения координат травинок
vector<VM::vec4> grassVarianceData(GRASS_INSTANCES); // Вектор со смещениями для координат травинок

GLuint groundShader; // Шейдер для земли
GLuint groundVAO; // VAO для земли

// Размеры экрана
uint screenWidth = 800;
uint screenHeight = 600;

// Это для захвата мышки. Вам это не потребуется (это не значит, что нужно удалять эту строку)
bool captureMouse = true;

bool msaa_flag = true;

GLuint plantShader;

GLuint grass1_count = 100;
float grass1_height = 20;
float grass1_width = 20;
GLuint grass1_texture;
GLuint grass1VAO;
GLuint grass1_PointCount;
GLuint grass1_Variance;
vector<VM::vec4> grass1_VarianceData(grass1_count);


GLuint grass2_count = 100;
float grass2_height = 4;
float grass2_width = 40;
GLuint grass2_texture;
GLuint grass2VAO;
GLuint grass2_PointCount;
GLuint grass2_Variance;    // Буфер для смещения координат травинок
vector<VM::vec4> grass2_VarianceData(grass2_count); // Вектор со смещениями для координат травинок







void loadOBJ(const char * path,
             std::vector < VM::vec4 > & out_vertices,
             std::vector < VM::vec2 > & out_uvs,
             std::vector < VM::vec4 > & out_normals) {


    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<VM::vec4> temp_vertices;
    std::vector<VM::vec2> temp_uvs;
    std::vector<VM::vec4> temp_normals;

    FILE *file = fopen(path, "r");
    if (file == NULL) {
        printf("Impossible to open the file !\n");
        exit(-1);
    }

    while (true) {

        char lineHeader[128] = {0};
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        // else : parse lineHeader

        if (strcmp(lineHeader, "v") == 0) {
            VM::vec4 vertex;
            vertex.w = 1.0f;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        } else if (strcmp(lineHeader, "vt") == 0) {
            VM::vec2 uv;
            fscanf(file, " %f %f\n", &uv.x, &uv.y);
            temp_uvs.push_back(uv);
        }else if ( strcmp( lineHeader, "f" ) == 0 ){
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
            if (matches != 9){
                printf("File can't be read by our simple parser : ( Try exporting with other options\n");
                exit(-1);
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices    .push_back(uvIndex[0]);
            uvIndices    .push_back(uvIndex[1]);
            uvIndices    .push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
    }

    for (unsigned int i = 0; i < vertexIndices.size(); i++) {
        unsigned int vertexIndex = vertexIndices[i];
        VM::vec4 vertex = temp_vertices[vertexIndex - 1];
        out_vertices.push_back(vertex);
    }
    if (!temp_uvs.empty())
        for (unsigned int i = 0; i < uvIndices.size(); i++) {
            unsigned int vertexIndex = uvIndices[i];
            VM::vec2 uv = temp_uvs[vertexIndex - 1];
            out_uvs.push_back(uv);
        }
}

std::vector< VM::vec4 > vertices;
std::vector< VM::vec2 > uvs;
std::vector< VM::vec4 > normals;
GLfloat* vvv;

GLuint modelShader;
GLuint modelVAO;
GLuint modelVBO;
GLuint model_texture;

void CreateModel() {

    glEnable(GL_TEXTURE_2D);    CHECK_GL_ERRORS

    model_texture = SOIL_load_OGL_texture("../Texture/rock1.jpg",
                                       SOIL_LOAD_AUTO,
                                       SOIL_CREATE_NEW_ID,
                                       SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
    );

    glBindTexture(GL_TEXTURE_2D, model_texture);CHECK_GL_ERRORS
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);CHECK_GL_ERRORS
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);CHECK_GL_ERRORS
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);CHECK_GL_ERRORS
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);CHECK_GL_ERRORS

    glGenerateMipmap(GL_TEXTURE_2D);CHECK_GL_ERRORS

    loadOBJ("../Texture/Rock2.obj", vertices, uvs, normals);

    vvv = new GLfloat[4*vertices.size() + 2*uvs.size()];
    for (uint i = 0 ; i < vertices.size(); i+=1){
        vvv[6*i] = vertices[i].x;
        vvv[6*i+1] = vertices[i].y;
        vvv[6*i+2] = vertices[i].z;
        vvv[6*i+3] = vertices[i].w;
        vvv[6*i+4] = uvs[i].x;
        vvv[6*i+5] = uvs[i].y;
    }

    modelShader = GL::CompileShaderProgram("model");

    glGenVertexArrays(1, &modelVAO);                                            CHECK_GL_ERRORS
    glGenBuffers(1, &modelVBO);                                              CHECK_GL_ERRORS


    glBindVertexArray(modelVAO);                                                CHECK_GL_ERRORS

    glBindBuffer(GL_ARRAY_BUFFER, modelVBO);                                 CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * vertices.size(), vvv, GL_STATIC_DRAW); CHECK_GL_ERRORS

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Tex attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind VAO                                         CHECK_GL_ERRORS
}

void DrawModel(){
    // Используем шейдер для земли
    glUseProgram(modelShader);                                                   CHECK_GL_ERRORS

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, model_texture);
    GLuint model_text_loc = glGetUniformLocation(groundShader, "inTexture");
    glUniform1i(model_text_loc, 0);

    // Устанавливаем юниформ для шейдера. В данном случае передадим перспективную матрицу камеры
    // Находим локацию юниформа 'camera' в шейдере
    GLint cameraLocation = glGetUniformLocation(groundShader, "camera");         CHECK_GL_ERRORS
    // Устанавливаем юниформ (загружаем на GPU матрицу проекции?)
    glUniformMatrix4fv(cameraLocation, 1, GL_TRUE, camera.getMatrix().data().data()); CHECK_GL_ERRORS

    // Подключаем VAO, который содержит буферы, необходимые для отрисовки земли
    glBindVertexArray(modelVAO);                                                CHECK_GL_ERRORS

    // Рисуем землю: 2 треугольника (6 вершин)
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());                             CHECK_GL_ERRORS

    // Отсоединяем VAO
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    // Отключаем шейдер
    glUseProgram(0);                                                             CHECK_GL_ERRORS

}



















// Обновление смещения травинок
void UpdateGrassVariance() {
    static VM::vec4 hooke(0.0f, 0.0f, 0.0f, 0.0f);
    static vector<VM::vec4> velocities(GRASS_INSTANCES, VM::vec4(0.0f, 0.0f, 0.0f, 0.0f));
    static vector<VM::vec4> accelerations(GRASS_INSTANCES, VM::vec4(0.0f, 0.0f, 0.0f, 0.0f));

    // Генерация случайных смещений
    for (uint i = 0; i < GRASS_INSTANCES; ++i) {
        hooke.x = -(k + 50*float(i%(GRASS_INSTANCES/50))/GRASS_INSTANCES) * grassVarianceData[i].x;
        hooke.y = k * abs(grassVarianceData[i].y);
        hooke.z = -(k + 50*float(i%(GRASS_INSTANCES/50))/GRASS_INSTANCES) * grassVarianceData[i].z;

        accelerations[i].x = wind.x + hooke.x;
        accelerations[i].y = hooke.y - g;
        accelerations[i].z = wind.z + hooke.z;

        velocities[i].x += accelerations[i].x * dt;
        velocities[i].y += accelerations[i].y * dt;
        velocities[i].z += accelerations[i].z * dt;

        grassVarianceData[i].x += velocities[i].x * dt;
        grassVarianceData[i].y += velocities[i].y * dt;
        grassVarianceData[i].z += velocities[i].z * dt;
    }

    // Привязываем буфер, содержащий смещения
    glBindBuffer(GL_ARRAY_BUFFER, grassVariance);                                CHECK_GL_ERRORS
    // Загружаем данные в видеопамять
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * GRASS_INSTANCES, grassVarianceData.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
    // Отвязываем буфер
    glBindBuffer(GL_ARRAY_BUFFER, 0);                                            CHECK_GL_ERRORS
}



// Рисование травы
void DrawGrass() {
    // Тут то же самое, что и в рисовании земли
    glUseProgram(grassShader); CHECK_GL_ERRORS

    if (msaa_flag) {
        glEnable(GL_MULTISAMPLE);
    } else {
        glDisable(GL_MULTISAMPLE);
    }

    GLuint height_loc = glGetUniformLocation(grassShader, "GRASS_HEIGHT");
    glUniform1f(height_loc, GRASS_HEIGHT);CHECK_GL_ERRORS

    GLuint width_loc = glGetUniformLocation(grassShader, "GRASS_WIDTH");
    glUniform1f(width_loc, GRASS_WIDTH);CHECK_GL_ERRORS

    glActiveTexture(GL_TEXTURE0);CHECK_GL_ERRORS
    glBindTexture(GL_TEXTURE_2D, grass_texture);
    GLuint grass_text_loc = glGetUniformLocation(grassShader, "inTexture");
    glUniform1i(grass_text_loc, 0);CHECK_GL_ERRORS

    GLint cameraLocation = glGetUniformLocation(grassShader, "camera");          CHECK_GL_ERRORS
    glUniformMatrix4fv(cameraLocation, 1, GL_TRUE, camera.getMatrix().data().data()); CHECK_GL_ERRORS
    glBindVertexArray(grassVAO);                                                 CHECK_GL_ERRORS
    // Обновляем смещения для травы
    UpdateGrassVariance();
    // Отрисовка травинок в количестве GRASS_INSTANCES
    glDrawArraysInstanced(GL_TRIANGLES, 0, grassPointsCount, GRASS_INSTANCES);   CHECK_GL_ERRORS
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    glUseProgram(0);
}

// Функция, рисующая замлю
void DrawGround() {
    // Используем шейдер для земли
    glUseProgram(groundShader);                                                  CHECK_GL_ERRORS

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gr_texture);
    GLuint gr_text_loc = glGetUniformLocation(groundShader, "inTexture");
    glUniform1i(gr_text_loc, 0);

    // Устанавливаем юниформ для шейдера. В данном случае передадим перспективную матрицу камеры
    // Находим локацию юниформа 'camera' в шейдере
    GLint cameraLocation = glGetUniformLocation(groundShader, "camera");         CHECK_GL_ERRORS
    // Устанавливаем юниформ (загружаем на GPU матрицу проекции?)
    glUniformMatrix4fv(cameraLocation, 1, GL_TRUE, camera.getMatrix().data().data()); CHECK_GL_ERRORS

    // Подключаем VAO, который содержит буферы, необходимые для отрисовки земли
    glBindVertexArray(groundVAO);                                                CHECK_GL_ERRORS
    // Рисуем землю: 2 треугольника (6 вершин)
    glDrawArrays(GL_TRIANGLES, 0, 6);                                            CHECK_GL_ERRORS

    // Отсоединяем VAO
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    // Отключаем шейдер
    glUseProgram(0);                                                             CHECK_GL_ERRORS
}



void DrawPlant(GLuint count, GLuint p_count, float h, float w, GLuint tex,
               GLuint VAO, GLuint var, vector<VM::vec4>& vars){
    glUseProgram(plantShader); CHECK_GL_ERRORS

    GLuint height_loc = glGetUniformLocation(plantShader, "GRASS_HEIGHT");
    glUniform1f(height_loc, h);                     CHECK_GL_ERRORS

    GLuint width_loc = glGetUniformLocation(plantShader, "GRASS_WIDTH");
    glUniform1f(width_loc, w);                      CHECK_GL_ERRORS

    glActiveTexture(GL_TEXTURE0);               CHECK_GL_ERRORS
    glBindTexture(GL_TEXTURE_2D, tex);
    GLuint plant_text_loc = glGetUniformLocation(plantShader, "inTexture");
    glUniform1i(plant_text_loc, 0);             CHECK_GL_ERRORS

    GLint cameraLocation = glGetUniformLocation(plantShader, "camera");          CHECK_GL_ERRORS
    glUniformMatrix4fv(cameraLocation, 1, GL_TRUE, camera.getMatrix().data().data()); CHECK_GL_ERRORS
    glBindVertexArray(VAO);                                                 CHECK_GL_ERRORS
    //UpdateGrassVariance(var, vars);

    for (uint i = 0; i < vars.size(); ++i)
        vars[i] = grassVarianceData[i];

    glBindBuffer(GL_ARRAY_BUFFER, var);                                CHECK_GL_ERRORS
    // Загружаем данные в видеопамять

    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * count, grass1_VarianceData.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
    // Отвязываем буфер
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArraysInstanced(GL_TRIANGLES, 0, p_count, count);   CHECK_GL_ERRORS
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    glUseProgram(0);

}










// Эта функция вызывается для обновления экрана
void RenderLayouts() {
    // Включение буфера глубины
    glEnable(GL_DEPTH_TEST);
    // Очистка буфера глубины и цветового буфера
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Рисуем меши
    DrawGround();
    DrawGrass();

    DrawPlant(grass1_count, grass1_PointCount, grass1_height, grass1_width, grass1_texture,
              grass1VAO, grass1_Variance, grass1_VarianceData);

    DrawPlant(grass2_count, grass2_PointCount, grass2_height, grass2_width, grass2_texture,
              grass2VAO, grass2_Variance, grass2_VarianceData);

    DrawModel();

    glutSwapBuffers();
}

// Завершение программы
void FinishProgram() {
    glutDestroyWindow(glutGetWindow());
}

// Обработка события нажатия клавиши (специальные клавиши обрабатываются в функции SpecialButtons)
void KeyboardEvents(unsigned char key, int x, int y) {
    if (key == 27) {
        FinishProgram();
    } else if (key == 'w') {
        camera.goForward(SPEED);
    } else if (key == 'a') {
        msaa_flag = !msaa_flag;
    } else if (key == 's') {
        camera.goBack(SPEED);
    } else if (key == 'm') {
        captureMouse = !captureMouse;
        if (captureMouse) {
            glutWarpPointer(screenWidth / 2, screenHeight / 2);
            glutSetCursor(GLUT_CURSOR_NONE);
        } else {
            glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
        }
    }
}

// Обработка события нажатия специальных клавиш
void SpecialButtons(int key, int x, int y) {
    if (key == GLUT_KEY_RIGHT) {
        camera.rotateY(0.02);
    } else if (key == GLUT_KEY_LEFT) {
        camera.rotateY(-0.02);
    } else if (key == GLUT_KEY_UP) {
        camera.rotateTop(-0.02);
    } else if (key == GLUT_KEY_DOWN) {
        camera.rotateTop(0.02);
    }
}

void IdleFunc() {
    glutPostRedisplay();
}

// Обработка события движения мыши
void MouseMove(int x, int y) {
    if (captureMouse) {
        int centerX = screenWidth / 2,
            centerY = screenHeight / 2;
        if (x != centerX || y != centerY) {
            camera.rotateY((x - centerX) / 1000.0f);
            camera.rotateTop((y - centerY) / 1000.0f);
            glutWarpPointer(centerX, centerY);
        }
    }
}

// Обработка нажатия кнопки мыши
void MouseClick(int button, int state, int x, int y) {
}

// Событие изменение размера окна
void windowReshapeFunc(GLint newWidth, GLint newHeight) {
    glViewport(0, 0, newWidth, newHeight);
    screenWidth = newWidth;
    screenHeight = newHeight;

    camera.screenRatio = (float)screenWidth / screenHeight;
}

// Инициализация окна
void InitializeGLUT(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitContextVersion(3, 0);
    glutInitWindowPosition(-1, -1);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("Computer Graphics 3");
    glutWarpPointer(400, 300);
    glutSetCursor(GLUT_CURSOR_NONE);

    glutDisplayFunc(RenderLayouts);
    glutKeyboardFunc(KeyboardEvents);
    glutSpecialFunc(SpecialButtons);
    glutIdleFunc(IdleFunc);
    glutPassiveMotionFunc(MouseMove);
    glutMouseFunc(MouseClick);
    glutReshapeFunc(windowReshapeFunc);
}

bool is_in_area(const VM::vec2& v){
    return (v.x)*(v.x) + (v.y)*(v.y) > GROUND_X*GROUND_X/4;
}

// Генерация позиций травинок (эту функцию вам придётся переписать)
vector<VM::vec2> GenerateGrassPositions(GLuint num = GRASS_INSTANCES,
                                        std::function<bool(VM::vec2)> f = [](const VM::vec2&){return true;}) {
    vector<VM::vec2> grassPositions(num);
    for (uint i = 0; i < num; ++i) {
        grassPositions[i] = VM::vec2((float)rand()/RAND_MAX*GROUND_X, (float)rand()/RAND_MAX*GROUND_Y);
        while (!f(grassPositions[i]))
            grassPositions[i] = VM::vec2((float)rand()/RAND_MAX*GROUND_X, (float)rand()/RAND_MAX*GROUND_Y);
    }
    return grassPositions;
}

// Здесь вам нужно будет генерировать меш
vector<VM::vec4> GenMesh(uint n = 1) {
    vector<VM::vec4> mesh;
    uint i;
    for (i = 0; i < 3.0f*n/4.0f; ++i){
        mesh.push_back(VM::vec4(0.0f, i*GRASS_HEIGHT/n, 0, 1));
        mesh.push_back(VM::vec4(GRASS_WIDTH, i*GRASS_HEIGHT/n, 0, 1));
        mesh.push_back(VM::vec4(0.0f, (i+1)*GRASS_HEIGHT/n, 0, 1));
        mesh.push_back(VM::vec4(0.0f, (i+1)*GRASS_HEIGHT/n, 0, 1));
        mesh.push_back(VM::vec4(GRASS_WIDTH, (i+1)*GRASS_HEIGHT/n, 0, 1));
        mesh.push_back(VM::vec4(GRASS_WIDTH, (i)*GRASS_HEIGHT/n, 0, 1));
    }

    mesh.push_back(VM::vec4(0.0f, (float)i*GRASS_HEIGHT/n, 0, 1));
    mesh.push_back(VM::vec4(GRASS_WIDTH, (float)i*GRASS_HEIGHT/n, 0, 1));
    mesh.push_back(VM::vec4((float)GRASS_WIDTH/2, GRASS_HEIGHT , 0, 1));

    return mesh;
}

// Создание травы
void CreateGrass() {
    // Создаём меш
    vector<VM::vec4> grassPoints = GenMesh(LOD);
    // Сохраняем количество вершин в меше травы
    grassPointsCount = grassPoints.size();
    // Создаём позиции для травинок
    auto f = [](const VM::vec2 v){ return v.x * v.x + v.y * v.y > GROUND_X * GROUND_X / 4 &&
            v.y < 6.0*GROUND_Y/7; };
    vector<VM::vec2> grassPositions = GenerateGrassPositions(GRASS_INSTANCES, f);
    // Инициализация смещений для травинок
    for (uint i = 0; i < GRASS_INSTANCES; ++i) {
        grassVarianceData[i] = init_variance;
    }


    glEnable(GL_TEXTURE_2D);    CHECK_GL_ERRORS

    grass_texture = SOIL_load_OGL_texture("../Texture/grass.bmp",
                                       SOIL_LOAD_AUTO,
                                       SOIL_CREATE_NEW_ID,
                                       SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
    );

    glBindTexture(GL_TEXTURE_2D, grass_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);CHECK_GL_ERRORS
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);CHECK_GL_ERRORS
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);CHECK_GL_ERRORS
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);CHECK_GL_ERRORS

    glGenerateMipmap(GL_TEXTURE_2D);CHECK_GL_ERRORS

    grassShader = GL::CompileShaderProgram("grass");

    // Здесь создаём буфер
    GLuint pointsBuffer;
    // Это генерация одного буфера (в pointsBuffer хранится идентификатор буфера)
    glGenBuffers(1, &pointsBuffer);                                              CHECK_GL_ERRORS
    // Привязываем сгенерированный буфер
    glBindBuffer(GL_ARRAY_BUFFER, pointsBuffer);                                 CHECK_GL_ERRORS
    // Заполняем буфер данными из вектора
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * grassPoints.size(), grassPoints.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS

    // Создание VAO
    // Генерация VAO
    glGenVertexArrays(1, &grassVAO);                                             CHECK_GL_ERRORS
    // Привязка VAO
    glBindVertexArray(grassVAO);                                                 CHECK_GL_ERRORS

    // Получение локации параметра 'point' в шейдере
    GLuint pointsLocation = glGetAttribLocation(grassShader, "point");           CHECK_GL_ERRORS
    // Подключаем массив атрибутов к данной локации
    glEnableVertexAttribArray(pointsLocation);                                   CHECK_GL_ERRORS
    // Устанавливаем параметры для получения данных из массива (по 4 значение типа float на одну вершину)
    glVertexAttribPointer(pointsLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);          CHECK_GL_ERRORS

    // Создаём буфер для позиций травинок
    GLuint positionBuffer;
    glGenBuffers(1, &positionBuffer);                                            CHECK_GL_ERRORS
    // Здесь мы привязываем новый буфер, так что дальше вся работа будет с ним до следующего вызова glBindBuffer
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);                               CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec2) * grassPositions.size(), grassPositions.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS

    GLuint positionLocation = glGetAttribLocation(grassShader, "position");      CHECK_GL_ERRORS
    glEnableVertexAttribArray(positionLocation);                                 CHECK_GL_ERRORS
    glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);        CHECK_GL_ERRORS
    // Здесь мы указываем, что нужно брать новое значение из этого буфера для каждого инстанса (для каждой травинки)
    glVertexAttribDivisor(positionLocation, 1);                                  CHECK_GL_ERRORS

    // Создаём буфер для смещения травинок
    glGenBuffers(1, &grassVariance);                                            CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, grassVariance);                               CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * GRASS_INSTANCES, grassVarianceData.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS

    GLuint varianceLocation = glGetAttribLocation(grassShader, "variance");      CHECK_GL_ERRORS
    glEnableVertexAttribArray(varianceLocation);                                 CHECK_GL_ERRORS
    glVertexAttribPointer(varianceLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);        CHECK_GL_ERRORS
    glVertexAttribDivisor(varianceLocation, 1);                                  CHECK_GL_ERRORS

    // Отвязываем VAO
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    // Отвязываем буфер
    glBindBuffer(GL_ARRAY_BUFFER, 0);                                            CHECK_GL_ERRORS
}

// Создаём замлю
void CreateGround() {
    glEnable(GL_TEXTURE_2D);    CHECK_GL_ERRORS

    gr_texture = SOIL_load_OGL_texture("../Texture/ground.bmp",
                    SOIL_LOAD_AUTO,
                    SOIL_CREATE_NEW_ID,
                    SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
            );

    glBindTexture(GL_TEXTURE_2D, gr_texture);CHECK_GL_ERRORS
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);CHECK_GL_ERRORS
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);CHECK_GL_ERRORS
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);CHECK_GL_ERRORS
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);CHECK_GL_ERRORS

    glGenerateMipmap(GL_TEXTURE_2D);CHECK_GL_ERRORS

    // Земля состоит из двух треугольников
    vector<VM::vec4> meshPoints = {
        VM::vec4(0, 0, 0, 1),
        VM::vec4(GROUND_X, 0, 0, 1),
        VM::vec4(GROUND_X, 0, GROUND_Y, 1),
        VM::vec4(0, 0, 0, 1),
        VM::vec4(GROUND_X, 0, GROUND_Y, 1),
        VM::vec4(0, 0, GROUND_Y, 1),
    };

    // Подробнее о том, как это работает читайте в функции CreateGrass

    groundShader = GL::CompileShaderProgram("ground");

    GLuint pointsBuffer;
    glGenBuffers(1, &pointsBuffer);                                              CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, pointsBuffer);                                 CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * meshPoints.size(), meshPoints.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS

    glGenVertexArrays(1, &groundVAO);                                            CHECK_GL_ERRORS
    glBindVertexArray(groundVAO);                                                CHECK_GL_ERRORS

    GLuint index = glGetAttribLocation(groundShader, "point");                   CHECK_GL_ERRORS
    glEnableVertexAttribArray(index);                                            CHECK_GL_ERRORS
    glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 0, 0);                   CHECK_GL_ERRORS

    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, 0);                                            CHECK_GL_ERRORS
}



vector<VM::vec4> GenPlantMesh(uint n = 1, float HEIGHT = 1, float WIDTH=1) {
    vector<VM::vec4> mesh;
    uint i;
    for (i = 0; i < n; ++i){
        mesh.push_back(VM::vec4(0.0f, i*HEIGHT/n, 0, 1));
        mesh.push_back(VM::vec4(WIDTH, i*HEIGHT/n, 0, 1));
        mesh.push_back(VM::vec4(0.0f, (i+1)*HEIGHT/n, 0, 1));
        mesh.push_back(VM::vec4(0.0f, (i+1)*HEIGHT/n, 0, 1));
        mesh.push_back(VM::vec4(WIDTH, (i+1)*HEIGHT/n, 0, 1));
        mesh.push_back(VM::vec4(WIDTH, (i)*HEIGHT/n, 0, 1));
    }
    return mesh;
}


void CreatePlant(GLuint count, const char* texpath,
                 GLuint& tex, float h, float w, GLuint& pts,
                 GLuint& VAO, GLuint& var, vector<VM::vec4>& vars, std::function<bool(const VM::vec2)> f){
    vector<VM::vec4> plantPoints = GenPlantMesh(LOD, h, w);

    pts = plantPoints.size();
    vector<VM::vec2> plantPositions = GenerateGrassPositions(count, f);
    for (uint i = 0; i < count; ++i) {
        vars[i] = init_variance;
    }

    glEnable(GL_TEXTURE_2D);    CHECK_GL_ERRORS

    tex = SOIL_load_OGL_texture(texpath,
                                          SOIL_LOAD_AUTO,
                                          SOIL_CREATE_NEW_ID,
                                          SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
    );

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);CHECK_GL_ERRORS
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);CHECK_GL_ERRORS
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);CHECK_GL_ERRORS
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);CHECK_GL_ERRORS

    glGenerateMipmap(GL_TEXTURE_2D);CHECK_GL_ERRORS

    plantShader = GL::CompileShaderProgram("plant");

    GLuint pointsBuffer;
    glGenBuffers(1, &pointsBuffer);                                              CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, pointsBuffer);                                 CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * plantPoints.size(), plantPoints.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS

    glGenVertexArrays(1, &VAO);                                             CHECK_GL_ERRORS
    glBindVertexArray(VAO);                                                 CHECK_GL_ERRORS

    GLuint pointsLocation = glGetAttribLocation(plantShader, "point");           CHECK_GL_ERRORS
    glEnableVertexAttribArray(pointsLocation);                                   CHECK_GL_ERRORS
    glVertexAttribPointer(pointsLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);          CHECK_GL_ERRORS

    GLuint positionBuffer;
    glGenBuffers(1, &positionBuffer);                                            CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);                               CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec2) * plantPositions.size(), plantPositions.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS

    GLuint positionLocation = glGetAttribLocation(plantShader, "position");      CHECK_GL_ERRORS
    glEnableVertexAttribArray(positionLocation);                                 CHECK_GL_ERRORS
    glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);        CHECK_GL_ERRORS
    glVertexAttribDivisor(positionLocation, 1);                                  CHECK_GL_ERRORS

    glGenBuffers(1, &var);                                            CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, var);                               CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * count, vars.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS

    GLuint varianceLocation = glGetAttribLocation(plantShader, "variance");      CHECK_GL_ERRORS
    glEnableVertexAttribArray(varianceLocation);                                 CHECK_GL_ERRORS
    glVertexAttribPointer(varianceLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);        CHECK_GL_ERRORS
    glVertexAttribDivisor(varianceLocation, 1);                                  CHECK_GL_ERRORS

    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, 0);                                            CHECK_GL_ERRORS
}




// Создаём камеру (Если шаблонная камера вам не нравится, то можете переделать, но я бы не стал)
void CreateCamera() {
    camera.angle = 45.0f / 180.0f * M_PI;
    camera.direction = VM::vec3(0.1, 0.3, -0.5);
    camera.position = VM::vec3(5, 3, -3);
    camera.screenRatio = (float)screenWidth / screenHeight;
    camera.up = VM::vec3(0, 1, 0);
    camera.zfar = 50.0f;
    camera.znear = 0.05f;
}


int main(int argc, char **argv)
{
    putenv((char*) "MESA_GL_VERSION_OVERRIDE=3.3COMPAT");
    try {
        cout << "Start" << endl;
        InitializeGLUT(argc, argv);
        cout << "GLUT inited" << endl;
        glewInit();
        //glClearColor(0.0, 0.0, 1.0, 1.0);
        cout << "glew inited" << endl;
        CreateCamera();
        cout << "Camera created" << endl;
        CreateGrass();
        cout << "Grass created" << endl;
        CreateGround();
        cout << "Ground created" << endl;

        auto gr1_pos = [](const VM::vec2 v){ return v.y > GROUND_Y * 6.0 / 7; };

        CreatePlant(grass1_count, "../Texture/bam.bmp", grass1_texture,
                    grass1_height, grass1_width, grass1_PointCount,
                    grass1VAO, grass1_Variance, grass1_VarianceData, gr1_pos);
        cout << "Plant1 created" << endl;

        auto gr2_pos = [](const VM::vec2 v){
            return v.x * v.x + v.y * v.y < GROUND_X * GROUND_X / 5 &&
                    v.x * v.x + v.y * v.y > GROUND_X * GROUND_X / 9 &&
                    v.x > 0.25 && v.y > 0.25;};

        CreatePlant(grass2_count, "../Texture/lop.bmp", grass2_texture,
                    grass2_height, grass2_width, grass2_PointCount,
                    grass2VAO, grass2_Variance, grass2_VarianceData, gr2_pos);
        cout << "Plant2 created" << endl;


        CreateModel();
        cout << "Model created" << endl;

        glutMainLoop();
    } catch (string s) {
        cout << s << endl;
    }
}
