#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include<GL/glad.h>
#include<GL/glfw3.h>
#include<math.h>
#define GLM_ENABLE_EXPERIMENTAL
#include<GL/glm/glm.hpp>
#include<GL/glm/gtc/matrix_transform.hpp>
#include<GL/glm/gtc/type_ptr.hpp>
#include<GL/glm/gtx/rotate_vector.hpp>
#include<GL/glm/gtx/vector_angle.hpp>
#include<windows.h>

#include"shaderClass.h"
#include"shaderClass_Geo.h"

class camera{


public:
    glm::vec3 Position;
    glm::vec3 Orientation= glm::vec3(0.0f,-0.0f,-1.0f);  //glm::vec3(0.6f,-0.30f,-1.0f);
    glm::vec3 Up= glm::vec3(0.0f,1.0f,0.0f);

    bool firstClick = true;
    int width;
    int height;

    float ortho_scale = 60;
    float speed = 1.0f; //for inputs
    float senstivity = 100.0f;//for inputs

    camera(int width,int height,glm::vec3 position);
    camera();

    void Create_and_Send_Matrix(float FOVdeg,float nearPlane,float farPlane,shader& shader,const char* uniform,glm::mat4 World_Matrix); //look at Matrix
    void Create_and_Send_Matrix_ortho(float cam_size,float nearPlane,float farPlane,shader& shader,const char* uniform,glm::mat4 World_Matrix);

    void Send_ProjMat(float FOVdeg,float nearPlane,float farPlane,shader& shader,const char* uniform);
    void Send_ViewMat(float FOVdeg,float nearPlane,float farPlane,shader& shader,const char* uniform);


    void Create_and_Send_Matrix_2(float FOVdeg,float nearPlane,float farPlane,shader& shader,const char* uniform,const char* camPos,glm::mat4 World_Matrix); //look at Matrix
    void Create_and_Send_Matrix_ortho_2(float cam_size,float nearPlane,float farPlane,shader& shader,const char* uniform,const char* camPos,glm::mat4 World_Matrix);


    glm::mat4 inverseProj;

};

#endif // CAMERA_CLASS_H
