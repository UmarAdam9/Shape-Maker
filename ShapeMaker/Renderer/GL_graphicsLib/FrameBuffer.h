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

#define MAX_COL_ATTACHMENTS 2


class FrameBuffer
{

public:
    //currently i'll have a single frame buffer with 2 color attachment and 1 depth_stencil attachment
    unsigned int FBO_ID;
    //2 color attachment texture
    unsigned int framebufferTexture[MAX_COL_ATTACHMENTS];
    //renderbuffer for depth and stencil attachment
    unsigned int RBO;
    int width;
    int height;


    void setup(int w,int h)
    {
        width  =w;
        height =h;

        //generate a framebuffer
        glGenFramebuffers(1,&FBO_ID);
        glBindFramebuffer(GL_FRAMEBUFFER,FBO_ID);

        //create color attachments
        for(int i=0;i<MAX_COL_ATTACHMENTS;i++)
        {
            //create texture for color attachment
            glGenTextures(1,&framebufferTexture[i]);
            glBindTexture(GL_TEXTURE_2D,framebufferTexture[i]);
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,NULL);
            //set the parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);    //specifies to openGl that it should use nearest neighbor algorithm to handle when texture is minimized.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);    //specifies to openGl that it should use nearest neighbor algorithm to handle when texture is magnified.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);  //specifies to openGl that it should clamp the texture to the edge of the screen.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);  //specifies to openGl that it should clamp the texture to the edge of the screen.
        }
        //Attach the texture to the framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,framebufferTexture[0],0);


        //create RBO for depth and stencil attachment
        glGenRenderbuffers(1,&RBO);
        //Attach RenderBuffer
        glBindRenderbuffer(GL_RENDERBUFFER,RBO);
        glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH24_STENCIL8,width,height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_RENDERBUFFER,RBO); //this attaches the renderbuffer to the frambuffer..?

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR: framebuffer incomplete\n";

        //unbind the framebuffer to close cleanly
         glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void setupBloom(int w, int h)
    {
        width  =w;
        height =h;

        //generate a framebuffer
        glGenFramebuffers(1,&FBO_ID);
        glBindFramebuffer(GL_FRAMEBUFFER,FBO_ID);

        //create color attachments
        for(int i=0;i<MAX_COL_ATTACHMENTS;i++)
        {
            //create texture for color attachment
            glGenTextures(1,&framebufferTexture[i]);
            glBindTexture(GL_TEXTURE_2D,framebufferTexture[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
            //set the parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);    //specifies to openGl that it should use nearest neighbor algorithm to handle when texture is minimized.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);    //specifies to openGl that it should use nearest neighbor algorithm to handle when texture is magnified.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);  //specifies to openGl that it should clamp the texture to the edge of the screen.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);  //specifies to openGl that it should clamp the texture to the edge of the screen.
        }
        //Attach the texture to the framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,framebufferTexture[0],0);


        //create RBO for depth and stencil attachment
        glGenRenderbuffers(1,&RBO);
        //Attach RenderBuffer
        glBindRenderbuffer(GL_RENDERBUFFER,RBO);
        glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH24_STENCIL8,width,height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_RENDERBUFFER,RBO); //this attaches the renderbuffer to the frambuffer..?

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR: framebuffer incomplete\n";

        //unbind the framebuffer to close cleanly
         glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }


    void Bind()
    {
         //Bind the framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER,FBO_ID);
        glViewport(0, 0, width, height);


    }


};
