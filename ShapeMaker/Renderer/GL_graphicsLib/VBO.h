#ifndef VBO_CLASS_H
#define VBO_CLASS_H
#include<GL/glad.h>
#include <vector>
#include<GL/glm/glm.hpp>

class VBO
{
 public:
    GLuint ID;

    VBO(GLfloat *vertices,GLsizeiptr size);
    VBO(std::vector<glm::vec3>vertices,GLsizeiptr size );
    void Bind();
    void Unbind();
    void Delete();





};


#endif // VBO_CLASS_H
