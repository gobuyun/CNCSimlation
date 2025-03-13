#pragma once

#include "BoundingBox.h"


namespace GMath
{

BoundingBox::BoundingBox(const std::vector<KDL::Vector> &vertices)
{
    Reset();
    raw_vertices = vertices;
}

KDL::Vector BoundingBox::GetCenter() const
{
    return center;
}

std::vector<KDL::Vector> BoundingBox::GetCorners() const
{
    return box_each_vertices;
}

KDL::Vector BoundingBox::GetMax() const
{
    return max_point;
}

KDL::Vector BoundingBox::GetMin() const
{
    return min_point;
}

void BoundingBox::Transform(const KDL::Vector &offset, const KDL::Vector &rot, bool is_abs)
{
    KDL::Frame trans = KDL::Frame(KDL::Rotation::RPY(rot.x(),rot.y(),rot.z()),offset);
    if(is_abs)
    {
        end_frame = trans;
    }else
    {
        end_frame = end_frame * trans;
    }
    if(type == AABB)
    {
        if(KDL::Equal(rot,KDL::Vector(0,0,0)))
        {
            for (unsigned int i = 0; i < box_each_vertices.size(); i++)
            {
                box_each_vertices.at(i) = end_frame * base_box_each_vertices.at(i);
            }
            max_point = end_frame * base_max_point;
            min_point = end_frame * base_min_point;
        }else
        {
            for (unsigned int i = 0; i < raw_vertices.size(); i++)
            {
                raw_vertices.at(i) = end_frame * raw_vertices.at(i);
            }
            Initialized();
        }
    }else
    {
        for (unsigned int i = 0; i < box_each_vertices.size(); i++)
        {
            box_each_vertices.at(i) = end_frame * base_box_each_vertices.at(i);
        }
        xAxis = end_frame * xAxis;
        yAxis = end_frame * yAxis;
        zAxis = end_frame * zAxis;
    }


}

void BoundingBox::FindMinMax(const std::vector<KDL::Vector> &vertices, KDL::Vector &min, KDL::Vector &max)
{
    //set max
    for (unsigned int i = 0; i < vertices.size(); i++)
    {
        for (unsigned int k = 0; k < 3; k++)
        {
            if(vertices.at(i)(k)<min(k))
                min(k) = vertices.at(i)(k);

            if(vertices.at(i)(k)>max(k))
                max(k) = vertices.at(i)(k);

        }
    }
}


void BoundingBox::Reset()
{
    end_frame = KDL::Frame::Identity();
    box_each_vertices.resize(8);
    xAxis = KDL::Vector(1,0,0);
    yAxis = KDL::Vector(0,1,0);
    zAxis = KDL::Vector(0,0,1);
    double limit = 99999999;
    max_point = KDL::Vector(-limit,-limit,-limit);
    min_point = KDL::Vector(limit,limit,limit);
}


KDL::Vector BoundingBox::EdgeDirection(unsigned int index)
{

    KDL::Vector line;

    switch(index)
    {
    case 0:// edge with x axis
        line = box_each_vertices[5] - box_each_vertices[6];
        line.Normalize();
        break;
    case 1:// edge with y axis
        line = box_each_vertices[7] - box_each_vertices[6];
        line.Normalize();
        break;
    case 2:// edge with z axis
        line = box_each_vertices[1] - box_each_vertices[6];
        line.Normalize();
        break;
    default:
        break;
    }
    return line;
}

KDL::Vector BoundingBox::FaceDirection(unsigned int index)
{

    KDL::Vector face, v0, v1;

    switch(index)
    {
    case 0:// front and back
        v0 = box_each_vertices[2] - box_each_vertices[1];
        v1 = box_each_vertices[0] - box_each_vertices[1];
        break;
    case 1:// left and right
        v0 = box_each_vertices[5] - box_each_vertices[2];
        v1 = box_each_vertices[3] - box_each_vertices[2];
        break;
    case 2:// top and bottom
        v0 = box_each_vertices[1] - box_each_vertices[2];
        v1 = box_each_vertices[5] - box_each_vertices[2];
        break;
    default:
        break;
    }

    face = v0 * v1;
    face.Normalize();

    return face;
}


}
