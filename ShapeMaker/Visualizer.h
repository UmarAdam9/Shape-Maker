#include "imGui_Docker/imgui.h"
#include "imGui_Docker/imgui_impl_glfw.h"
#include "imGui_Docker/imgui_impl_opengl3.h"

#include <iostream>
#include "Renderer/Renderer.h"  //includes glfw,glad,math.h,glm even iostream





struct Canvas
{
    //==================
    //Ids , flags
    //==================
    int m_id;
    int m_FBO_id;
    bool m_isActive;
    bool m_isBound;
    camera m_camera;
    int m_width;
    int m_height;

    //===================
    //Grid specific stuff
    //===================
    float m_gridScale;
    vec4 m_bgCol;
    float m_lineLen; //for grid create function
    vec4 m_gridCol;
    vec4 m_xCol;
    vec4 m_yCol;
    shader m_bg_shader;
    float m_funcScale;
    float m_XY_spacing;
    //==================
    //Mouse variables
    //==================
    vec2 m_CanvasCorner;
    vec2 m_mouseWorldCoord;

    //=====================================
    //Arrays for ImGui sliders and elements
    //=====================================
    vector<int*>  m_intPtrs;
    vector<float*>m_floatPtrs;

    vector< vector<char> >m_intNamePtrs;
    vector< vector<char> >m_floatNamePtrs;

    vector<int>  m_minMaxInt;
    vector<float>m_minMaxFloat;

    //===================
    //Methods
    //===================
    Canvas(){}

    Canvas(int id ,int width , int height ,vec4 bgCol= vec4(0,0.8,0.1,1) , vec4 gridCol= vec4(1,1,1,0.4), int scale = 1, float lineLen = 100  , vec4 xCol = vec4(0,1,1,1), vec4 yCol = vec4(0,1,1,1) )
    {
        m_id = id;
        m_bg_shader = shader("Renderer/Renderer_Shaders/VertexShader_PostProcessing.txt","Renderer/Renderer_Shaders/FragmentShader_bg.txt");
        m_camera = camera(m_width,m_height,glm::vec3(8.0f,3.0f,200.0f));
        m_XY_spacing = 3;
        m_lineLen = lineLen;
        m_bgCol = bgCol;
        m_width = width;
        m_height = height;
        m_gridCol = gridCol;
        m_xCol = xCol;
        m_yCol = yCol;
        m_funcScale = scale;
        m_gridScale = 1;
    }


    void m_bindCanvas(Renderer &myRenderer , int drawOptions = 1 , int RenderShader = 1)
    {
        myRenderer.FBO[m_id].Bind();
        myRenderer.clear_bg(m_bgCol);

        //get mouse coordinates
        vec2 mouseCircle = vec2(  ImGui::GetMousePos().x -  m_CanvasCorner.x - 8 ,  (ImGui::GetMousePos().y - m_CanvasCorner.y -26));
        vec4 mouseCircle_v4 =  m_camera.inverseProj * vec4( (mouseCircle.x/(float)m_width - 0.5f) * 2.0f , ( (m_height-mouseCircle.y)/(float)m_height - 0.5) * 2.0f  , -1 ,1) ;
        m_mouseWorldCoord  = vec2(mouseCircle_v4.x ,mouseCircle_v4.y);

        if(RenderShader)
        {
            //instead of clearing the screen draw the gradient image
            m_bg_shader.Activate();
            glUniform4f(glGetUniformLocation(m_bg_shader.ID, "bgColor"), m_bgCol.x,m_bgCol.y,m_bgCol.z,m_bgCol.a);
            if(glGetUniformLocation(m_bg_shader.ID, "bgColor") == -1) std::cout << "Uniform not found"<<std::endl;
            myRenderer.RenderPostProcessingFrame(m_id, m_id, m_bg_shader );//Post-process render on Frame buffer 0 with framebuffer 0 as input

        }


        switch(drawOptions)
        {
            case 0:
                /* draw nothing*/
                break;
            case 1:
                  Draw_XY_grid( &myRenderer ,60,m_lineLen,m_XY_spacing*m_gridScale,m_gridCol,m_xCol,m_yCol);
                 break;
            case 2:
                  Draw_XY_point_grid( &myRenderer ,15,m_XY_spacing*m_gridScale,m_gridCol);
                 break;
        }

    }

