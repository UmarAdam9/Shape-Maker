#ifndef VAO_CLASS_H
#define VAO_CLASS_H


#include<GL/glad.h>
#include"VBO.h"

class VAO{

public:
    GLuint ID;
    VAO();


    void LinkAttrib(VBO& vbo,GLuint layout,GLuint num_components,GLenum type,GLsizeiptr stride,void*offset);
    void Bind();
    void Unbind();
    void Delete();


};

#endif // VAO_CLASS_H
