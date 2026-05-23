#include<iostream>
#include<vector>
#include <algorithm> // Required for std::max_element

#include<GL/glad.h>
#include<GL/glfw3.h>
#include<math.h>
#define GLM_ENABLE_EXPERIMENTAL
#include<GL/glm/glm.hpp>
#include<GL/glm/gtc/matrix_transform.hpp>
#include<GL/glm/gtc/type_ptr.hpp>
#include<GL/glm/gtx/rotate_vector.hpp>
#include<GL/glm/gtx/vector_angle.hpp>

#include"GL_graphicsLib/shaderClass.h"
#include"GL_graphicsLib/VAO.h"
#include"GL_graphicsLib/VBO.h"
#include"GL_graphicsLib/EBO.h"
#include"GL_graphicsLib/camera.h"
#include"GL_graphicsLib/Texture.h"
#include"GL_graphicsLib/FrameBuffer.h"

using namespace glm;


enum RenderType
{
    DRAW_TRIANGLES,
    DRAW_LINES,
    DRAW_TRIANGLES_INDEX,
    DRAW_2D
};





class Renderer
{
public:

    int renderer_width;
    int renderer_height;

    /*****GL_TRIANGLES_INDEX************************************************/
    int init_index_triangles;
    //vertex and Index buffers for drawing stuff with GL_TRIANGLES
    std::vector<float>local_vertex_buffer;
    std::vector<GLuint>local_index_buffer;
    int index_counter = 0;
    VAO VAO1;
	GLuint VBO1;
	shader shaderProgram;

    /*****GL_LINE******************************************************/
     int init_line;
    //vertex buffer for drawing stuff with GL_LINES
    std::vector<float>local_line_vertex_buffer;
    VAO VAO_line;
	GLuint VBO_line;
	shader shaderProgram_line;


    /*****GL_TRIANGLES********************************************/
     int init_triangles;
    //vertex and Index buffers for drawing stuff with GL_TRIANGLES
    std::vector<float>local_vertex_buffer_without_indices;
    VAO VAO_triangles;
	GLuint VBO_triangles;
	shader shaderProgram_triangles;


    /*****2D Renderer********************************************/
     int init_2D;
    //vertex and Index buffers for drawing 2D stuff
    std::vector<float>local_vertex_buffer_2D;
    VAO VAO_2D;
	GLuint VBO_2D;
	shader shaderProgram_2D;
    float z_coord2D;
    int local_vertex_buffer_2D_size;

    /*****GL_MANUAL************************************************/
    int init_manual;
    //vertex and Index buffers for drawing stuff with GL_TRIANGLES
    std::vector<float>local_vertex_buffer_manual;
    VAO VAO_manual;
	GLuint VBO_manual;
	shader shaderProgram_manual;

    /******Frame buffers*******************************************/
    //const int bloomMipChainLen= 8;
    shader downsample_shader;
    shader upsample_shader;
    shader brightness_shader;

    FrameBuffer FBO[16];



    /****Post processing Quad*****************************/
    int init_postProcessing;
    VAO PostProcessingQuad_VAO;
    GLuint PostProcessingQuad_VBO;
    float PostProcessingQuad_Vertices[24] =
    {
        // positions      // texCoords
        // Triangle 1
        -1.0f, -1.0f,      0.0f, 0.0f,
         1.0f, -1.0f,      1.0f, 0.0f,
         1.0f,  1.0f,      1.0f, 1.0f,

        // Triangle 2
        -1.0f, -1.0f,      0.0f, 0.0f,
         1.0f,  1.0f,      1.0f, 1.0f,
        -1.0f,  1.0f,      0.0f, 1.0f
    };




    /**************Drawing Methods***********************/
    Renderer(int enable_index_triangle , int enable_triangles,int enable_line, int enable_2D ,int enable_manual, int enable_postProcessing)
    {
        init_2D = enable_2D;
        init_line = enable_line;
        init_triangles = enable_triangles;
        init_index_triangles = enable_index_triangle;
        init_postProcessing = enable_postProcessing;
        init_manual = enable_manual;

    }


