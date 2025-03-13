#pragma once

#include "BoundingBox.h"


namespace GMath
{

class OBBox : public BoundingBox
{
public:

    OBBox(const std::vector<KDL::Vector> &vertices);
    //两个box是否存在交集
    virtual bool IsIntersects(const BoundingBox::Ptr box) override;
    //判断点是否在Box内
    virtual bool IsPointInside(const KDL::Vector &point) override;

protected:

    virtual void Initialized() override;

    double Project(KDL::Vector p1, KDL::Vector axis);

    void Interval(const std::vector<KDL::Vector> vertices,KDL::Vector axis,double &min, double &max);

};

}
