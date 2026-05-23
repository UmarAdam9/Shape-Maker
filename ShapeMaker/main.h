#include "Application/Application.h"
#include "Visualizer.h" //includes Renderer.h ,glfw,glad,math.h,glm even iostream
#include "Collider2D.h"
#include "Curves.h"

#include <chrono>
using namespace glm;
using namespace std;

using namespace std::chrono;


/** TODO list
    1. slider name glitch
    2. pointChecker not working with triangles
    3. mousescroll callback [DONE]
    4. A func scale and grid scale variable that is attached automatically [DONE]
    5. seperate variables for mouse drawing position , mouse collision checking position and collider to mouse snapping position
    6. safety measures for canvas binding
    7. HDR framebuffer for bloom
    8. Animation System
    9. 3D primitives calculation and Rendering
    10.Text rendering
**/

void DrawCollider2D(Collider2D shape,vec3 col,float thickness, Renderer& myRenderer , float worldScale )
{
    for(int i=0;i<shape.transformedVertices.size();i++)
    {
        myRenderer.DrawLine2D(shape.transformedVertices[i], shape.transformedVertices[(i+1) % shape.transformedVertices.size()] ,col,thickness , worldScale);
    }
}

bool point_checker_circle(vec2 point ,vec2 circle_pos ,float circle_radius){

    if(glm::length(circle_pos - point) < circle_radius)
        return true;
    else
        return false;
}

enum POINT_TYPE{

    TYPE_LINE,
    TYPE_BEZIER,
    TYPE_BEZIER_CONTROL

};



struct Line
{
	int p1;
	int p2;

	Line(int a, int b)
	{
	    p1 =a;
	    p2 = b;
	}
};

struct Bezier
{
    float numPoints;
    int controlPointIndexes[4];
	vector<int>bezierIndexes;
	Bezier(float num = 20){numPoints = num;};
};

enum MODE
{
	MODE_LINE ,
	MODE_BEZIER,
	MODE_SHAPE_SELECT
};


struct Point
{

	vec2 position;
	float radius;
	POINT_TYPE type;
    int ctrlPointNum;
    int parentIndex;

	Point()
	{
	    radius = 1;
	}

	Point(vec2 pos)
	{
	    position = pos;
	}

	Point(vec2 pos, POINT_TYPE t ,int parentIdx ,int ctrlNum =0)
	{
	    position = pos;
	    type = t;
	    ctrlPointNum =ctrlNum;
	    parentIndex = parentIdx;
	}
};


bool pointCheckerPolygon(vec2 point, vector<vec2>frame){


    for(int i=0;i<frame.size();i++){

        vec2 p = frame[i];
        vec2 q = frame[(i+1) % frame.size()];

        vec2 refv = q-p;
        refv = glm::normalize(refv);


        if(glm::dot(point,refv) < glm::dot(p,refv)){return false;}
    }

return true;
}