    void init_renderer(int init_size_float,int width,int height)                //allocates space on the GPU and creates the frame buffers and their attachments
    {
        for(int i=0;i<8;i++)
            FBO[i].setup(width,height);


        for(int i=0;i<8;i++)
        {
            static int mipSize_x = width;
            static int mipSize_y = height;

            FBO[i+8].setupBloom(mipSize_x, mipSize_y);

            mipSize_x /= 2;
            mipSize_y /= 2;

        }

        downsample_shader = shader("Renderer/Renderer_Shaders/VertexShader_PostProcessing.txt" , "Renderer/Renderer_Shaders/FragmentShader_Downsample.txt");
        upsample_shader   = shader("Renderer/Renderer_Shaders/VertexShader_PostProcessing.txt" , "Renderer/Renderer_Shaders/FragmentShader_Upsample.txt");
        brightness_shader = shader("Renderer/Renderer_Shaders/VertexShader_PostProcessing.txt" , "Renderer/Renderer_Shaders/FragmentShader_Brightness.txt");

        renderer_width = width;
        renderer_height= height;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if(init_index_triangles)
        {
            /*****GL_TRIANGLES_INDEX************************************************/
                local_vertex_buffer.clear();
                local_index_buffer.clear();

                VAO1.Bind();
                glGenBuffers(1,&VBO1);
                glBindBuffer(GL_ARRAY_BUFFER,VBO1);
                glBufferData(GL_ARRAY_BUFFER, init_size_float*sizeof(float) ,nullptr,GL_DYNAMIC_DRAW);

                //vertex Attribute
                glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
                glEnableVertexAttribArray(1);

                shaderProgram = shader("Renderer/Renderer_Shaders/VertexShader.txt" , "Renderer/Renderer_Shaders/FragmentShader.txt");

                VAO1.Unbind();
        }

        if(init_line)
        {
             /*****GL_LINES************************************************/
                glLineWidth(1); // Set line width to 3 pixels
                glEnable(GL_LINE_SMOOTH);

                local_line_vertex_buffer.clear();
                VAO_line.Bind();
                glGenBuffers(1,&VBO_line);
                glBindBuffer(GL_ARRAY_BUFFER,VBO_line);
                glBufferData(GL_ARRAY_BUFFER, init_size_float*sizeof(float) ,nullptr,GL_DYNAMIC_DRAW);

                //vertex Attribute
                glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,7*sizeof(float),(void*)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,7*sizeof(float),(void*)(3*sizeof(float)));
                glEnableVertexAttribArray(1);
                try{
                 shaderProgram_line = shader("Renderer/Renderer_Shaders/VertexShader_Line.txt","Renderer/Renderer_Shaders/FragmentShader_Line.txt");
                }
                catch(int e){
                    std::cerr << "Shader loading failed. Error code: " << e << std::endl;
                }
                VAO_line.Unbind();
        }
        if(init_triangles)
        {
            /******GL_TRIANGLES************************************************/
                local_vertex_buffer_without_indices.clear();

                VAO_triangles.Bind();
                glGenBuffers(1,&VBO_triangles);
                glBindBuffer(GL_ARRAY_BUFFER,VBO_triangles);
                glBufferData(GL_ARRAY_BUFFER, init_size_float*sizeof(float) ,nullptr,GL_DYNAMIC_DRAW);

                //vertex Attribute
                glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,16*sizeof(float),(void*)0);                  //local coordinate :vec3
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,16*sizeof(float),(void*)(3*sizeof(float)));  //position :vec3
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,16*sizeof(float),(void*)(6*sizeof(float)));  //normal :vec3
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,16*sizeof(float),(void*)(9*sizeof(float)));  //Albedo :vec3
                glEnableVertexAttribArray(3);
                glVertexAttribPointer(4,1,GL_FLOAT,GL_FALSE,16*sizeof(float),(void*)(12*sizeof(float)));  //Roughness :float
                glEnableVertexAttribArray(4);
                glVertexAttribPointer(5,3,GL_FLOAT,GL_FALSE,16*sizeof(float),(void*)(13*sizeof(float)));  //base reflectance :vec3
                glEnableVertexAttribArray(5);


                shaderProgram_triangles = shader("Renderer/Renderer_Shaders/VertexShaders_triangles.txt","Renderer/Renderer_Shaders/FragmentShader_triangles.txt");

                VAO_triangles.Unbind();
        }
        if(init_2D)
        {
            /******GL_2D************************************************/
                z_coord2D =0.1;
                local_vertex_buffer_2D.clear();
                VAO_2D.Bind();
                glGenBuffers(1,&VBO_2D);
                glBindBuffer(GL_ARRAY_BUFFER,VBO_2D);
                glBufferData(GL_ARRAY_BUFFER, init_size_float*sizeof(float) ,nullptr,GL_DYNAMIC_DRAW);

                //vec2 WorldPos , vec2 uvPos, vec3 color , float length , float thickness , float mode


                //vertex Attribute
                glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,11*sizeof(float),(void*)0);         //vec3 world pos
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,11*sizeof(float),(void*)(3*sizeof(float)));   //vec2 uv pos
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,11*sizeof(float),(void*)(5*sizeof(float)));   //vec3 color
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(3,1,GL_FLOAT,GL_FALSE,11*sizeof(float),(void*)(8*sizeof(float)));    //float len
                glEnableVertexAttribArray(3);
                glVertexAttribPointer(4,1,GL_FLOAT,GL_FALSE,11*sizeof(float),(void*)(9*sizeof(float)));    //float thickness
                glEnableVertexAttribArray(4);
                glVertexAttribPointer(5,1,GL_FLOAT,GL_FALSE,11*sizeof(float),(void*)(10*sizeof(float)));    //float mode
                glEnableVertexAttribArray(5);


                shaderProgram_2D = shader("Renderer/Renderer_Shaders/VertexShader_2D.txt","Renderer/Renderer_Shaders/FragmentShader_2D.txt");

                VAO_2D.Unbind();
        }
        if(init_manual)
        {
                /******GL_manual************************************************/
                local_vertex_buffer_manual.clear();
                VAO_manual.Bind();
                glGenBuffers(1,&VBO_manual);
                glBindBuffer(GL_ARRAY_BUFFER,VBO_manual);
                glBufferData(GL_ARRAY_BUFFER, init_size_float*sizeof(float) ,nullptr,GL_DYNAMIC_DRAW);

                //vertex Attribute
                glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);         //vec2 pos
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(2*sizeof(float)));   //vec4 color
                glEnableVertexAttribArray(1);

                shaderProgram_manual = shader("Renderer/Renderer_Shaders/VertexShader_manual.txt","Renderer/Renderer_Shaders/FragmentShader_manual.txt");

                VAO_manual.Unbind();

        }
        if(init_postProcessing)
        {
                PostProcessingQuad_VAO.Bind();
                glGenBuffers(1,&PostProcessingQuad_VBO);
                glBindBuffer(GL_ARRAY_BUFFER,PostProcessingQuad_VBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(PostProcessingQuad_Vertices),PostProcessingQuad_Vertices, GL_STATIC_DRAW);

                //vertex Attribute
                glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)0);
                glEnableVertexAttribArray(0);

                glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)(2*sizeof(float)));
                glEnableVertexAttribArray(1);

                PostProcessingQuad_VAO.Unbind();
        }



    }

    void clear_bg(vec4 bg_col)
    {
        glClearColor(bg_col.r,bg_col.g,bg_col.b,bg_col.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

    }

    void Draw(float* vertices , int vert_size , int* indices, int indices_size, vec3 position )	//takes in vertex data and an index buffer -> adds it to the local vertex and index buffer
    {
        for (int i = 0; i < vert_size; i += 3)
        {
            // Vertex position
            float vx = vertices[i];
            float vy = vertices[i + 1];
            float vz = vertices[i + 2];

            // Append vertex
            local_vertex_buffer.push_back(vx);
            local_vertex_buffer.push_back(vy);
            local_vertex_buffer.push_back(vz);

            // Append collider position
            local_vertex_buffer.push_back(position.x);
            local_vertex_buffer.push_back(position.y);
            local_vertex_buffer.push_back(position.z);
        }

        for(int i=0; i<indices_size ; i++)
            local_index_buffer.push_back(indices[i]+index_counter);

        int maxValue = *std::max_element(local_index_buffer.begin(), local_index_buffer.end());
        index_counter=maxValue+1;

    }

    void DrawLine(vec3 p0,vec3 p1,vec4 col,vec4 col2)
    {
        local_line_vertex_buffer.push_back(p0.x);
        local_line_vertex_buffer.push_back(p0.y);
        local_line_vertex_buffer.push_back(p0.z);

        local_line_vertex_buffer.push_back(col.x);
        local_line_vertex_buffer.push_back(col.y);
        local_line_vertex_buffer.push_back(col.z);
        local_line_vertex_buffer.push_back(col.a);

        local_line_vertex_buffer.push_back(p1.x);
        local_line_vertex_buffer.push_back(p1.y);
        local_line_vertex_buffer.push_back(p1.z);

        local_line_vertex_buffer.push_back(col2.x);
        local_line_vertex_buffer.push_back(col2.y);
        local_line_vertex_buffer.push_back(col2.z);
        local_line_vertex_buffer.push_back(col2.a);


    }

    void Draw(float* vertices , int vert_size,float* normals,vec3 position,vec3 albedo , float roughness , vec3 reflectance)
    {
          for (int i = 0; i < vert_size; i += 3)
          {
            //Vertex position
            float vx = vertices[i];
            float vy = vertices[i + 1];
            float vz = vertices[i + 2];

            //Append vertex
            local_vertex_buffer_without_indices.push_back(vx);
            local_vertex_buffer_without_indices.push_back(vy);
            local_vertex_buffer_without_indices.push_back(vz);

            //Append collider position
            local_vertex_buffer_without_indices.push_back(position.x);
            local_vertex_buffer_without_indices.push_back(position.y);
            local_vertex_buffer_without_indices.push_back(position.z);

            //Normals
            local_vertex_buffer_without_indices.push_back(normals[i]);
            local_vertex_buffer_without_indices.push_back(normals[i+1]);
            local_vertex_buffer_without_indices.push_back(normals[i+2]);

            //Albedo
            local_vertex_buffer_without_indices.push_back(albedo.x);
            local_vertex_buffer_without_indices.push_back(albedo.y);
            local_vertex_buffer_without_indices.push_back(albedo.z);

            //roughness
            local_vertex_buffer_without_indices.push_back(roughness);

            //Albedo
            local_vertex_buffer_without_indices.push_back(reflectance.x);
            local_vertex_buffer_without_indices.push_back(reflectance.y);
            local_vertex_buffer_without_indices.push_back(reflectance.z);

          }

    }

    void Draw_manual(vec2 vertices ,vec4 albedo)
    {
        //Vertex position
        float vx = vertices.x;
        float vy = vertices.y;

        //Append vertex
        local_vertex_buffer_manual.push_back(vx);
        local_vertex_buffer_manual.push_back(vy);

        //Albedo
        local_vertex_buffer_manual.push_back(albedo.x);
        local_vertex_buffer_manual.push_back(albedo.y);
        local_vertex_buffer_manual.push_back(albedo.z);
        local_vertex_buffer_manual.push_back(albedo.a);
    }

    void DrawLine2D(vec2 p1, vec2 p2, vec3 col, float thickness , float scale)
    {

        //vec2 WorldPos , vec2 uvPos, vec3 color , float length , float thickness , float mode
        p1*=scale; p2*=scale;
        vec2 dir = p2 -p1;
        float line_len = length(dir);
        vec2 normal = vec2(-1*dir.y,dir.x);
        normal = normalize(normal);

        vec2 v1 = p1-normal;
        vec2 v2 = p1+normal;
        vec2 v3 = p2+normal;
        vec2 v4 = p2-normal;


        float squareRatio;
        if(renderer_width > renderer_height)
            squareRatio = (float)renderer_height / (float)renderer_width;
        else if(renderer_height>renderer_width)
            squareRatio = renderer_width / (float)renderer_height ;
        else
            squareRatio = 1;



        for(int i=0;i<6;i++)
        {
            vec2 WorldPos;
            vec2 uv;

            switch(i)
            {
                case 0:
                {
                    uv.x =  -1*line_len/2;
                    uv.y = -1;

                    WorldPos.x = v1.x;
                    WorldPos.y = v1.y;


                    break;
                }

                case 1:
                {
                    uv.x = -line_len/2;
                    uv.y = 1;

                    WorldPos.x = v2.x;
                    WorldPos.y = v2.y;


                    break;
                }

                case 2:
                {
                    uv.x = line_len/2;
                    uv.y = 1;

                    WorldPos.x = v3.x;
                    WorldPos.y = v3.y;

                    break;
                }

                case 3:
                {
                    uv.x =  -line_len/2;
                    uv.y = -1;

                    WorldPos.x = v1.x;
                    WorldPos.y = v1.y;

                    break;
                }

                case 4:
                {
                    uv.x = line_len/2;
                    uv.y = 1;

                    WorldPos.x = v3.x;
                    WorldPos.y = v3.y;


                    break;
                }

                case 5:
                {
                    uv.x = line_len/2;
                    uv.y = -1;

                    WorldPos.x = v4.x;
                    WorldPos.y = v4.y;

                    break;
                }

            }


            local_vertex_buffer_2D.push_back(WorldPos.x);
            local_vertex_buffer_2D.push_back(WorldPos.y);
            local_vertex_buffer_2D.push_back(z_coord2D);
            local_vertex_buffer_2D.push_back(uv.x);
            local_vertex_buffer_2D.push_back(uv.y);
            local_vertex_buffer_2D.push_back(col.x);
            local_vertex_buffer_2D.push_back(col.y);
            local_vertex_buffer_2D.push_back(col.z);
            local_vertex_buffer_2D.push_back(line_len);
            local_vertex_buffer_2D.push_back(thickness);
            local_vertex_buffer_2D.push_back(0.2f);
        }
        z_coord2D+=0.0;
    }

     void DrawCircle2D(vec2 center, float radius, vec3 col, float fill_param , float scale)
    {

        center*=scale;
        radius*=scale;

        float thickness = 0.0f;
        for(int i=0;i<6;i++)
        {
            vec2 WorldPos;
            vec2 uv;

            switch(i)
            {
                case 0:
                {
                    uv.x =  -1;
                    uv.y = -1;

                    WorldPos.x = -1;
                    WorldPos.y = -1;


                    break;
                }

                case 1:
                {
                    uv.x = -1;
                    uv.y = 1;

                    WorldPos.x = -1;
                    WorldPos.y = 1;


                    break;
                }

                case 2:
                {
                    uv.x = 1;
                    uv.y = 1;

                    WorldPos.x = 1;
                    WorldPos.y = 1;

                    break;
                }

                case 3:
                {
                    uv.x =  -1;
                    uv.y = -1;

                    WorldPos.x = -1;
                    WorldPos.y = -1;

                    break;
                }

                case 4:
                {
                    uv.x = 1;
                    uv.y = 1;

                    WorldPos.x = 1;
                    WorldPos.y = 1;


                    break;
                }

                case 5:
                {
                    uv.x = 1;
                    uv.y = -1;

                    WorldPos.x = 1;
                    WorldPos.y = -1;

                    break;
                }

            }

            vec2 squareRatio;
            if(renderer_width > renderer_height)
                squareRatio = vec2((float)renderer_height / (float)renderer_width , 1.0f);
            else if(renderer_height>renderer_width)
                squareRatio = vec2(1.0f ,  (float)renderer_width / (float)renderer_height );
            else
                squareRatio = vec2(1.0f,1.0f);




             if(radius > 5)
             {
                  thickness = 0.2/(radius * 0.25);

             }

             else
             {
                  thickness = 0.2;

             }


            local_vertex_buffer_2D.push_back(WorldPos.x*squareRatio.x*radius + center.x);
            local_vertex_buffer_2D.push_back(WorldPos.y*squareRatio.y*radius + center.y);
            local_vertex_buffer_2D.push_back(z_coord2D);
            local_vertex_buffer_2D.push_back(uv.x*2);
            local_vertex_buffer_2D.push_back(uv.y*2);
            local_vertex_buffer_2D.push_back(col.x);
            local_vertex_buffer_2D.push_back(col.y);
            local_vertex_buffer_2D.push_back(col.z);
            local_vertex_buffer_2D.push_back(fill_param);
            local_vertex_buffer_2D.push_back(thickness);
            local_vertex_buffer_2D.push_back(1.2f);
        }
        z_coord2D+=0.0;
    }

    void DrawPostProcessingFrame(int framebufferID)
    {
        glBindTexture(GL_TEXTURE_2D,framebufferID);
         //vec2 WorldPos , vec2 uvPos, vec3 color , float length , float thickness , float mode
        for(int i=0;i<6;i++)
        {
            vec2 WorldPos;
            vec2 uv;

            switch(i)
            {
                case 0:
                {
                    uv.x =  0;
                    uv.y = 0;

                    WorldPos.x = -1;
                    WorldPos.y = -1;


                    break;
                }

                case 1:
                {
                    uv.x = 0;
                    uv.y = 1;

                    WorldPos.x = -1;
                    WorldPos.y = 1;


                    break;
                }

                case 2:
                {
                    uv.x = 1;
                    uv.y = 1;

                    WorldPos.x = 1;
                    WorldPos.y = 1;

                    break;
                }

                case 3:
                {
                    uv.x =  0;
                    uv.y = 0;

                    WorldPos.x = -1;
                    WorldPos.y = -1;

                    break;
                }

                case 4:
                {
                    uv.x = 1;
                    uv.y = 1;

                    WorldPos.x = 1;
                    WorldPos.y = 1;


                    break;
                }

                case 5:
                {
                    uv.x = 1;
                    uv.y = 0;

                    WorldPos.x = 1;
                    WorldPos.y = -1;

                    break;
                }

            }


            local_vertex_buffer_2D.push_back(WorldPos.x );
            local_vertex_buffer_2D.push_back(WorldPos.y);
            local_vertex_buffer_2D.push_back(0.0f);
            local_vertex_buffer_2D.push_back(uv.x);
            local_vertex_buffer_2D.push_back(uv.y);
            local_vertex_buffer_2D.push_back(0.0);
            local_vertex_buffer_2D.push_back(0);
            local_vertex_buffer_2D.push_back(0);
            local_vertex_buffer_2D.push_back(0);
            local_vertex_buffer_2D.push_back(0);
            local_vertex_buffer_2D.push_back(2.2f);
        }
        z_coord2D+=0.0;


    }

    void Render(camera &camera_1,int ortho = 0, float cam_size=20 , float scale = 1)		                 //Actually calls glDrawElements,glDrawArrays or glDrawLines
    {

        cam_size = camera_1.ortho_scale;

        if(init_index_triangles)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);

            //Bind VAO
            VAO1.Bind();
            //create index buffer
            EBO EBO1(&local_index_buffer[0],local_index_buffer.size()*sizeof(GLuint));
            //bind VBO
            glBindBuffer(GL_ARRAY_BUFFER,VBO1);
            //Bind index buffer
            EBO1.Bind();
            //populate the allocated memory with local vertex buffer
            glBufferSubData(GL_ARRAY_BUFFER, 0, local_vertex_buffer.size()*sizeof(float) ,&local_vertex_buffer[0]);
            //Activate shader
            shaderProgram.Activate();

            //send camera for camera multiplication
            !ortho? camera_1.Create_and_Send_Matrix(45.0f,0.1f,1000.0f,shaderProgram,"camMatrix",glm::mat4(1.0f)) : camera_1.Create_and_Send_Matrix_ortho(cam_size,0.1f,1000.0f,shaderProgram,"camMatrix",glm::mat4(1.0f));
            //glDrawArrays(GL_TRIANGLES,0,3);
            glDrawElements(GL_TRIANGLES,local_index_buffer.size(),GL_UNSIGNED_INT,nullptr);


            local_vertex_buffer.clear();
            local_index_buffer.clear();
            index_counter=0;

        }



        if(init_triangles)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);

            //Bind VAO
            VAO_triangles.Bind();
            //bind VBO
            glBindBuffer(GL_ARRAY_BUFFER,VBO_triangles);
            //populate the allocated memory with local vertex buffer
            glBufferSubData(GL_ARRAY_BUFFER, 0, local_vertex_buffer_without_indices.size()*sizeof(float) ,&local_vertex_buffer_without_indices[0]);
            //Activate shader
            shaderProgram_triangles.Activate();
            //send camera for camera multiplication
            !ortho? camera_1.Create_and_Send_Matrix_2(45.0f,0.1f,1000.0f,shaderProgram_triangles,"camMatrix","camPos",glm::mat4(1.0f)) : camera_1.Create_and_Send_Matrix_ortho_2(cam_size,0.1f,1000.0f,shaderProgram_triangles,"camMatrix","camPos",glm::mat4(1.0f));
            //glDrawArrays(GL_TRIANGLES,0,3);
            glDrawArrays(GL_TRIANGLES,0,local_vertex_buffer_without_indices.size()/16);


            local_vertex_buffer_without_indices.clear();

        }


         if(init_line)
        {
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);

            //Bind VAO
            VAO_line.Bind();
            //Activate shader
            shaderProgram_line.Activate();
            //Bind VBO
            glBindBuffer(GL_ARRAY_BUFFER,VBO_line);
            //populate the allocated memory with local vertex buffer
            glBufferSubData(GL_ARRAY_BUFFER, 0, local_line_vertex_buffer.size()*sizeof(float) ,&local_line_vertex_buffer[0]);
            //send camera for camera multiplication
            !ortho? camera_1.Create_and_Send_Matrix(45.0f,0.1f,1000.0f,shaderProgram_line,"camMatrix",glm::mat4(1.0f)) : camera_1.Create_and_Send_Matrix_ortho(cam_size,0.1f,1000.0f,shaderProgram_line,"camMatrix",glm::mat4(1.0f));
            //glDrawArrays(GL_TRIANGLES,0,3);
            glDrawArrays(GL_LINES,0,local_line_vertex_buffer.size()/7);


            local_line_vertex_buffer.clear();

        }

         if(init_manual)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);

            VAO_manual.Bind();
            glBindBuffer(GL_ARRAY_BUFFER,VBO_manual);
            glBufferSubData(GL_ARRAY_BUFFER, 0, local_vertex_buffer_manual.size()*sizeof(float) ,&local_vertex_buffer_manual[0]);
            shaderProgram_manual.Activate();
            !ortho? camera_1.Create_and_Send_Matrix(45.0f,0.1f,1000.0f,shaderProgram_manual,"camMatrix",glm::mat4(1.0f)) : camera_1.Create_and_Send_Matrix_ortho(cam_size,0.1f,1000.0f,shaderProgram_manual,"camMatrix",glm::mat4(1.0f));
            glDrawArrays(GL_TRIANGLES,0,local_vertex_buffer_manual.size()/ 6);
            local_vertex_buffer_manual.clear();

          //  glDepthMask(GL_TRUE);


        }

        if(init_2D)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);

            VAO_2D.Bind();
            glBindBuffer(GL_ARRAY_BUFFER,VBO_2D);
            glBufferSubData(GL_ARRAY_BUFFER, 0, local_vertex_buffer_2D.size()*sizeof(float) ,&local_vertex_buffer_2D[0]);
            shaderProgram_2D.Activate();

