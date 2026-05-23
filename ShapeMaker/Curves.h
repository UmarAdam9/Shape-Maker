#include<iostream>
#include<vector>
#include<GL/glm/glm.hpp>
#include<GL/glm/gtc/matrix_transform.hpp>
#include<GL/glm/gtc/type_ptr.hpp>
#include<GL/glm/gtx/rotate_vector.hpp>
#include<GL/glm/gtx/vector_angle.hpp>




vec2 Lerp(vec2 p0,vec2 p1,float t)
{
    //p1 = p0 + t(p1 - p0)

    return p0 + t*(p1-p0);

}


vec2 GetBezierPoint_cubic(vec2 p1 , vec2 p2, vec2 p3,vec2 p4 , float t)
{

    vec2 firstLerp  = Lerp(p1,p2,t);
    vec2 secondLerp = Lerp(p2,p3,t);
    vec2 thirdLerp  = Lerp(p3,p4,t);

    vec2 fourthLerp = Lerp(firstLerp,secondLerp,t);
    vec2 fifthLerp  = Lerp(secondLerp,thirdLerp,t);

    return Lerp(fourthLerp,fifthLerp,t);



}
