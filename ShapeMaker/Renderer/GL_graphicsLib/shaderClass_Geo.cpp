#include "shaderClass_Geo.h"


// Reads a text file and outputs a string with everything in the text file

shader_Geo::shader_Geo(const char* vertexFile,const char* fragmentFile,const char* geometryFile){


    GLint success;
    std::string vertexCode=get_file_contents(vertexFile);
    std::string fragmentCode=get_file_contents(fragmentFile);
    std::string geometryCode=get_file_contents(geometryFile);

    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();
    const char* geometrySource = geometryCode.c_str();


    //Create vertex shader, link it to source and compile it
    GLuint vertexShader= glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&vertexSource,NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
     if(!success){
        GLchar infoLog[512];
        GLint size; //gives 0 when checked in debugger
        glGetShaderInfoLog(vertexShader, 512, &size, infoLog);
        throw std::runtime_error(std::string("Failed to compile vertexShader: ") + infoLog);
    }

     //Create geometry shader, link it to source and compile it
    GLuint geometryShader= glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometryShader,1,&geometrySource,NULL);
    glCompileShader(geometryShader);
    glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
    if(!success){
        GLchar infoLog[512];
        GLint size; //gives 0 when checked in debugger
        glGetShaderInfoLog(geometryShader, 512, &size, infoLog);
        throw std::runtime_error(std::string("Failed to compile geometry shader: ") + infoLog);
    }





    //Create fragment shader, link it to source and compile it
    GLuint fragmentShader= glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&fragmentSource,NULL);
    glCompileShader(fragmentShader);

     glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
     if(!success){
        GLchar infoLog[512];
        GLint size; //gives 0 when checked in debugger
        glGetShaderInfoLog(fragmentShader, 512, &size, infoLog);
        throw std::runtime_error(std::string("Failed to compile fragment Shader: ") + infoLog);
    }






    //create Shader program now

     ID =glCreateProgram();

    //Attach the shaders to the shader program

    glAttachShader(ID,vertexShader);
    glAttachShader(ID,geometryShader);
    glAttachShader(ID,fragmentShader);

    //Link the program

    glLinkProgram(ID);


    glGetProgramiv(ID,GL_LINK_STATUS,&success);
    if(!success){
        GLchar infoLog[512];
        GLint size; //gives 0 when checked in debugger
        glGetProgramInfoLog(ID, 512, &size, infoLog);
        throw std::runtime_error(std::string("Failed to link shader program: ") + infoLog);
    }


    //Delete the now useless shader objects because they are now part of shaderProgram
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
   // glDeleteShader(geometryShader);



}
void shader_Geo::Activate(){

 glUseProgram(ID);


}
void shader_Geo::Delete(){
glDeleteProgram(ID);
}