//            glUniform1f(glGetUniformLocation(shaderProgram_2D.ID, "scale"),scale);
//            if(glGetUniformLocation(shaderProgram_2D.ID,"scale") == -1) std::cout << "scale Uniform not found in shaderProgram_2D"<<std::endl;

            !ortho? camera_1.Create_and_Send_Matrix(45.0f,0.1f,1000.0f,shaderProgram_2D,"camMatrix",glm::mat4(1.0f)) : camera_1.Create_and_Send_Matrix_ortho(cam_size,0.1f,1000.0f,shaderProgram_2D,"camMatrix",glm::mat4(1.0f));
            glDrawArrays(GL_TRIANGLES,0,local_vertex_buffer_2D.size()/ 11);
            local_vertex_buffer_2D_size = local_vertex_buffer_2D.size();
            local_vertex_buffer_2D.clear();
            z_coord2D=0.1;

            glDepthMask(GL_TRUE);

        }

    }

    void RenderPostProcessingFrame(int framebufferID_out,int framebufferID_in, shader& PostProcessing_Shader )
    {
         glDisable(GL_DEPTH_TEST);

        //bind the framebuffer
         FBO[framebufferID_out].Bind();

        //Bind shader
         PostProcessing_Shader.Activate();

        //Bind the texture to be used by the quad
         glActiveTexture(GL_TEXTURE0);
         glBindTexture(GL_TEXTURE_2D,FBO[framebufferID_in].framebufferTexture[0]);
         int loc = glGetUniformLocation(PostProcessing_Shader.ID, "screenTexture");
         //if(loc == -1) std::cout << "Uniform not found! for input" <<framebufferID_in<< " and output"<<framebufferID_out<<std::endl;
         glUniform1i(loc, 0);
         //PostProcessing_Shader.setInt("screenTexture", 0);

        //bind VAO
         PostProcessingQuad_VAO.Bind();

        //bind VBO
        //glBindBuffer(GL_ARRAY_BUFFER,PostProcessingQuad_VBO); not needed?

        //call glDrawArrays
         glDrawArrays(GL_TRIANGLES, 0, 6);

         glEnable(GL_DEPTH_TEST);
    }

    void ApplyBloom(int framebufferID)
    {
        //apply post-processing brightness shader to framebuffer
        RenderPostProcessingFrame(7, framebufferID, brightness_shader );//Post-process render on Frame buffer 7 with framebuffer 0 as input

        //shader bloom_shader("VertexShader_PostProcessing.txt" , "FragmentShader_PostProcessing.txt");
        downsample_shader.Activate();
        for(int i=0;i<8;i++)
        {
            glUniform2f(glGetUniformLocation(downsample_shader.ID, "srcResolution"), (float)FBO[8+i].width,(float)FBO[8+i].height);
            if(glGetUniformLocation(downsample_shader.ID, "srcResolution") == -1) std::cout << "Uniform not found"<<std::endl;

            if(i==0)
                RenderPostProcessingFrame(8+i,7,downsample_shader);

            else
               RenderPostProcessingFrame(8+i,8+i-1,downsample_shader);
        }

        upsample_shader.Activate();

         // Enable additive blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glBlendEquation(GL_FUNC_ADD);

         for(int i=7;i>0;i--)
        {
            RenderPostProcessingFrame(7+i-1,7+i,upsample_shader);
        }

        RenderPostProcessingFrame(framebufferID,7,upsample_shader);

         // Disable additive blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

};


                                                                            //y and x spacing