    void m_RenderCanvas(Renderer &myRenderer)
    {
        myRenderer.Render(m_camera ,1 ,20,m_funcScale);

        char headerString[20];
        sprintf(headerString,"Draw Space %d",m_id);

        //render ImGui
       ImGui::Begin(headerString);

          if(ImGui::IsWindowFocused())
          {
              m_isActive = 1;
          }
          else
          {
              m_isActive = 0;
          }

          m_CanvasCorner.x = ImGui::GetWindowPos().x ;
          m_CanvasCorner.y = ImGui::GetWindowPos().y ;


          ImGui::GetWindowDrawList()->AddImage
          ((void *)myRenderer.FBO[m_id].framebufferTexture[0],
           ImVec2(ImGui::GetCursorScreenPos()),
           ImVec2(ImGui::GetCursorScreenPos().x + m_width,
                  ImGui::GetCursorScreenPos().y + m_height),
           ImVec2(0, 1),
           ImVec2(1, 0)
           );
      ImGui::End();


        sprintf(headerString,"Controls %d",m_id);

        ImGui::Begin(headerString);
        if (ImGui::CollapsingHeader("graph Controls"))
         {
            ImGui::ColorEdit4("Background Col",(float*)&m_bgCol,ImGuiColorEditFlags_DefaultOptions_|ImGuiColorEditFlags_AlphaBar);
            ImGui::ColorEdit4("Grid Col", (float*)&m_gridCol,ImGuiColorEditFlags_DefaultOptions_|ImGuiColorEditFlags_AlphaBar);
            ImGui::ColorEdit4("X-Col", (float*)&m_xCol,ImGuiColorEditFlags_DefaultOptions_|ImGuiColorEditFlags_AlphaBar);
            ImGui::ColorEdit4("Y-Col", (float*)&m_yCol,ImGuiColorEditFlags_DefaultOptions_|ImGuiColorEditFlags_AlphaBar);


            //render graph related controls
            for(int i=0;i<m_intPtrs.size();i++)
            {
                ImGui::SliderInt((const char*)m_intNamePtrs[i].data(),m_intPtrs[i],m_minMaxInt[2*i],m_minMaxInt[(2*i) + 1]);
            }
            for(int i=0;i<m_floatPtrs.size();i++)
            {
                ImGui::SliderFloat((const char*)m_floatNamePtrs[i].data(),m_floatPtrs[i],m_minMaxFloat[2*i],m_minMaxFloat[(2*i) + 1]);
            }
         }
        ImGui::End();
    }


    void m_addIntSlider(int* val , char* name, int minVal , int maxVal)
    {
        m_intPtrs.push_back(val);
        m_intNamePtrs.emplace_back(name, name + strlen(name)); //does not give null terminater
        m_minMaxInt.push_back(minVal);
        m_minMaxInt.push_back(maxVal);
    }

    void m_addFloatSlider(float* val , char* name, float minVal , float maxVal )
    {
        m_floatPtrs.push_back(val);
        m_floatNamePtrs.emplace_back(name, name + strlen(name)); //does not give null terminater
        m_minMaxFloat.push_back(minVal);
        m_minMaxFloat.push_back(maxVal);
    }
};

struct Visualizer
{
    //==================
    //windowing stuff
    //==================
    GLFWwindow *m_window;
    int m_windowWidth;
    int m_windowHeight;
    char m_AppNAme[20];
    vector<Canvas>m_canvas;



    //==================
    //ImGUI stuff
    //==================
    //ImGuiIO& m_io;


