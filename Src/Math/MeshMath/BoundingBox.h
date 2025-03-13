#pragma once

#include <memory>
#include <vector>
#include "../kdl/kinfam_io.hpp"
#include "../kdl/frames_io.hpp"


namespace GMath
{

class BoundingBox
{
public:

    typedef std::shared_ptr<BoundingBox> Ptr;

    BoundingBox(const std::vector<KDL::Vector> &vertices);
    //两个box是否存在交集
    virtual bool IsIntersects(const BoundingBox::Ptr box) = 0;
    //判断点是否在Box内
    virtual bool IsPointInside(const KDL::Vector &point) = 0;

    //当模型平移或者旋转，调用更新box顶点位置，传入平移位置x y z 和 旋转角度rx ry rz  ,参数3 false代表相对于上个姿态进行变换
    void Transform(const KDL::Vector &offset,const KDL::Vector &rot,bool is_abs = false);
    //获取中心坐标
    KDL::Vector GetCenter() const;
    //获取8个顶点坐标
    std::vector<KDL::Vector> GetCorners() const;

    KDL::Vector GetMax() const;
    KDL::Vector GetMin() const;

    KDL::Vector EdgeDirection(unsigned int index);

    KDL::Vector FaceDirection(unsigned int index);

protected:

    enum BBType
    {
        AABB = 1,
        OBB
    }type;

    virtual void Initialized() = 0;

    void Reset();

    void FindMinMax(const std::vector<KDL::Vector> &vertices,KDL::Vector &min,KDL::Vector &max);




    KDL::Vector center;   // box center
    KDL::Vector xAxis;    // x axis of box, unit vector
    KDL::Vector yAxis;    // y axis of box, unit vector
    KDL::Vector zAxis;    // z axis of box, unit vector
    KDL::Vector half_length;  // box half length along each axis
    std::vector<KDL::Vector> box_each_vertices;//size must be = 8
    std::vector<KDL::Vector> base_box_each_vertices;//size must be = 8
    std::vector<KDL::Vector> raw_vertices;//原始顶点坐标
    KDL::Frame end_frame;
    KDL::Vector max_point,min_point;
    KDL::Vector base_max_point,base_min_point;


};

}
