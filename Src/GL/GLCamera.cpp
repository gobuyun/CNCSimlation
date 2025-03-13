#include "GLCamera.h"

#include <GL/glut.h>

GLCamera::GLCamera(KDL::Vector pos, KDL::Vector rpy):invert_factor(-1)
{
    look_frame.M = KDL::Rotation::RPY(rpy.x()*KDL::deg2rad,rpy.y()*KDL::deg2rad,rpy.z()*KDL::deg2rad);
    look_frame.p = pos;
    UpdateLookAt();
}

void GLCamera::MoveX(double delta, bool is_abs)
{
    KDL::Vector pos = look_frame.p;
    if(is_abs)
        pos(0) = delta * invert_factor;
    else
        pos(0) += delta * invert_factor;
    look_frame.p = pos;
    UpdateLookAt();
}

void GLCamera::MoveY(double delta, bool is_abs)
{
    KDL::Vector pos = look_frame.p;
    if(is_abs)
        pos(1) = delta * invert_factor;
    else
        pos(1) += delta * invert_factor;
    look_frame.p = pos;
    UpdateLookAt();
}

void GLCamera::MoveZ(double delta, bool is_abs)
{
    KDL::Vector pos = look_frame.p;
    if(is_abs)
        pos(2) = delta * invert_factor;
    else
        pos(2) += delta * invert_factor;
    look_frame.p = pos;
    UpdateLookAt();
}

void GLCamera::RotX(double delta, bool is_abs)
{
    RotAnyAxis(delta, KDL::Vector(1,0,0),is_abs);
    UpdateLookAt();
}

void GLCamera::RotY(double delta, bool is_abs)
{
    RotAnyAxis(delta, KDL::Vector(0,1,0),is_abs);
    UpdateLookAt(); 
}

void GLCamera::RotZ(double delta, bool is_abs)
{
    RotAnyAxis(delta, KDL::Vector(0,0,1),is_abs);
    UpdateLookAt(); 
}

void GLCamera::RotAnyAxis(double delta, KDL::Vector ax,bool is_abs)
{
    double r,p,y;
    look_frame.M.GetRPY(r,p,y);
    double angle = 0.0;
    if(is_abs)
        angle = (delta+ r) * invert_factor ;
    else
        angle = (r) * invert_factor ;
    look_frame.p = KDL::Rotation::Rot2(ax,angle * KDL::deg2rad) * look_frame.p;

    UpdateLookAt(); 
}


void GLCamera::UpdateLookAt()
{
    double le = look_frame.p.Norm();
    gluLookAt(look_frame.p.x(),look_frame.p.y(),look_frame.p.z(),
              look_frame.p.x() + look_frame.M.UnitZ().x(),look_frame.p.y() + look_frame.M.UnitZ().y(),look_frame.p.z()+look_frame.M.UnitZ().z(),
              look_frame.M.UnitY().x(),
              look_frame.M.UnitY().y(),
              look_frame.M.UnitY().z()
              );
}