    //==================
    //Timing variables?
    //==================



    //==================
    //Scroll Callback variables
    //==================
    float* m_gridScalePtr;
    float* m_funcScalePtr;



    //==================
    //Methods
    //==================
    static void m_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        // Handle scroll input here
        // yoffset will be positive for scrolling up, negative for scrolling down
        // xoffset will be for horizontal scrolling, if supported by the device
//        m_gridScalePtr = nullptr;
//        m_funcScalePtr = nullptr;

        auto* self = (Visualizer*)glfwGetWindowUserPointer(window);
        if (!self) return;


//        cout<<self->m_gridScalePtr<<endl;
//        cout<<self->m_funcScalePtr<<endl;
//        cout<<endl;

        if( self->m_gridScalePtr == nullptr ||  self->m_funcScalePtr == nullptr)
            return;

//        cout<<"after nullptr check"<<endl;
//        cout<<endl;

        if(yoffset > 0) //forward
        {
            //The Grid=========================
            if(*self->m_gridScalePtr > 5)
            {
                *self->m_gridScalePtr = 1;
                //update scale text
            }
            else
            {
                *self->m_gridScalePtr *= 1.2;
            }
            //The Function=====================
             *self->m_funcScalePtr *= 1.2;
        }

        else if(yoffset<0)
        {
            //The grid=======================
            *self->m_gridScalePtr /= 1.2;
            if(*self->m_gridScalePtr < 1)
            {
                *self->m_gridScalePtr = 5;
                //update scale text
            }
            //The function===================
            *self->m_funcScalePtr /= 1.2;
        }
    }


    void m_init_GLFW_IMGUI_GLAD() //inits glfw , glad and ImGui
    {
        //initialize glfw
        glfwInit();
       //Tell GLFW which version we are using and compatibility (core)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
        glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
        //Create a window by using a GLFwindow object
        m_window =glfwCreateWindow(m_windowWidth,m_windowHeight,"Visualizer-v3",NULL,NULL);
        //store the object in glfw to retrieve in scroll callback
        glfwSetWindowUserPointer(m_window, this);
         //Make the created window the current window
        glfwMakeContextCurrent(m_window);
        //Load glad to configure OpenGL
        gladLoadGL();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& m_io = ImGui::GetIO(); (void)m_io;
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(m_window,true);
        ImGui_ImplOpenGL3_Init("#version 330");

        glfwSetScrollCallback(m_window, m_scroll_callback);
    }

    Visualizer( int width = 800 , int height = 800)
    {
        m_windowWidth = width;
        m_windowHeight = height;

        m_gridScalePtr = nullptr;
        m_funcScalePtr = nullptr;

        m_init_GLFW_IMGUI_GLAD();
    }
    void m_init_canvas(int numCanvas,int width , int height)
    {


    }
    void m_startFrame()
    {
        //init ImGui new frame=============
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // Create a dockspace in main viewport.
        ImGui::DockSpaceOverViewport();
        //================================

        //detach the callback variable pointers
        m_gridScalePtr = nullptr;
        m_funcScalePtr = nullptr;

//        cout<<m_gridScalePtr<<endl;
//        cout<<m_funcScalePtr<<endl;

    }

    void m_finishFrame(Renderer& myRenderer)
    {
        // -------------------Bind the default framebuffer and draw the textured rectangle over the screen-------------------
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_windowWidth, m_windowHeight);
        myRenderer.clear_bg(vec4(0.0));

        //call ImGui Render
         ImGui::Render();
         ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        //Swap the Buffers
         glfwSwapBuffers(m_window);
        //Take care of events
         glfwPollEvents();


    }

    void m_destroyViz() //should I rather create a destructor??
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }



};


void AssignScrollCallbackVariables(Visualizer &Viz , Canvas &Can)
{
    Viz.m_gridScalePtr = &Can.m_gridScale;
    Viz.m_funcScalePtr = &Can.m_funcScale;

}
