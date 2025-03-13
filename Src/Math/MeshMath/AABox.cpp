#pragma once

#include "AABox.h"


namespace GMath
{

AABox::AABox(const std::vector<KDL::Vector> &vertices):BoundingBox(vertices)
{
    type = AABB;
    Initialized();
}

bool AABox::IsIntersects(const Ptr box)
{

    KDL::Vector min_p = box->GetMin();
    KDL::Vector max_p = box->GetMax();

    unsigned int k = 0;
    bool res1 = (min_point(k) >= min_p(k) && min_point(k) <= max_p(k)) || (min_p(k) >= min_point(k) && min_p(k) <= max_point(k));
    k++;
    bool res2 = (min_point(k) >= min_p(k) && min_point(k) <= max_p(k)) || (min_p(k) >= min_point(k) && min_p(k) <= max_point(k));
    k++;
    bool res3 = (min_point(k) >= min_p(k) && min_point(k) <= max_p(k)) || (min_p(k) >= min_point(k) && min_p(k) <= max_point(k));


    return res1 && res2 && res3;

}

bool AABox::IsPointInside(const KDL::Vector &point)
{
    for (unsigned int k = 0; k < 3; k++)
    {
        if(point(k)<min_point(k))
        {
            return false;
        }

        if(point(k)>max_point(k))
        {
            return false;
        }
    }

    return true;
}




void AABox::Initialized()
{
    FindMinMax(raw_vertices, min_point, max_point);
    base_max_point = max_point;
    base_min_point = min_point;

    for (unsigned int k = 0; k < 3; k++)
    {
        center(k) = (min_point(k)+max_point(k))*0.5;
        half_length(k) = (max_point(k) - min_point(k))*0.5;
    }

    box_each_vertices[0] = KDL::Vector(min_point.x(), max_point.y(), max_point.z());
    // Left-bottom-front.
    box_each_vertices[1] = KDL::Vector(min_point.x(), min_point.y(), max_point.z());
    // Right-bottom-front.
    box_each_vertices[2] = KDL::Vector(max_point.x(), min_point.y(), max_point.z());
    // Right-top-front.
    box_each_vertices[3] = KDL::Vector(max_point.x(), max_point.y(), max_point.z());
    // Right-top-back.
    box_each_vertices[4] = KDL::Vector(max_point.x(), max_point.y(), min_point.z());
    // Right-bottom-back.
    box_each_vertices[5] = KDL::Vector(max_point.x(), min_point.y(), min_point.z());
    // Left-bottom-back.
    box_each_vertices[6] = KDL::Vector(min_point.x(), min_point.y(), min_point.z());
    // Left-top-back.
    box_each_vertices[7] = KDL::Vector(min_point.x(), max_point.y(), min_point.z());

    base_box_each_vertices = box_each_vertices;
}



}