void Draw_XY_grid(Renderer* myRenderer ,int num_lines , float lineLen, float grid_scale , vec4 gridCol ,vec4 x_col, vec4 y_col)
{
      //draw lines
      myRenderer->DrawLine(vec3(-lineLen,0,0),vec3(lineLen,0,0),x_col,x_col);
      myRenderer->DrawLine(vec3(0,-lineLen,0),vec3(0,lineLen,0),y_col,y_col);
      myRenderer->DrawLine(vec3(0,0,-lineLen),vec3(0,0,lineLen),vec4(0.0,0.0,0.0,1.0),vec4(0.0,0.0,0.0,1.0));

      for(int i=0;i<num_lines  ;i++)
      {
        myRenderer->DrawLine(vec3(-lineLen,-i*grid_scale,0),vec3(lineLen,-i*grid_scale,0),gridCol,gridCol);
        myRenderer->DrawLine(vec3(-lineLen,i*grid_scale,0),vec3(lineLen,i*grid_scale,0),gridCol,gridCol);

        myRenderer->DrawLine(vec3(i*grid_scale,-lineLen,0),vec3(i*grid_scale,lineLen,0),gridCol,gridCol);
        myRenderer->DrawLine(vec3(-i*grid_scale,-lineLen,0),vec3(-i*grid_scale,lineLen,0),gridCol,gridCol);
      }
}
void Draw_ZX_grid(Renderer* myRenderer ,int num_lines , float lineLen, float grid_scale , vec4 gridCol)
{
      //draw lines
      myRenderer->DrawLine(vec3(-lineLen,0,0),vec3(lineLen,0,0),vec4(0.9,0.2,0,1.0),gridCol);
      myRenderer->DrawLine(vec3(0,-lineLen,0),vec3(0,lineLen,0),vec4(0,0,1,1.0),gridCol);
      myRenderer->DrawLine(vec3(0,0,-lineLen),vec3(0,0,lineLen),vec4(0.3,0.8,0.0,1.0),gridCol);

      for(int i=0;i<num_lines  ;i++)
      {
        myRenderer->DrawLine(vec3(-lineLen,0,-i*grid_scale),vec3(lineLen,0,-i*grid_scale),gridCol,gridCol);
        myRenderer->DrawLine(vec3(-lineLen,0,i*grid_scale),vec3(lineLen,0,i*grid_scale),gridCol,gridCol);

        myRenderer->DrawLine(vec3(i*grid_scale,0,-lineLen),vec3(i*grid_scale,0,lineLen),gridCol,gridCol);
        myRenderer->DrawLine(vec3(-i*grid_scale,0,-lineLen),vec3(-i*grid_scale,0,lineLen),gridCol,gridCol);
      }
}


void Draw_XY_point_grid(Renderer* myRenderer ,int num_lines, float grid_scale , vec4 gridCol , vec4 originCol = vec4(1) )
{
    float radius = 1.5;

    for(int i=0;i<num_lines;i++)
    {
        for(int j=0;j<num_lines;j++)
        {
            if(i==0 && j== 0)
            {
                myRenderer->DrawCircle2D(vec2(2*i*grid_scale,2*j*grid_scale) , radius,originCol,1,1);
            }

            else
            {
                 myRenderer->DrawCircle2D(vec2(2*i*grid_scale,2*j*grid_scale) , radius,gridCol,1,1);
                 myRenderer->DrawCircle2D(vec2(2*i*grid_scale,-2*j*grid_scale) , radius,gridCol,1,1);
                 myRenderer->DrawCircle2D(vec2(-2*i*grid_scale,-2*j*grid_scale) , radius,gridCol,1,1);
                 myRenderer->DrawCircle2D(vec2(-2*i*grid_scale,2*j*grid_scale) , radius,gridCol,1,1);
            }
        }


    }



}







