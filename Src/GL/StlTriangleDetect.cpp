#include "StlTriangleDetect.h"
#include "../Math/MotionPlanning/MathDataDefine.h"

namespace GMath
{




StlTriangleDetect::StlTriangleDetect()
{
    type = INTERSCTION_NONE;
}

StlTriangleDetect::~StlTriangleDetect()
{

}

bool StlTriangleDetect::SetAttackTriangle(const TriangleVertex &in)
{
    attack = in;
    //check 三角形顶点与法向量是否一致
    KDL::Vector a1b1 = in.b - in.a;
    KDL::Vector a1c1 = in.c - in.a;
    KDL::Vector n1 = a1b1*a1c1;

    if(!KDL::Equal(n1,in.n) && !KDL::Equal(n1,-in.n))
    {
        std::cout<< "Attack normal vector err!"<<std::endl;
        return false;
    }
    return true;
}

bool StlTriangleDetect::SetDefenseTriangle(const TriangleVertex &in)
{
    defense = in;

    KDL::Vector a1b1 = in.b - in.a;
    KDL::Vector a1c1 = in.c - in.a;
    KDL::Vector n1 = a1b1*a1c1;

    if(!KDL::Equal(n1,in.n) && !KDL::Equal(n1,-in.n))
    {

        std::cout<< "Defense normal vector err!"<<std::endl;
        return false;
    }
    return true;
}

IntersectionType StlTriangleDetect::Detect()
{
    IntersectionType ty = IsIntersection();

    if(ty == INTERSCTION_NONE)
    {
        type = INTERSCTION_NONE;
    }
    else if(ty == INTERSCTION_SAMEPLANE_UNKNOW)
    {
        type = INTERSCTION_SAMEPLANE_NONE;

        if(IsPointInTriangle(attack.a,defense) || IsPointInTriangle(attack.b,defense) || IsPointInTriangle(attack.c,defense))
        {
            type = INTERSCTION_SAMEPLANE;
        }
        else
        {
            if(IsStarofDavid(defense.a,defense.b,defense.n,attack) && IsStarofDavid(defense.b,defense.c,defense.n,attack) && IsStarofDavid(defense.c,defense.a,defense.n,attack))
            {
                type = INTERSCTION_SAMEPLANE;
            }
        }
        if(IsPointInTriangle(defense.a,attack) || IsPointInTriangle(defense.b,attack) || IsPointInTriangle(defense.c,attack))
        {
            type = INTERSCTION_SAMEPLANE;
        }
        else
        {
            if(IsStarofDavid(attack.a,attack.b,attack.n,defense) && IsStarofDavid(attack.b,attack.c,attack.n,defense) && IsStarofDavid(attack.c,attack.a,attack.n,defense))
            {
                type = INTERSCTION_SAMEPLANE;
            }
        }

    }else if(ty == INTERSCTION_DIFFPLANE_UNKNOW)
    {
        if(IsLineCrossTriangle(attack.a,attack.b,defense) || IsLineCrossTriangle(attack.b,attack.c,defense) || IsLineCrossTriangle(attack.c,attack.a,defense) ||
           IsLineCrossTriangle(defense.a,defense.b,attack) || IsLineCrossTriangle(defense.b,defense.c,attack) || IsLineCrossTriangle(defense.c,defense.a,attack) )
        {
            type = INTERSCTION_DIFFPLANE;
        }
        else
        {
            type = INTERSCTION_DIFFPLANE_NONE;
        }
    }

    return type;
}

bool StlTriangleDetect::IsPointInTriangle(const KDL::Vector &p,const TriangleVertex &in)
{

    KDL::Vector a1b1 = in.b - in.a;
    KDL::Vector a1c1 = in.c - in.a;
    KDL::Vector b1c1 = in.c - in.b;

    KDL::Vector b1a1 = in.a - in.b;
    KDL::Vector c1a1 = in.a - in.c;
    KDL::Vector c1b1 = in.b - in.c;

    KDL::Vector a1p = p - in.a;
    KDL::Vector b1p = p - in.b;
    KDL::Vector c1p = p - in.c;

    double k1 = dot(a1p,a1b1*in.n) * dot(a1c1,a1b1*in.n);
    double k2 = dot(b1p,b1c1*in.n) * dot(b1a1,b1c1*in.n);
    double k3 = dot(c1p,c1a1*in.n) * dot(c1b1,c1a1*in.n);

    if(k1<0 || k2<0 ||k3<0)
    {
        //k1 k2 k3 存在负值，则P不在T1中
        return false;
    }

    return true;

}

bool StlTriangleDetect::IsStarofDavid(const KDL::Vector &p1,const KDL::Vector &p2,const KDL::Vector &n1, const TriangleVertex &in)
{
    KDL::Vector a1a2 = in.a - p1;
    KDL::Vector a1b2 = in.b - p1;
    KDL::Vector a1c2 = in.c - p1;

    KDL::Vector p1p2 = p2 - p1;


    double k1 = dot(a1a2,p1p2*n1);
    double k2 = dot(a1b2,p1p2*n1);
    double k3 = dot(a1c2,p1p2*n1);

    if(SIGN(k1) == SIGN(k2) || SIGN(k2) == SIGN(k3))
    {
        //三个同号则不成立
        return false;
    }
    //不同时为正或不同时为负
    return true;
}

bool StlTriangleDetect::IsLineCrossTriangle(const KDL::Vector &p1, const KDL::Vector &p2, const TriangleVertex &in)
{
    KDL::Vector a1a2 = p1 - in.a;
    KDL::Vector a1b2 = p2 - in.a;

    KDL::Vector s1 = a1a2*in.n;
    KDL::Vector s2 = a1b2*in.n;
    //if(KDL::Equal(s1,KDL::Vector(0,0,0)) && KDL::Equal(s2,KDL::Vector(0,0,0)))
    if(KDL::Equal(a1a2*a1b2,in.n) || KDL::Equal(a1a2*a1b2,-1*in.n))
    {
        //共面
        if(IsPointInTriangle(p1,in) || IsPointInTriangle(p2,in))
        {
            return true;
        }

    }
    else
    {
        //异面
        double k1 = dot(a1a2,in.n);
        double k2 = dot(a1b2,in.n);

        if(k1*k2>0)
        {
            //同时在一边不相交
            return false;
        }
        // if(dot(s1,s2)>0)
        // {
        //     return false;
        // }

        KDL::Vector a1b1 = in.b - in.a;
        KDL::Vector a1c1 = in.c - in.a;

        KDL::Vector b1b2 = p2 - in.b;
        KDL::Vector b1a2 = p1 - in.b;
        KDL::Vector b1c1 = in.c - in.b;

        KDL::Vector b1a1 = in.a - in.b;
        KDL::Vector c1a1 = in.a - in.c;
        KDL::Vector c1a2 = p1 - in.c;
        KDL::Vector c1b1 = in.b - in.c;
        KDL::Vector c1b2 = p2 - in.c;


        double g1 = dot(a1b2,a1b1*a1a2) * dot(a1c1,a1b1*a1a2);
        double g2 = dot(b1b2,b1c1*b1a2) * dot(b1a1,b1c1*b1a2);
        double g3 = dot(c1b2,c1a1*c1a2) * dot(c1b1,c1a1*c1a2);

        if(g1>=0 && g2>=0 && g3>=0)
        {
            return true;
        }
    }

    return false;

}

IntersectionType StlTriangleDetect::IsIntersection()
{
    //判断两个空间三角形面片是否相交
    //判断T2的3个顶点是否在平面π１ 同侧
    KDL::Vector a1a2 = attack.a - defense.a;
    KDL::Vector a1b2 = attack.b - defense.a;
    KDL::Vector a1c2 = attack.c - defense.a;
    KDL::Vector res = KDL::Vector(dot(a1a2,defense.n),dot(a1b2,defense.n),dot(a1c2,defense.n));
    // //判断T1的3个顶点是否在平面π2同侧
    // KDL::Vector a2a1 = defense.a - attack.a;
    // KDL::Vector a2b1 = defense.b - attack.a;
    // KDL::Vector a2c1 = defense.c - attack.a;
    // KDL::Vector res = KDL::Vector(dot(a2a1,attack.n),dot(a2b1,attack.n),dot(a2c1,attack.n));
    if(SIGN(res.x()) == SIGN(res.y()) && SIGN(res.y())==SIGN(res.z()))
    {
        if(fabs(res.x())>KDL::epsilon && fabs(res.y())>KDL::epsilon && fabs(res.z())>KDL::epsilon)
        {
            return INTERSCTION_NONE;
        }
    }

    KDL::Vector f = defense.n * attack.n;

    if(KDL::Equal(f,KDL::Vector(0,0,0)))
    {
        if(KDL::Equal(res,KDL::Vector(0,0,0)))
        {
            //T1和 T2共面
            return INTERSCTION_SAMEPLANE_UNKNOW;
        }
    }
    else
    {
        return INTERSCTION_DIFFPLANE_UNKNOW;
    }
}


}

