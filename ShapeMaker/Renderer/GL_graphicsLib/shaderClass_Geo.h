#ifndef SHADER_CLASS_GEO_H
#define SHADER_CLASS_GEO_H

#include<GL/glad.h>
#include<string>
#include<string.h>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>
#include"shaderClass.h"



class shader_Geo{

public:
    GLuint ID;
    shader_Geo(const char* vertexfile,const char* fragmentFile,const char* geometryFile);

    void Activate();
    void Delete();


};


#endif // SHADER_CLASS_H

