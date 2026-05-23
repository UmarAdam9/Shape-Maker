#include<iostream>
#include<vector>
#include<GL/glm/glm.hpp>
#include<GL/glm/gtc/matrix_transform.hpp>
#include<GL/glm/gtc/type_ptr.hpp>
#include<GL/glm/gtx/rotate_vector.hpp>
#include<GL/glm/gtx/vector_angle.hpp>


using namespace glm;

struct Collider2D{

std::vector<vec2>localVertices;
std::vector<vec2>transformedVertices;
vec2 position;
vec2 scale;
float angle;
int isActive;

Collider2D(vec2 pos , std::vector<vec2>verts,vec2 s = vec2(1,1),float a=0.0f)
{
    localVertices = verts;
    position=pos;
    transformedVertices.resize(localVertices.size());
    scale = s;
    angle = a;
    isActive = 0;
}


void update_to_World_Space(){

    for(int i=0;i<localVertices.size();i++){


      //(           x           *       Sx*   cos(theta)           -                  y             *      Sy*  sin(theta)         )  +  PosX
       transformedVertices[i].x  =  (localVertices[i].x * scale.x * cos(angle) -   localVertices[i].y    * scale.y*sin(angle)) + position.x;

     // (           x           *       Sx*  sin(theta)           +                  y             *       Sy*  cos(theta)         )  +  PosY

      transformedVertices[i].y  =  (localVertices[i].x * scale.x  * sin(angle) +   localVertices[i].y    *  scale.y*cos(angle)) + position.y;



      }

}



Collider2D(){}
};



bool mouseCheckerCollider(vec2 point, Collider2D frame,float worldScale=1){


for(int i=0;i<frame.transformedVertices.size();i++){

    vec2 p = frame.transformedVertices[i]*worldScale;
    vec2 q = frame.transformedVertices[(i+1) % frame.transformedVertices.size()]*worldScale;

    vec2 refv = q-p;
    refv = glm::normalize(refv);


    if(glm::dot(point,refv) < glm::dot(p,refv)){return false;}
}

return true;
}




