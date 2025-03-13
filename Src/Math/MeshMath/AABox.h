#pragma once

#include "BoundingBox.h"


namespace GMath
{

class AABox : public BoundingBox
{
public:

    AABox(const std::vector<KDL::Vector> &vertices);
    //两个box是否存在交集
    virtual bool IsIntersects(const BoundingBox::Ptr box) override;
    //判断点是否在Box内
    virtual bool IsPointInside(const KDL::Vector &point) override;

protected:

    virtual void Initialized() override;

//    KDL::Vector max_point,min_point;
};

}
