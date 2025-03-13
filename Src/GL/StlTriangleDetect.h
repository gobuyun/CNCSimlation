#pragma once

#include "../Math/kdl/kinfam_io.hpp"
#include "../Math/kdl/frames_io.hpp"

namespace GMath
{

enum IntersectionType
{
    INTERSCTION_NONE = 0,//不相交
    INTERSCTION_SAMEPLANE,//同平面相交
    INTERSCTION_SAMEPLANE_NONE,//同平面不相交
    INTERSCTION_SAMEPLANE_UNKNOW,//同平面未知相交情况
    INTERSCTION_DIFFPLANE,//不同平面相交
    INTERSCTION_DIFFPLANE_NONE,//不同平面不相交
    INTERSCTION_DIFFPLANE_UNKNOW//不同平面未知相交情况
};

struct TriangleVertex
{

    KDL::Vector a;
    KDL::Vector b;
    KDL::Vector c;
    KDL::Vector n;//normal vector
    TriangleVertex()
    {

    }
    //传入三个顶点和法向量
    TriangleVertex(KDL::Vector a_,KDL::Vector b_,KDL::Vector c_,KDL::Vector n_)
    {
        a=a_;
        b=b_;
        c=c_;
        n=n_;
    }
};

class StlTriangleDetect
{
public:
    StlTriangleDetect();
    ~StlTriangleDetect();
    //如果三角形的三个顶点形成平面的法向量与提供的不一致，会返回false
    bool SetAttackTriangle(const TriangleVertex &in);
    bool SetDefenseTriangle(const TriangleVertex &in);
    //比较返回类型
    IntersectionType Detect();
private:
    //入侵三角形和防守三角形
    //attack 为T2 ，defense T1
    TriangleVertex attack,defense;
    IntersectionType type;
private:
    //判断一点是否在三角形T1内
    bool IsPointInTriangle(const KDL::Vector &p,const TriangleVertex &in);
    //判断三角形的三个顶点是否在p1p2边异测
    bool IsStarofDavid(const KDL::Vector &p1,const KDL::Vector &p2,const KDL::Vector &n1, const TriangleVertex &in);
    //判断边p1p2是否与T1相交
    bool IsLineCrossTriangle(const KDL::Vector &p1,const KDL::Vector &p2,const TriangleVertex &in);
    IntersectionType IsIntersection();


};

}


