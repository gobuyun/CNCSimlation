#pragma once

#include "TrajPlanning.h"



namespace CNC
{

class SVelPlanning:public TrajPlanningMethod
{
public:
	SVelPlanning();
    virtual ~SVelPlanning();

public:

	void SReset(); //参数复位
	bool GenerateALL(double Period_);
    virtual bool OnlinePlanning(double Period_, double &Pos_, double &Vel_,double &Acc_) override;
	virtual bool InitialTrajData() override;//初始化轨迹数据 检查是否可以生成轨迹

private:
	
	virtual bool GenerateVelData(double Period_, double &Vel_) override;//生成速度数据
	virtual bool GeneratePosData(double Period_, double &Pos_) override;//生成位置数据
	virtual bool GenerateACCData(double Period_, double &Acc_) override;//生成加速度数据
	bool GenerateJerkData(double Period_, double &Jerk_);//生成加加速度数据
	void SignTransform();
	bool CheckIsfeasible();
	bool GetMaxRunSpeed();
	bool RecomputeMaxRunSpeed();

	double Tj1_;//加速阶段，加加速度恒定的时间
	double Tj2_;//减速阶段，加加速度恒定的时间

	double Limit_DEC_;//整段S型曲线能够达到的最小减速度

	double Sign_;
	double Phase_ACC_Sign_;

};

}

