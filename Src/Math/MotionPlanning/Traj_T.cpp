//#include "stdafx.h"
#include <math.h>
#include "Traj_T.h"
#include "MathDataDefine.h"

CNC::TVelPlanning::TVelPlanning()
{
	La = 0.0;
	Lv = 0.0;
	Ld = 0.0;

	Sign_ = 0.0;
}

CNC::TVelPlanning::~TVelPlanning()
{
}

void CNC::TVelPlanning::TReset()
{
	Reset();

	La = 0.0;
	Lv = 0.0;
	Ld = 0.0;

	Sign_ = 0.0;
}
//离线生成轨迹参数
bool CNC::TVelPlanning::GenerateALL(double Period_)
{
	double pos = 0.0;double vel = 0.0;
	bool res = false;

	for (double ts = 0.0; ts <= Traj_Data_.Traj_Time_; ts += Period_)
	{
		res = GeneratePosData(ts, pos);
		if (!res)
			break;
		Traj_Data_.PosData.push_back(pos);

		res = GenerateVelData(ts, vel);
		if (!res)
			break;
		Traj_Data_.VelData.push_back(vel);

	}
	return res;
}
//实时周期性生成轨迹位置
bool CNC::TVelPlanning::OnlinePlanning(double Period_, double &Pos_, double &Vel_,double &Acc_)
{

	if (Traj_Data_.Is_Traj_End_)
		return false;

	bool res = false;

	res = GeneratePosData(Traj_Data_.Online_Ts_, Pos_);
	if (!res)
		return false;
	GenerateVelData(Traj_Data_.Online_Ts_, Vel_);
    GenerateACCData(Traj_Data_.Online_Ts_, Acc_);
	//if (!res)
	//	return false;
	//只会在位置层调用实时规划
	Traj_Data_.Online_Ts_ += Period_;

	//非整数倍周期需要补全
	if (fmod(Traj_Data_.Traj_Time_, Period_) > DoubleErr)
	{
		if (Traj_Data_.Online_Ts_ > Traj_Data_.Traj_Time_ + Period_ + DoubleErr)
			Traj_Data_.Is_Traj_End_ = true;
	}
	else
	{
		if (Traj_Data_.Online_Ts_ > Traj_Data_.Traj_Time_ + DoubleErr)
			Traj_Data_.Is_Traj_End_ = true;
	}
	
	return true;
}
// 轨迹参数计算
bool CNC::TVelPlanning::InitialTrajData()
{
	Traj_Data_.Is_Generated_ = false;

	SignTransform();

	double dis = Traj_Data_.End_Pos_ - Traj_Data_.Start_Pos_; //单端两点间的距离
	 //可以达到的最大速度
	double vf = sqrt((2.0 * Traj_Data_.Max_ACC * Traj_Data_.Max_DEC * dis - Traj_Data_.Max_ACC * pow(Traj_Data_.End_Vel_, 2) + 
				Traj_Data_.Max_DEC * pow(Traj_Data_.Start_Vel_, 2)) / (Traj_Data_.Max_DEC - Traj_Data_.Max_ACC)); 
	//对匀速阶段速度进行判断
	if (vf < Traj_Data_.Max_Vel)
	{
		Traj_Data_.Limit_Vel_ = vf;
	}
	else
	{
		Traj_Data_.Limit_Vel_ = Traj_Data_.Max_Vel;
	}
	//计算加速阶段的时间和位移
	Traj_Data_.Ta_ = (Traj_Data_.Limit_Vel_ - Traj_Data_.Start_Vel_) / Traj_Data_.Max_ACC;
	La = Traj_Data_.Start_Vel_ * Traj_Data_.Ta_ + 0.5 * Traj_Data_.Max_ACC * pow(Traj_Data_.Ta_, 2);

	//计算匀速阶段的时间和位移
	Traj_Data_.Tv_ = (dis - (pow(Traj_Data_.Limit_Vel_, 2) - pow(Traj_Data_.Start_Vel_, 2)) / (2.0 * Traj_Data_.Max_ACC) - (pow(Traj_Data_.End_Vel_, 2) - pow(Traj_Data_.Limit_Vel_, 2)) / (2.0 * Traj_Data_.Max_DEC)) / Traj_Data_.Limit_Vel_;
	Lv = Traj_Data_.Limit_Vel_ * Traj_Data_.Tv_;

	//计算减速阶段的时间和位移
	Traj_Data_.Td_ = (Traj_Data_.End_Vel_ - Traj_Data_.Limit_Vel_) / Traj_Data_.Max_DEC;
	Ld = Traj_Data_.Limit_Vel_ * Traj_Data_.Td_ + 0.5 * Traj_Data_.Max_DEC * pow(Traj_Data_.Td_, 2);

	//计算总时间
	Traj_Data_.Traj_Time_ = Traj_Data_.Ta_ + Traj_Data_.Tv_ + Traj_Data_.Td_;

	////总时间向上取整
	//if (fabs(MOD(Traj_Data_.Traj_Time_ ,Period_))  >= DoubleErr)
	//{
	//	Traj_Data_.Traj_Time_ = ceil(Traj_Data_.Traj_Time_ / Period_) * Period_;
	//}

	Traj_Data_.Is_Generated_ = true;

	return Traj_Data_.Is_Generated_;
}
//计算规划加速度
bool CNC::TVelPlanning::GenerateACCData(double Period_, double &Acc_)
{
	if (!Traj_Data_.Is_Generated_)
		return false;
	double acc = 0.0;

	if (Period_ + DoubleErr >= 0.0 && Period_ < Traj_Data_.Ta_)
	{
		acc = Traj_Data_.Max_ACC;
	}
    else if (Period_ + DoubleErr >= Traj_Data_.Ta_ && Period_ <= Traj_Data_.Ta_ + Traj_Data_.Tv_)
	{
		acc = 0;
	}
    else if (Period_ + DoubleErr >= Traj_Data_.Ta_ + Traj_Data_.Tv_ && Period_ + DoubleErr <= Traj_Data_.Traj_Time_)
	{
		acc = Traj_Data_.Max_DEC;
	}

	Acc_ = Sign_ * acc;

	return true;
}
//计算规划速度
bool CNC::TVelPlanning::GenerateVelData(double Period_, double &Vel_)
{
	if (!Traj_Data_.Is_Generated_)
		return false;
	double vel = 0.0;
	if (Period_ + DoubleErr >= 0.0 && Period_ < Traj_Data_.Ta_)
	{
		vel = Traj_Data_.Start_Vel_ + Traj_Data_.Max_ACC * Period_;
		//Traj_Data_.Limit_Vel_ = Vel_;
	}
    else if (Period_ + DoubleErr>= Traj_Data_.Ta_ && Period_ <= Traj_Data_.Ta_ + Traj_Data_.Tv_)
	{
		vel = Traj_Data_.Limit_Vel_;
	}
    else if (Period_ + DoubleErr >= Traj_Data_.Ta_ + Traj_Data_.Tv_ && Period_ + DoubleErr < Traj_Data_.Traj_Time_)
	{
		vel = Traj_Data_.Limit_Vel_ + Traj_Data_.Max_DEC * (Period_ - Traj_Data_.Ta_ - Traj_Data_.Tv_);
	}
	else
	{
		vel= Traj_Data_.End_Vel_;
	}

	Vel_ = Sign_ * vel;

	return true;
}
//计算点位
bool CNC::TVelPlanning::GeneratePosData(double Period_, double &Pos)
{

	if (!Traj_Data_.Is_Generated_)
		return false;

	double pos = 0.0;

	if (Period_ + DoubleErr >= 0.0 && Period_ < Traj_Data_.Ta_)
	{
		pos = Traj_Data_.Start_Pos_ + Traj_Data_.Start_Vel_ * Period_ + 0.5 * Traj_Data_.Max_ACC * pow(Period_, 2);
	}
    else if (Period_ + DoubleErr >= Traj_Data_.Ta_ && Period_ <= Traj_Data_.Ta_ + Traj_Data_.Tv_)
	{
		pos = Traj_Data_.Start_Pos_ + La + Traj_Data_.Limit_Vel_ * (Period_ - Traj_Data_.Ta_);
	}
    else if (Period_ + DoubleErr >= Traj_Data_.Ta_ + Traj_Data_.Tv_ && Period_ + DoubleErr < Traj_Data_.Traj_Time_)
	{
		pos = Traj_Data_.Start_Pos_ + La + Lv + Traj_Data_.Limit_Vel_ * (Period_ - Traj_Data_.Ta_ - Traj_Data_.Tv_) + 0.5 * Traj_Data_.Max_DEC * pow((Period_ - Traj_Data_.Ta_ - Traj_Data_.Tv_), 2);
	}
	else 
	{
		pos= Traj_Data_.End_Pos_;
	}

	Pos = Sign_ * pos;

	return true;
}

// 函数作用：对位置参数进行转换
void CNC::TVelPlanning::SignTransform()
{
	Sign_ = copysign(1.0, (Traj_Data_.End_Pos_ - Traj_Data_.Start_Pos_));

	double vs1 = (Sign_ + 1.0) / 2.0;
	double vs2 = (Sign_ - 1.0) / 2.0;

	Traj_Data_.Start_Pos_ = Sign_ * Traj_Data_.Start_Pos_;
	Traj_Data_.End_Pos_ = Sign_ * Traj_Data_.End_Pos_;
	Traj_Data_.Start_Vel_ = Sign_ * Traj_Data_.Start_Vel_;
	Traj_Data_.End_Vel_ = Sign_ * Traj_Data_.End_Vel_;
	Traj_Data_.Max_Vel = (vs1 * Traj_Data_.Max_Vel) + (vs2 *(-Traj_Data_.Max_Vel));//double v_min = -Max_Vel;
	Traj_Data_.Max_ACC = (vs1 * Traj_Data_.Max_ACC) + (vs2 * (-Traj_Data_.Max_ACC));//double a_min = -Max_ACC;

}
