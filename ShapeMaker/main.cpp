#include "main.h"


/************************************************************************
Boilerplate for using Visualizer - v1.0
The Application has a default IP of 192.168.100.20 and an open port 4992
By Default the application creates a canvas and renders udp data stream coming
from a server with IP 192.168.100.20 and port 4991
*************************************************************************/


/**
TODO:

1.Rectangular selection of multiple points/shapes [DONE]
2.Deletion of points
3.Merging of points
4.Circle
5.Intersections
6.Duplicate
7.Scale
8.Rotate

**/


struct sRenderingApplication : public sApplication
{


  void m_RenderFunction() override
    {
        //Visualizer Variables=========================
        Visualizer myViz(1024,800);
        vector<Canvas>vecCan{ Canvas(1, 1024 , 800,vec4(0.07,0.13,0.17,1.0),vec4(0,1,1,0.2))};
        Renderer myRenderer(0,1,1,1,1,1);
        myRenderer.init_renderer(5120000,1024,800);
        long long totalTime = 0;
        int frameCounter = 0;

        //Application Variables=========================
        int pointNum = 0;

        vector<Point>pointArr;
        vector<Line>vecLines;
        vector<Bezier>vecBezier;

        int prevPointIndex;
        int prevPrevPointIndex;
        Point* dragPoint = nullptr;

        MODE mode = MODE_LINE;
        bool isPressed  = 0;
        bool wasPressed = 0;

        bool mouseClicked = 0;
        bool mouseHeld = 0;
        bool mouseUp = 0;

        bool shapeSelected = 0;

        int selectShapeIndex = -1;
        vec2 mousePosBefore;
        vec2 mouseOffset;

        vector<vec2>pointPosBefore;
        vector<int>pointIndexes;

        bool drawControlPoints = true;
        bool createBoundingBox = false;
        vector<vec2>boundingBoxVerts;
        boundingBoxVerts.resize(4);
        int boundingBoxLen = 0;


        //main while Loop================================
        while(!glfwWindowShouldClose(myViz.m_window))
        {
            //=================
            //Timing
            //==================
             auto start = high_resolution_clock::now();

            //*********Application Logic************************************************

            if( GetAsyncKeyState('G') )
            {
                mode = MODE_LINE;
            }
            if( GetAsyncKeyState('H') )
            {
                mode = MODE_BEZIER;
            }
            if( GetAsyncKeyState('J'))
            {
                mode = MODE_SHAPE_SELECT;
            }

            if(GetAsyncKeyState('A'))
            {
                drawControlPoints = !drawControlPoints;
            }

            if(vecCan[0].m_isActive)
            {

            //figure out mouse held or mouse clicked
            //=====================================
                if( GetAsyncKeyState(VK_LBUTTON) )
                {
                    wasPressed = isPressed;
                    isPressed = 1;
                }
                else
                {
                    wasPressed = isPressed;
                    isPressed = 0;
                }

                mouseClicked = isPressed && !wasPressed;
                mouseHeld = isPressed && wasPressed;
                mouseUp = !isPressed && wasPressed;
            //=======================================



                if(mouseHeld)
                {
                    if(shapeSelected)
                    {
                        if(dragPoint != nullptr)
                        {
                            //if point was a line point -> simply set it to mousePos
                            dragPoint->position = vecCan[0].m_mouseWorldCoord/vecCan[0].m_funcScale;

                            //if point was a bezier control point -> then recalculate the bezier curve points
                            if( dragPoint->type == TYPE_BEZIER_CONTROL){
                                //get its parent
                                int numPoints = vecBezier[ dragPoint->parentIndex].numPoints;
                                vec2 p1 = pointArr[vecBezier[ dragPoint->parentIndex].controlPointIndexes[0]].position;
                                vec2 p2 = pointArr[vecBezier[ dragPoint->parentIndex].controlPointIndexes[1]].position;
                                vec2 p3 = pointArr[vecBezier[ dragPoint->parentIndex].controlPointIndexes[2]].position;
                                vec2 p4 = pointArr[vecBezier[ dragPoint->parentIndex].controlPointIndexes[3]].position;

                                //get offset to first curve point
                                int offset = vecBezier[ dragPoint->parentIndex].controlPointIndexes[0] + 4 ;

                                float t = 0.0f;
                                for(int j=0;j<numPoints;j++)
                                {
                                    pointArr[offset + j].position = GetBezierPoint_cubic(p1,p2,p3,p4,t);
                                    t+=1.0/numPoints;
                                }
                            }
                        }

                    }

                    else if(createBoundingBox)
                    {

                        vec2 currentMousePos = vecCan[0].m_mouseWorldCoord/vecCan[0].m_funcScale;
                        cout<<"Bounding Box length is "<<glm::length(currentMousePos - mousePosBefore)<<endl;
                        boundingBoxLen = glm::length(currentMousePos - mousePosBefore);
                        boundingBoxVerts[0] = (mousePosBefore);
                        boundingBoxVerts[1]=(vec2(currentMousePos.x, mousePosBefore.y) );
                        boundingBoxVerts[2]=(currentMousePos);
                        boundingBoxVerts[3]=(vec2(mousePosBefore.x,currentMousePos.y));


                    }

                }

                else if(mouseClicked)
                {
                    dragPoint = nullptr;
                    shapeSelected = false;
                    selectShapeIndex = -1;
                    pointPosBefore.clear();
                    pointIndexes.clear();
                  //  boundingBoxVerts.clear();

                    //check collision with all the points
                    for(int i=0;i< pointArr.size();i++)
                    {
                        if(point_checker_circle(vecCan[0].m_mouseWorldCoord/vecCan[0].m_funcScale,pointArr[i].position,1.5))
                        {
                            if(mode  == MODE_SHAPE_SELECT) //in shape select mode -> dragShapeIndex is set
                            {
                                selectShapeIndex = i;
                                shapeSelected = true;
                                mousePosBefore = vecCan[0].m_mouseWorldCoord/vecCan[0].m_funcScale;
                                mouseOffset = vec2(0);
                                int idx= pointArr[selectShapeIndex].parentIndex;

                                if(pointArr[selectShapeIndex].type == TYPE_BEZIER_CONTROL || pointArr[selectShapeIndex].type == TYPE_BEZIER )
                                {
                                    for(int i=0;i<4;i++)
                                    {
                                        pointPosBefore.push_back(pointArr[vecBezier[idx].controlPointIndexes[i]].position);
                                        pointIndexes.push_back(vecBezier[idx].controlPointIndexes[i]);
                                    }

                                    for(int i=0;i<vecBezier[idx].bezierIndexes.size();i++)
                                    {
                                        pointPosBefore.push_back(pointArr[vecBezier[idx].bezierIndexes[i]].position );
                                        pointIndexes.push_back(vecBezier[idx].bezierIndexes[i]);
                                    }

                                }

                                else if(pointArr[selectShapeIndex].type == TYPE_LINE)
                                {
                                    pointPosBefore.push_back( pointArr[vecLines[idx].p1].position );
                                    pointIndexes.push_back(vecLines[idx].p1);
                                    pointPosBefore.push_back( pointArr[vecLines[idx].p2].position );
                                    pointIndexes.push_back(vecLines[idx].p2);
                                }

                                break;
                            }

                            else    //in any other mode -> individual point is selected
                            {
                                 //line points and bezier control points can be manipulated directly
                                if(pointArr[i].type == TYPE_LINE || pointArr[i].type == TYPE_BEZIER_CONTROL )
                                {
                                    dragPoint = &pointArr[i];
                                    shapeSelected = true;
                                    break;
                                }
                            }
                        }

                    }


                    if(!shapeSelected) //shapeSelected is false
                    {
                        if(mode == MODE_SHAPE_SELECT)
                        {
                            /** create a rectangular bounding box  **/
                            //get current mouse  position
                            mousePosBefore = vecCan[0].m_mouseWorldCoord/vecCan[0].m_funcScale;
                            //set flag to true
                            createBoundingBox = true;
                            cout<<"creating bounding box"<<endl;
                         }

                        if(mode == MODE_LINE)
                        {
                            pointArr.push_back( Point( vecCan[0].m_mouseWorldCoord/vecCan[0].m_funcScale , TYPE_LINE ,vecLines.size()) );
                            pointArr.push_back( Point( (vecCan[0].m_mouseWorldCoord+vec2(10,10))/vecCan[0].m_funcScale , TYPE_LINE ,vecLines.size()) );

                            prevPointIndex = pointArr.size() - 1 ;
                            prevPrevPointIndex = pointArr.size() - 2;

                            vecLines.push_back( Line (prevPrevPointIndex , prevPointIndex  ) );
                        }

                        if(mode == MODE_BEZIER)
                        {
                            vecBezier.push_back(Bezier());

                            pointArr.push_back( Point( vecCan[0].m_mouseWorldCoord/vecCan[0].m_funcScale , TYPE_BEZIER_CONTROL,vecBezier.size() -1,0 ) );
                            vecBezier[vecBezier.size() - 1].controlPointIndexes[0]=pointArr.size() - 1 ;

                            pointArr.push_back( Point( (vecCan[0].m_mouseWorldCoord+vec2(5,5))/vecCan[0].m_funcScale , TYPE_BEZIER_CONTROL,vecBezier.size()-1,1 ) );
                            vecBezier[vecBezier.size() - 1].controlPointIndexes[1]=pointArr.size() - 1 ;

                            pointArr.push_back( Point( (vecCan[0].m_mouseWorldCoord+vec2(10,5))/vecCan[0].m_funcScale , TYPE_BEZIER_CONTROL,vecBezier.size()-1,2 ) );
                            vecBezier[vecBezier.size() - 1].controlPointIndexes[2]=pointArr.size() - 1 ;

                            pointArr.push_back( Point( (vecCan[0].m_mouseWorldCoord+vec2(15,0))/vecCan[0].m_funcScale , TYPE_BEZIER_CONTROL,vecBezier.size()-1,3 ) );
                            vecBezier[vecBezier.size() - 1].controlPointIndexes[3]=pointArr.size() - 1 ;


                            vec2 p1 = pointArr[pointArr.size()-4].position;
                            vec2 p2 = pointArr[pointArr.size()-3].position;
                            vec2 p3 = pointArr[pointArr.size()-2].position;
                            vec2 p4 = pointArr[pointArr.size()-1].position;

                            for(float t =0.0f; t <= 1.0f ; t+= (1.0/vecBezier[vecBezier.size() - 1].numPoints))
                            {
                                 pointArr.push_back ( Point( GetBezierPoint_cubic(p1, p2, p3, p4, t) ,TYPE_BEZIER, vecBezier.size()-1));
                                 vecBezier[vecBezier.size() - 1].bezierIndexes.push_back(pointArr.size() - 1 );
                            }


                        }

                    }

                }

                else if(mouseUp)
                {

                    if(createBoundingBox)
                    {
                        pointPosBefore.clear();
                        pointIndexes.clear();

                        createBoundingBox = false;
                        cout<<"destroying bounding box"<<endl;

                        if(boundingBoxLen > 0.00001)
                        {
                            //checkif there are any points contained inside the boundingBox
                            int k  =0;
                            while( k<pointArr.size() )
                            {
                                if(pointCheckerPolygon(pointArr[k].position,boundingBoxVerts))
                                {
                                    cout<<"here"<<endl;
                                    selectShapeIndex = k;
                                    shapeSelected = true;
                                    mousePosBefore = vecCan[0].m_mouseWorldCoord/vecCan[0].m_funcScale;
                                    mouseOffset = vec2(0);
                                    int idx= pointArr[selectShapeIndex].parentIndex;

                                    if(pointArr[selectShapeIndex].type == TYPE_BEZIER_CONTROL || pointArr[selectShapeIndex].type == TYPE_BEZIER )
                                    {
                                        for(int i=0;i<4;i++)
                                        {
                                            pointPosBefore.push_back(pointArr[vecBezier[idx].controlPointIndexes[i]].position);
                                            pointIndexes.push_back(vecBezier[idx].controlPointIndexes[i]);
                                        }

                                        for(int i=0;i<vecBezier[idx].bezierIndexes.size();i++)
                                        {
                                            pointPosBefore.push_back(pointArr[vecBezier[idx].bezierIndexes[i]].position );
                                            pointIndexes.push_back(vecBezier[idx].bezierIndexes[i]);
                                        }

                                        k = vecBezier[idx].bezierIndexes[ vecBezier[idx].bezierIndexes.size()-1 ] ;
                                    }

                                    else if(pointArr[selectShapeIndex].type == TYPE_LINE)
                                    {
                                        cout<<"here again"<<endl;
                                        pointPosBefore.push_back( pointArr[vecLines[idx].p1].position );
                                        pointIndexes.push_back(vecLines[idx].p1);
                                        pointPosBefore.push_back( pointArr[vecLines[idx].p2].position );
                                        pointIndexes.push_back(vecLines[idx].p2);

                                        k = vecLines[idx].p2;
                                        cout<<"value of k is "<< k <<endl;
                                        cout<<endl;
                                    }

                                }

                                k++;
                            }
                            mousePosBefore = boundingBoxVerts[0] - vec2( (boundingBoxVerts[0].x - boundingBoxVerts[1].x)/2 , (boundingBoxVerts[0].y - boundingBoxVerts[3].y)/2 );

                        }
                        for(int i=0; i< boundingBoxVerts.size();i++)
                            boundingBoxVerts[i] = vec2(0,0);
                    }

                }

                else
                {

                    if(shapeSelected)
                    {
                        if( GetAsyncKeyState('D') )
                        {
                            //delete shape
                        }

                        else if(GetAsyncKeyState('S'))
                        {
                             //scale shape
                        }

                        else if(GetAsyncKeyState('R'))
                        {
                             //rotate shape
                        }

                       else if(GetAsyncKeyState('F'))
                        {
                             //move shape
                             vec2 mouseOffset = (vecCan[0].m_mouseWorldCoord/vecCan[0].m_funcScale) - mousePosBefore;

                             for(int i=0;i<pointIndexes.size();i++)
                             {
                                 pointArr[pointIndexes[i]].position = pointPosBefore[i] + mouseOffset;

                             }

                        }

                    }
                }


            }


            //*********Draw everything************************************************
            //==================
            //start viz
            //==================
            myViz.m_startFrame();
            //==================

             vecCan[0].m_bindCanvas(myRenderer,1,0); //updates mouse position as well

                if(vecCan[0].m_isActive)
                    AssignScrollCallbackVariables(myViz,vecCan[0]);


                //draw mouse not in world space but rather translated and at a constant scale
                if( vecCan[0].m_isActive)
                  mode==MODE_LINE ? myRenderer.DrawCircle2D(vec2( vecCan[0].m_mouseWorldCoord.x, vecCan[0].m_mouseWorldCoord.y),2,vec3(1,0,0),0.3,1) : myRenderer.DrawCircle2D(vec2( vecCan[0].m_mouseWorldCoord.x, vecCan[0].m_mouseWorldCoord.y),2,vec3(0,0,1),0.3,1);

                if(!pointArr.empty())
                {
                    for(int i=0; i < pointArr.size() ;i++)
                    {
                        if(pointArr[i].type == TYPE_LINE)
                        {
                            myRenderer.DrawCircle2D(pointArr[i].position,2,vec3(0,1,1),1,vecCan[0].m_funcScale);
                            myRenderer.DrawCircle2D(pointArr[i].position,2,vec3(1,1,1),0.3,vecCan[0].m_funcScale);
                        }

                        if(pointArr[i].type == TYPE_BEZIER)
                        {
                            myRenderer.DrawCircle2D(pointArr[i].position,0.5,vec3(1,1,1),1,vecCan[0].m_funcScale);
                        }

                        if(pointArr[i].type == TYPE_BEZIER_CONTROL)
                        {

                           myRenderer.DrawLine2D(pointArr[i].position + vec2(-1,-1) ,pointArr[i].position + vec2(-1,1) ,vec3(1,1,0),0.05,vecCan[0].m_funcScale);
                           myRenderer.DrawLine2D(pointArr[i].position + vec2(-1,1) ,pointArr[i].position + vec2(1,1) ,vec3(1,1,0),0.05,vecCan[0].m_funcScale);
                           myRenderer.DrawLine2D(pointArr[i].position + vec2(1,1) ,pointArr[i].position + vec2(1,-1) ,vec3(1,1,0),0.05,vecCan[0].m_funcScale);
                           myRenderer.DrawLine2D(pointArr[i].position + vec2(1,-1) ,pointArr[i].position + vec2(-1,-1) ,vec3(1,1,0),0.05,vecCan[0].m_funcScale);

                        }

                        if(&pointArr[i] == dragPoint)
                        {
                           myRenderer.DrawCircle2D(pointArr[i].position,2,vec3(1,0,0),1,vecCan[0].m_funcScale);
                        }
                    }
                }

                if(!vecLines.empty())
                {
                    for(int i=0; i < vecLines.size() ;i++)
                    {
                        myRenderer.DrawLine2D(pointArr[vecLines[i].p1].position,pointArr[vecLines[i].p2].position,vec3(0,1,1),0.1,vecCan[0].m_funcScale);

                    }
                }

                if(!vecBezier.empty())
                {
                    for(int i=0; i < vecBezier.size() ;i++)
                    {

                       myRenderer.DrawLine2D( pointArr[ vecBezier[i].controlPointIndexes[0] ].position,pointArr[vecBezier[i].controlPointIndexes[1]].position,vec3(1,1,0),0.09,vecCan[0].m_funcScale);
                       myRenderer.DrawLine2D( pointArr[vecBezier[i].controlPointIndexes[2]].position,pointArr[vecBezier[i].controlPointIndexes[3]].position,vec3(1,1,0),0.09,vecCan[0].m_funcScale);

                        for(int j=0;j<vecBezier[i].bezierIndexes.size() -1; j++)
                        {
                           myRenderer.DrawLine2D(pointArr[vecBezier[i].bezierIndexes[j]].position,pointArr[vecBezier[i].bezierIndexes[j+1]].position,vec3(1,1,1),0.05,vecCan[0].m_funcScale);

                        }

                    }
                }

                if(shapeSelected)
                {
                    if(selectShapeIndex >= 0)
                    {
                        for(int i=0; i<pointIndexes.size(); i++)
                        {
                            if(pointArr[pointIndexes[i]].type == TYPE_LINE || pointArr[pointIndexes[i]].type == TYPE_BEZIER_CONTROL)
                                myRenderer.DrawCircle2D(pointArr[pointIndexes[i]].position,2,vec3(1,0,0),1,vecCan[0].m_funcScale);

//                            if(i != pointIndexes.size()-1)
//                                myRenderer.DrawLine2D(pointArr[pointIndexes[i]].position,pointArr[pointIndexes[i+1]].position,vec3(1,0,0),0.05,vecCan[0].m_funcScale);
                        }
                    }

                }


                if(createBoundingBox)
                {
                    for(int i=0;i<boundingBoxVerts.size();i++)
                    {
                        myRenderer.DrawLine2D(boundingBoxVerts[i],boundingBoxVerts[(i+1)%boundingBoxVerts.size()],vec3(0,1,0),0.05,vecCan[0].m_funcScale);
                    }

                }


                myRenderer.DrawCircle2D(mousePosBefore,2,vec3(1,1,0),1,vecCan[0].m_funcScale);




//                if(isDragging && dragShapeIndex >= 0 )
//                     myRenderer.DrawLine2D(mousePosBefore,mousePosBefore + mouseOffset,vec3(1,1,0),0.03,vecCan[0].m_funcScale);



             vecCan[0].m_RenderCanvas(myRenderer);


            //=================
            //Render Viz
            //=================
            myViz.m_finishFrame(myRenderer);
            //=================


            //=================
            //Timing
            //=================
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end - start);

            totalTime += duration.count();
            frameCounter++;

            if(frameCounter == 60)
            {
                cout << "Avg frame time (60 frames): "
                     << (totalTime / 60.0) << " us\n";
                cout <<pointArr.size()<<" "<<endl;

                frameCounter = 0;
                totalTime = 0;
            }
            //=================

        }
        myViz.m_destroyViz();
    }

};

sRenderingApplication myApp;

int main()
{
    char appIP[] = "192.168.100.20";
    char serverIP[] = "192.168.100.20";
    vector<int>portArr = {4992,4991};
    vector<char*>serverIPArr = {serverIP};
    if( myApp.m_initApp(1,appIP,portArr ,serverIPArr ,2) == 0)
    {
        cout<<"App initialized successfully!"<<endl;
        //myApp.m_startApp();
    }
    else
        cout<<"App initialization failed"<<endl;


    myApp.m_startApp();
    return 0;
}

