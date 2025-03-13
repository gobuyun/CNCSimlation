#pragma once


#include "../Math/kdl/kinfam_io.hpp"
#include "../Math/kdl/frames_io.hpp"


class GLCamera
{
public:
    //setup lookat
    //1.pos 2.dir 3.up dir
    // rotatio rpy   r=rotx ,p=roty,y=rotz,pos = camera pos
    // 姿态描述为 绕世界坐标xyz转动的角度
    //hits:GL场景中 Z轴正方向朝外，且右手定则
    GLCamera(KDL::Vector pos,KDL::Vector rpy);

//    static KDL::Frame CalLookAtBaseInput();
    //input increasement
    //delta 为输入，模式可选增量或绝对
    //指定相机的xyz运动
    void MoveX(double delta,bool is_abs = true);
    void MoveY(double delta,bool is_abs = true);
    void MoveZ(double delta,bool is_abs = true);
    //相机绕着世界坐标xyz运动
    void RotX(double delta,bool is_abs = true);
    void RotY(double delta,bool is_abs = true);
    void RotZ(double delta,bool is_abs = true);
    void RotAnyAxis(double delta,KDL::Vector ax,bool is_abs = true);
private:
    KDL::Frame look_frame;
    double invert_factor;
    void UpdateLookAt();

};
