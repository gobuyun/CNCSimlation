#pragma once

#include "OBBox.h"
#include <Eigen/Eigenvalues>

namespace GMath
{

OBBox::OBBox(const std::vector<KDL::Vector> &vertices):BoundingBox(vertices)
{
    type = OBB;
    Initialized();
}

bool OBBox::IsIntersects(const Ptr box)
{
    double min1, max1, min2, max2;
    std::vector<KDL::Vector> vertices = box->GetCorners();
    for (unsigned int i = 0; i < 3; i++)
    {
        Interval(box_each_vertices, FaceDirection(i), min1, max1);
        Interval(vertices, FaceDirection(i), min2, max2);
        if (max1 < min2 || max2 < min1) return false;
    }

    for (unsigned int i = 0; i < 3; i++)
    {
        Interval(box_each_vertices, box->FaceDirection(i), min1, max1);
        Interval(vertices, box->FaceDirection(i), min2, max2);
        if (max1 < min2 || max2 < min1) return false;
    }

    for (unsigned int i = 0; i < 3; i++)
    {
        for (unsigned int j = 0; j < 3; j++)
        {
            KDL::Vector axis = EdgeDirection(i) * box->EdgeDirection(j);
            Interval(box_each_vertices, axis, min1, max1);
            Interval(vertices, axis, min2, max2);
            if (max1 < min2 || max2 < min1) return false;
        }
    }
    return true;
}

bool OBBox::IsPointInside(const KDL::Vector &point)
{
    KDL::Vector vd = point - center;


    double d = dot(vd,xAxis);

    if (d > half_length.x() || d < -half_length.x())
        return false;

    d =dot(vd,yAxis);
    if (d > half_length.y() || d < -half_length.y())
        return false;

    d = dot(vd,zAxis);
    if (d > half_length.z() || d < -half_length.z())
        return false;

    return true;
}

void OBBox::Initialized()
{
    //输入为input，输出为input的协方差矩阵covMat
    const unsigned int data_size = raw_vertices.size();

    assert(data_size>=2);//传入的数据至少有两个点

    Eigen::Vector3d means;
    means.setZero();

    for (unsigned int i = 0; i < data_size; i++)
    {
        means(0) += raw_vertices[i].x();
        means(1) += raw_vertices[i].y();
        means(2) += raw_vertices[i].z();
    }
    means /= data_size;

    Eigen::Matrix3d cov_mat;
    cov_mat.setZero();

    for (unsigned int i = 0; i < data_size; i++)
    {
        cov_mat(0,0) += (raw_vertices[i].x() - means(0)) * (raw_vertices[i].x() - means(0));
        cov_mat(0,1) += (raw_vertices[i].x() - means(0)) * (raw_vertices[i].y() - means(1));
        cov_mat(0,2) += (raw_vertices[i].x() - means(0)) * (raw_vertices[i].z() - means(2));
        cov_mat(1,1) += (raw_vertices[i].y() - means(1)) * (raw_vertices[i].y() - means(1));
        cov_mat(1,2) += (raw_vertices[i].y() - means(1)) * (raw_vertices[i].z() - means(2));
        cov_mat(2,2) += (raw_vertices[i].z() - means(2)) * (raw_vertices[i].z() - means(2));
    }

    cov_mat /= (data_size - 1);

    cov_mat(1,0) = cov_mat(0,1);
    cov_mat(2,0) = cov_mat(0,2);
    cov_mat(2,1) = cov_mat(1,2);

    //std::cout<<"my"<<cov_mat<<std::endl<<std::endl;


    // Eigen::MatrixXd input(data_size,3);
    // for (unsigned int i = 0; i < data_size; i++)
    // {
    //     input(i,0) = raw_vertices[i].x();
    //     input(i,1) = raw_vertices[i].y();
    //     input(i,2) = raw_vertices[i].z();
    // }

    // Eigen::MatrixXd meanVec = input.colwise().mean();
    // Eigen::RowVectorXd meanVecRow(Eigen::RowVectorXd::Map(meanVec.data(),input.cols()));

    // Eigen::MatrixXd zeroMeanMat = input;

    // zeroMeanMat.rowwise()-=meanVecRow;

    // Eigen::MatrixXd cov_mat;

    // cov_mat = (zeroMeanMat.adjoint()*zeroMeanMat)/double(input.rows()-1);

    // std::cout<<cov_mat<< std::endl;


    Eigen::EigenSolver<Eigen::Matrix3d> solver(cov_mat);

    //求解特征值
    Eigen::Matrix3d d = solver.pseudoEigenvalueMatrix();
    //std::cout << d<< std::endl;
    //求解特征向量
    Eigen::Matrix3d v = solver.pseudoEigenvectors();
    //std::cout << v << std::endl;

    //获取box方向向量
    xAxis = KDL::Vector(v.col(0)(0),v.col(0)(1),v.col(0)(2));
    yAxis = KDL::Vector(v.col(1)(0),v.col(1)(1),v.col(1)(2));

    if(data_size<3)
    {
        zAxis = xAxis * yAxis;
    }else
    {
        zAxis = KDL::Vector(v.col(2)(0),v.col(2)(1),v.col(2)(2));
    }

    KDL::Rotation trans = KDL::Rotation(xAxis,yAxis,zAxis);



    min_point = max_point = trans * raw_vertices[0];

    for (unsigned int i = 1; i < data_size; i++)
    {
        KDL::Vector vect = trans * raw_vertices[i];

        for (unsigned int k = 0; k < 3; k++)
        {
            max_point(k) = max_point(k) > vect(k) ? max_point(k) : vect(k);
            min_point(k) =  min_point(k) < vect(k) ?  min_point(k) : vect(k);
        }

    }
    center = (min_point+max_point)*0.5;
    center = trans.Inverse() * center;

    half_length = 0.5f * (max_point - min_point);

    KDL::Vector dx = xAxis * half_length.x();
    KDL::Vector dy = yAxis * half_length.y();
    KDL::Vector dz = zAxis * half_length.z();

    box_each_vertices[0] = center - dx + dy + dz;     // left top front
    box_each_vertices[1] = center - dx - dy + dz;     // left bottom front
    box_each_vertices[2] = center + dx - dy + dz;     // right bottom front
    box_each_vertices[3] = center + dx + dy + dz;     // right top front

    box_each_vertices[4] = center + dx + dy - dz;     // right top back
    box_each_vertices[5] = center + dx - dy - dz;     // right bottom back
    box_each_vertices[6] = center - dx - dy - dz;     // left bottom back
    box_each_vertices[7] = center - dx + dy - dz;     // left top back

    base_box_each_vertices = box_each_vertices;

}

void OBBox::Interval(const std::vector<KDL::Vector> vertices, KDL::Vector axis, double &min, double &max)
{

    min = max = Project(axis, vertices[0]);

    for(unsigned int i = 1; i < vertices.size(); i++)
    {
        double value = Project(axis, vertices[i]);

        min = fmin(min, value);
        max = fmax(max, value);
    }
}

double OBBox::Project(KDL::Vector p1, KDL::Vector axis)
{
    double d = dot(p1,axis);

    return d * p1.Norm();
}



}
