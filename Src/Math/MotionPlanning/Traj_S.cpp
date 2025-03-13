//#include "stdafx.h"
#include <math.h>
#include "TrajPlanning.h"
#include "Traj_S.h"
#include "MathDataDefine.h"


CNC::SVelPlanning::SVelPlanning()
{
	Tj1_ = 0.0;
	Tj2_ = 0.0;

	Limit_DEC_ = 0.0;

	Sign_ = 0.0;
	Phase_ACC_Sign_ = 0.0;
}

CNC::SVelPlanning::~SVelPlanning()
{
}

void CNC::SVelPlanning::SReset()
{
	//复位基类的数据
	Reset();

	//复位本类的数据
	Tj1_ = 0.0;
	Tj2_ = 0.0;

	Limit_DEC_ = 0.0;

	Sign_ = 0.0;
	Phase_ACC_Sign_ = 0.0;

}
//函数作用：离线生成轨迹参数
bool CNC::SVelPlanning::GenerateALL(double Period_)
{
	double pos = 0.0 ,vel = 0.0 ,acc = 0.0;
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

		res = GenerateACCData(ts, acc);
		if (!res)
			break;
		Traj_Data_.AccData.push_back(acc);

	}
	return res;
}
//函数作用：实时周期性生成轨迹位置
bool CNC::SVelPlanning::OnlinePlanning(double Period_, double &Pos_ ,double &Vel_,double &Acc_)
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
//函数作用：生成轨迹参数
bool CNC::SVelPlanning::InitialTrajData()
{
	Traj_Data_.Is_Generated_ = false;

	//输入参数转换
	SignTransform();

	//判断参数是否能规划出S型轨迹 
	if (CheckIsfeasible())
	{
		//printf("参数正确，可以进行s型加减速规划\n");
		if (GetMaxRunSpeed())
		{
			//printf("能够达到最大速度，规划结果的时间参数为：Tj1:%f, Ta:%f, Tj2:%f, Td:%f, Tv:%f\n",
			//	Tj1_, Traj_Data_.Ta_, Tj2_, Traj_Data_.Td_, Traj_Data_.Tv_);
		}
		else if (RecomputeMaxRunSpeed())
		{
			//printf("无法达到最大速度，规划结果的时间参数为：Tj1:%f, Ta:%f, Tj2:%f, Td:%f, Tv:%f\n",
			//	Tj1_, Traj_Data_.Ta_, Tj2_, Traj_Data_.Td_, Traj_Data_.Tv_);
		}
		else
		{
			//printf("轨迹不可行");
			return false;
		}
	}
	else
	{
		//printf("轨迹不可行");
		return false;
	}


	//traj 时间非周期整数倍需要补全一个周期
	Traj_Data_.Traj_Time_ = Traj_Data_.Ta_ + Traj_Data_.Td_ + Traj_Data_.Tv_;


	Traj_Data_.Limit_ACC_ = Traj_Data_.Max_Jerk * Tj1_;

	Limit_DEC_ = -Traj_Data_.Max_Jerk * Tj2_;

	Traj_Data_.Limit_Vel_ = abs(Traj_Data_.Start_Vel_) + Phase_ACC_Sign_ * (Traj_Data_.Ta_ - Tj1_) * Traj_Data_.Limit_ACC_;

	Traj_Data_.Is_Generated_ = true;

	//printf("规划结果：T: %f, Tj1: %f, Ta: %f, Tj2: %f, Td: %f, Tv: %f",
	//	Traj_Data_.Traj_Time_,Tj1_,Traj_Data_.Ta_,Tj2_,Traj_Data_.Td_,Traj_Data_.Tv_);

	return Traj_Data_.Is_Generated_; 
}
// 函数作用：计算t时刻的加速度 
bool CNC::SVelPlanning::GenerateACCData(double Period_, double &Acc_)
{
	if (!Traj_Data_.Is_Generated_)
	{
		return false;
	}
	double ret_val = 0.0;//初始化返回值

	 // 阶段0
	if (Period_ + DoubleErr >= 0.0 && Period_ < Tj1_)
	{
		ret_val = (Traj_Data_.Max_Jerk * Phase_ACC_Sign_) * Period_;
	}
	// 阶段1
    else if (Period_ + DoubleErr>= Tj1_ && Period_ < (Traj_Data_.Ta_ - Tj1_))
	{
		ret_val = Phase_ACC_Sign_ * Traj_Data_.Limit_ACC_;
	}
	// 阶段2
    else if (Period_ + DoubleErr>= (Traj_Data_.Ta_ - Tj1_) && Period_ < Traj_Data_.Ta_)
	{
		ret_val = (Traj_Data_.Max_Jerk * Phase_ACC_Sign_) * (Traj_Data_.Ta_ - Period_);
	}
	// 阶段3
    else if (Period_ + DoubleErr>= Traj_Data_.Ta_ && Period_ < (Traj_Data_.Ta_ + Traj_Data_.Tv_))
	{
		ret_val = 0.0;
	}
	// 阶段4
    else if (Period_ + DoubleErr>= (Traj_Data_.Traj_Time_ - Traj_Data_.Td_) && Period_ < (Traj_Data_.Traj_Time_ - Traj_Data_.Td_ + Tj2_))
	{
		ret_val = -Traj_Data_.Max_Jerk * (Period_ - Traj_Data_.Traj_Time_ + Traj_Data_.Td_);
	}
	// 阶段5
    else if (Period_ + DoubleErr>= (Traj_Data_.Traj_Time_ - Traj_Data_.Td_ + Tj2_) && Period_ < (Traj_Data_.Traj_Time_ - Tj2_))
	{
		//double tt = Period_ - Traj_Data_.Traj_Time_ + Traj_Data_.Td_;
		ret_val = Limit_DEC_;
	}
	// 阶段6
    else if (Period_ + DoubleErr>= (Traj_Data_.Traj_Time_ - Tj2_) && Period_ + DoubleErr< Traj_Data_.Traj_Time_)
	{
		ret_val = -Traj_Data_.Max_Jerk * (Traj_Data_.Traj_Time_ - Period_);
	}
	else
	{
		ret_val = 0.0;
	}

	Acc_ = ret_val * Sign_;

	return true;
}
// 函数作用：计算t时刻的加加速度 
bool CNC::SVelPlanning::GenerateJerkData(double Period_, double &Jerk_)
{
	if (!Traj_Data_.Is_Generated_)
	{
		return false;
	}
	double ret_val = 0.0;//初始化返回值

	// 阶段0
	if (Period_ + DoubleErr >= 0.0 && Period_ < Tj1_)
	{
		ret_val = Traj_Data_.Max_Jerk;
	}
	// 阶段1
    else if (Period_ + DoubleErr>= Tj1_ && Period_ < (Traj_Data_.Ta_ - Tj1_))
	{
		ret_val = 0.0;
	}
	// 阶段2
    else if (Period_ + DoubleErr>= (Traj_Data_.Ta_ - Tj1_) && Period_ < Traj_Data_.Ta_)
	{
		ret_val = -Traj_Data_.Max_Jerk;
	}
	// 阶段3
    else if (Period_ + DoubleErr>= Traj_Data_.Ta_ && Period_ < (Traj_Data_.Ta_ + Traj_Data_.Tv_))
	{
		ret_val = 0.0;
	}
	// 阶段4
    else if (Period_ + DoubleErr>= (Traj_Data_.Traj_Time_ - Traj_Data_.Td_) && Period_ < (Traj_Data_.Traj_Time_ - Traj_Data_.Td_ + Tj2_))
	{
		ret_val = -Traj_Data_.Max_Jerk;
	}
	// 阶段5
    else if (Period_ + DoubleErr>= (Traj_Data_.Traj_Time_ - Traj_Data_.Td_ + Tj2_) && Period_ < (Traj_Data_.Traj_Time_ - Tj2_))
	{
		ret_val = 0.0;
	}
	// 阶段6
    else if (Period_ + DoubleErr>= (Traj_Data_.Traj_Time_ - Tj2_) && Period_ + DoubleErr< Traj_Data_.Traj_Time_)
	{
		ret_val = Traj_Data_.Max_Jerk;
	}
	else
	{
		ret_val = 0.0;
	}

	Jerk_ = ret_val * Sign_;

	return true;
}
// 函数作用：计算t时刻的速度 
bool CNC::SVelPlanning::GenerateVelData(double Period_, double &Vel_)
{
	if (!Traj_Data_.Is_Generated_)
	{
		return false;
	}

	double ret_val = 0.0;

	// 阶段0
	if (Period_ + DoubleErr >= 0.0 && Period_ < Tj1_)
	{
		ret_val = Traj_Data_.Start_Vel_ + (Traj_Data_.Max_Jerk * Phase_ACC_Sign_) * pow(Period_, 2) / 2.0;
	}
	// 阶段1
    else if (Period_ + DoubleErr>= Tj1_ && Period_ < (Traj_Data_.Ta_ - Tj1_))
	{
		ret_val = Traj_Data_.Start_Vel_ + (Phase_ACC_Sign_ * Traj_Data_.Limit_ACC_) * (Period_ - Tj1_ / 2.0);
	}
	// 阶段2
    else if (Period_ + DoubleErr>= (Traj_Data_.Ta_ - Tj1_) && Period_ < Traj_Data_.Ta_)
	{
		double tt = Traj_Data_.Ta_ - Period_;
		ret_val = Traj_Data_.Limit_Vel_ - (Traj_Data_.Max_Jerk * Phase_ACC_Sign_) * pow(tt, 2) / 2.0;
	}
	// 阶段3
    else if (Period_ + DoubleErr>= Traj_Data_.Ta_ && Period_ < (Traj_Data_.Ta_ + Traj_Data_.Tv_))
	{
		ret_val = Traj_Data_.Limit_Vel_;
	}
	// 阶段4
    else if (Period_ + DoubleErr>= (Traj_Data_.Traj_Time_ - Traj_Data_.Td_) && Period_ < (Traj_Data_.Traj_Time_ - Traj_Data_.Td_ + Tj2_))
	{
		double tt = Period_ - Traj_Data_.Traj_Time_ + Traj_Data_.Td_;
		ret_val = Traj_Data_.Limit_Vel_ - Traj_Data_.Max_Jerk * pow(tt, 2) / 2.0;
	}
	// 阶段5
    else if (Period_ + DoubleErr>= (Traj_Data_.Traj_Time_ - Traj_Data_.Td_ + Tj2_) && Period_ < (Traj_Data_.Traj_Time_ - Tj2_))
	{
		double tt = Period_ - Traj_Data_.Traj_Time_ + Traj_Data_.Td_;
		ret_val = Traj_Data_.Limit_Vel_ + Limit_DEC_ * (tt - Tj2_ / 2.0);
	}
	// 阶段6
    else if (Period_ + DoubleErr>= (Traj_Data_.Traj_Time_ - Tj2_) && Period_ + DoubleErr< Traj_Data_.Traj_Time_)
	{
		double tt = Traj_Data_.Traj_Time_ - Period_;
		ret_val = Traj_Data_.End_Vel_ + Traj_Data_.Max_Jerk * pow(tt, 2) / 2.0;
	}
	else
	{
		ret_val = Traj_Data_.End_Vel_;
	}

	Vel_ = ret_val * Sign_;

	return true;
}
// 函数作用：计算t时刻的位置 
bool CNC::SVelPlanning::GeneratePosData(double Period_, double &Pos_)
{
	if (!Traj_Data_.Is_Generated_)
	{
		return false;
	}

	double ret_val = 0.0;

	// Phase 0
	if (Period_ + DoubleErr >= 0.0 && Period_ < Tj1_)
	{
		ret_val = Traj_Data_.Start_Pos_ + Traj_Data_.Start_Vel_ * Period_ + Traj_Data_.Max_Jerk * pow(Period_, 3) / 6.0;
	}
	// Phase 1
    else if (Period_ + DoubleErr >= Tj1_ && Period_ < (Traj_Data_.Ta_ - Tj1_))
	{
		ret_val = Traj_Data_.Start_Pos_ + Traj_Data_.Start_Vel_ * Period_ + (Phase_ACC_Sign_ * Traj_Data_.Limit_ACC_) * (3.0 * pow(Period_, 2) - 3.0 * Tj1_ * Period_ + pow(Tj1_, 2)) / 6.0;
	}
	// Phase 2
    else if (Period_ + DoubleErr >= (Traj_Data_.Ta_ - Tj1_) && Period_ < Traj_Data_.Ta_)
	{
		double tt = Traj_Data_.Ta_ - Period_;
		ret_val = Traj_Data_.Start_Pos_ + (Traj_Data_.Limit_Vel_ + Traj_Data_.Start_Vel_) * Traj_Data_.Ta_ / 2.0 - Traj_Data_.Limit_Vel_ * tt + Traj_Data_.Max_Jerk * pow(tt, 3) / 6.0;
	}
	// Phase 3
    else if (Period_ + DoubleErr >= Traj_Data_.Ta_ && Period_ < (Traj_Data_.Ta_ + Traj_Data_.Tv_))
	{
		ret_val = Traj_Data_.Start_Pos_ + (Traj_Data_.Limit_Vel_ + Traj_Data_.Start_Vel_) * Traj_Data_.Ta_ / 2.0 + Traj_Data_.Limit_Vel_ * (Period_ - Traj_Data_.Ta_);
	}
	// Phase 4
    else if (Period_ + DoubleErr>= (Traj_Data_.Traj_Time_ - Traj_Data_.Td_) && Period_ < (Traj_Data_.Traj_Time_ - Traj_Data_.Td_ + Tj2_))
	{
		double tt = Period_ - Traj_Data_.Traj_Time_ + Traj_Data_.Td_;
		ret_val = Traj_Data_.End_Pos_ - (Traj_Data_.Limit_Vel_ + Traj_Data_.End_Vel_) * Traj_Data_.Td_ / 2.0 + Traj_Data_.Limit_Vel_ * tt - Traj_Data_.Max_Jerk * pow(tt, 3) / 6.0;
	}
	// Phase 5
    else if (Period_ + DoubleErr >= (Traj_Data_.Traj_Time_ - Traj_Data_.Td_ + Tj2_) && Period_ < (Traj_Data_.Traj_Time_ - Tj2_))
	{
		double tt = Period_ - Traj_Data_.Traj_Time_ + Traj_Data_.Td_;
		ret_val = Traj_Data_.End_Pos_ - (Traj_Data_.Limit_Vel_ + Traj_Data_.End_Vel_) * Traj_Data_.Td_ / 2.0 + Traj_Data_.Limit_Vel_ * tt
			+ Limit_DEC_ * (3.0 * pow(tt, 2) - 3.0 * Tj2_ * tt + pow(Tj2_, 2)) / 6.0;
	}
	// Phase 6
    else if (Period_ + DoubleErr >= (Traj_Data_.Traj_Time_ - Tj2_) && Period_ + DoubleErr< Traj_Data_.Traj_Time_)
	{
		double tt = Traj_Data_.Traj_Time_ - Period_;
		ret_val = Traj_Data_.End_Pos_ - Traj_Data_.End_Vel_ * tt - Traj_Data_.Max_Jerk * pow(tt, 3) / 6.0;
	}
	// Other case
	else
	{
		ret_val = Traj_Data_.End_Pos_;
	}

	Pos_ = ret_val * Sign_;

	return true;
}
// 函数作用：对位置参数进行转换
void CNC::SVelPlanning::SignTransform()
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
	Traj_Data_.Max_Jerk = (vs1 * Traj_Data_.Max_Jerk) + (vs2 * (-Traj_Data_.Max_Jerk));//double j_min = -Max_Jerk;
}
// 函数作用：检查给定参数是否能输出轨迹，参数正确性检查
bool CNC::SVelPlanning::CheckIsfeasible()
{
	double dv = abs(Traj_Data_.Start_Vel_ - Traj_Data_.End_Vel_);
	double dq = abs(Traj_Data_.End_Pos_  - Traj_Data_.Start_Pos_);

	double time_to_reach_max_a = Traj_Data_.Max_ACC / Traj_Data_.Max_Jerk;
	double time_to_set_speeds = sqrt(dv / Traj_Data_.Max_Jerk);

	double Tjs = MIN(time_to_reach_max_a, time_to_set_speeds);
	
	if (Tjs == time_to_reach_max_a)
	{
		return (dq > (0.5 * (Traj_Data_.Start_Vel_ + Traj_Data_.End_Vel_) * (Tjs + (dv / Traj_Data_.Max_ACC))));
	}
	else if (Tjs < time_to_reach_max_a)
	{
		return (dq >(Tjs * (Traj_Data_.Start_Vel_ + Traj_Data_.End_Vel_)));
	}

	//std:cout << "参数错误，规划失败" << std::endl;
	return false;
}
// 函数作用：能够达到最大速度的情况对时间参数进行计算 
bool CNC::SVelPlanning::GetMaxRunSpeed()
{
	// Acceleration Period
	Tj1_ = 0.0;
	Traj_Data_.Ta_ = 0.0;

	Phase_ACC_Sign_ = copysign(1.0, (Traj_Data_.Max_Vel - Traj_Data_.Start_Vel_));
	if ((abs(Traj_Data_.Max_Vel - Traj_Data_.Start_Vel_) * Traj_Data_.Max_Jerk) < pow(Traj_Data_.Max_ACC, 2))
	{
		Tj1_ = sqrt(abs(Traj_Data_.Max_Vel - Traj_Data_.Start_Vel_) / Traj_Data_.Max_Jerk);
		Traj_Data_.Ta_ = 2.0 * Tj1_;
	}
	else
	{
		Tj1_ = Traj_Data_.Max_ACC / Traj_Data_.Max_Jerk;
		Traj_Data_.Ta_ = Tj1_ + abs(Traj_Data_.Max_Vel - Traj_Data_.Start_Vel_) / Traj_Data_.Max_ACC;
	}

	// Deceleration Period
	Tj2_ = 0.0;
	Traj_Data_.Td_ = 0.0;
	if (((Traj_Data_.Max_Vel - Traj_Data_.End_Vel_) * Traj_Data_.Max_Jerk) < pow(Traj_Data_.Max_ACC, 2))
	{
		Tj2_ = sqrt((Traj_Data_.Max_Vel - Traj_Data_.End_Vel_) / Traj_Data_.Max_Jerk);
		Traj_Data_.Td_ = 2.0 * Tj2_;
	}
	else
	{
		Tj2_ = Traj_Data_.Max_ACC / Traj_Data_.Max_Jerk;
		Traj_Data_.Td_ = Tj2_ + (Traj_Data_.Max_Vel - Traj_Data_.End_Vel_) / Traj_Data_.Max_ACC;
	}

	Traj_Data_.Tv_ = ((Traj_Data_.End_Pos_  - Traj_Data_.Start_Pos_) / Traj_Data_.Max_Vel)
		- ((Traj_Data_.Ta_ / 2.0) * (1.0 + Traj_Data_.Start_Vel_ / Traj_Data_.Max_Vel))
		- ((Traj_Data_.Td_ / 2.0) * (1.0 + Traj_Data_.End_Vel_ / Traj_Data_.Max_Vel));

	if (Traj_Data_.Tv_ + DoubleErr < 0.0)
	{
		return false;
	}

	//Out_[0] = Tj1;
	//Out_[1] = Ta;
	//Out_[2] = Tj2;
	//Out_[3] = Td;
	//Out_[4] = Tv;

	return true;
}
// 函数作用：无法达到最大速度的情况对时间参数进行计算 
bool CNC::SVelPlanning::RecomputeMaxRunSpeed()
{
	Tj1_ = Traj_Data_.Max_ACC / Traj_Data_.Max_Jerk;
	Tj2_ = Traj_Data_.Max_ACC / Traj_Data_.Max_Jerk;
	double Tj = Traj_Data_.Max_ACC / Traj_Data_.Max_Jerk;
	Traj_Data_.Tv_ = 0.0;

	double delta = (pow(Traj_Data_.Max_ACC, 4) / pow(Traj_Data_.Max_Jerk, 2))
		+ 2.0 * (pow(Traj_Data_.Start_Vel_, 2) + pow(Traj_Data_.End_Vel_, 2))
		+ Traj_Data_.Max_ACC * (4.0 * (Traj_Data_.End_Pos_  - Traj_Data_.Start_Pos_) - 2.0 * (Traj_Data_.Max_ACC / Traj_Data_.Max_Jerk) * (Traj_Data_.Start_Vel_ + Traj_Data_.End_Vel_));
	Traj_Data_.Ta_ = (pow(Traj_Data_.Max_ACC, 2) / Traj_Data_.Max_Jerk - 2.0 * Traj_Data_.Start_Vel_ + sqrt(delta)) / (2.0 * Traj_Data_.Max_ACC);
	Traj_Data_.Td_ = (pow(Traj_Data_.Max_ACC, 2) / Traj_Data_.Max_Jerk - 2.0 * Traj_Data_.End_Vel_ + sqrt(delta)) / (2.0 * Traj_Data_.Max_ACC);

	if ((Traj_Data_.Ta_ - 2.0 * Tj) > DoubleErr && (Traj_Data_.Td_ - 2.0 * Tj > DoubleErr))
	{
		//加速段和减速段均能达到最大速度
		//Out_[0] = Tj1;
		//Out_[1] = Ta;
		//Out_[2] = Tj2;
		//Out_[3] = Td;
		//Out_[4] = Tv;

		return true;
	}
	else
	{
		//至少有一段不能达到最大加速度
		double gamma = 0.99;
		//逐渐减小最大加速度约束
		while (Traj_Data_.Ta_ - 2.0 * Tj < 0 || Traj_Data_.Td_ - 2.0 * Tj < 0)
		{
			if (Traj_Data_.Ta_ > 0 && Traj_Data_.Td_ > 0)
			{
				Traj_Data_.Max_ACC = gamma * Traj_Data_.Max_ACC;
				Tj = Traj_Data_.Max_ACC / Traj_Data_.Max_Jerk;
				Tj1_ = Tj;
				Tj2_ = Tj;
				delta = (pow(Traj_Data_.Max_ACC, 4) / pow(Traj_Data_.Max_Jerk, 2))
					+ 2.0 * (pow(Traj_Data_.Start_Vel_, 2) + pow(Traj_Data_.End_Vel_, 2))
					+ Traj_Data_.Max_ACC * (4.0 * (Traj_Data_.End_Pos_  - Traj_Data_.Start_Pos_) - 2.0 * (Traj_Data_.Max_ACC / Traj_Data_.Max_Jerk) * (Traj_Data_.Start_Vel_ + Traj_Data_.End_Vel_));

				Traj_Data_.Ta_ = (pow(Traj_Data_.Max_ACC, 2.0) / Traj_Data_.Max_Jerk - 2.0 * Traj_Data_.Start_Vel_ + sqrt(delta)) / (2.0 * Traj_Data_.Max_ACC);
				Traj_Data_.Td_ = (pow(Traj_Data_.Max_ACC, 2.0) / Traj_Data_.Max_Jerk - 2.0 * Traj_Data_.End_Vel_ + sqrt(delta)) / (2.0 * Traj_Data_.Max_ACC);
			}
			else
			{
				//出现Ta_或Td_小于0
				if (Traj_Data_.Ta_ + DoubleErr <= 0)
				{
					Traj_Data_.Ta_ = 0;
					Tj1_ = 0;
					Traj_Data_.Td_ = 2 * (Traj_Data_.End_Pos_  - Traj_Data_.Start_Pos_) / (Traj_Data_.Start_Vel_ + Traj_Data_.End_Vel_);
					double num = (Traj_Data_.Max_Jerk * (Traj_Data_.End_Pos_  - Traj_Data_.Start_Pos_) - sqrt(Traj_Data_.Max_Jerk * (Traj_Data_.Max_Jerk * pow((Traj_Data_.End_Pos_  - Traj_Data_.Start_Pos_), 2) + pow((Traj_Data_.End_Vel_ + Traj_Data_.Start_Vel_), 2) * (Traj_Data_.End_Vel_ - Traj_Data_.Start_Vel_))));
					double den = Traj_Data_.Max_Jerk * (Traj_Data_.End_Vel_ + Traj_Data_.Start_Vel_);
					Tj2_ = num / den;
				}
				else if (Traj_Data_.Td_ + DoubleErr <= 0)
				{
					Traj_Data_.Td_ = 0;
					Tj2_ = 0;
					Traj_Data_.Ta_ = 2 * ((Traj_Data_.End_Pos_  - Traj_Data_.Start_Pos_) / (Traj_Data_.End_Vel_ + Traj_Data_.Start_Vel_));
					double num = (Traj_Data_.Max_Jerk * (Traj_Data_.End_Pos_  - Traj_Data_.Start_Pos_) - sqrt(Traj_Data_.Max_Jerk * (Traj_Data_.Max_Jerk * pow((Traj_Data_.End_Pos_  - Traj_Data_.Start_Pos_), 2) - pow((Traj_Data_.End_Vel_ + Traj_Data_.Start_Vel_), 2) * (Traj_Data_.End_Vel_ - Traj_Data_.Start_Vel_))));
					double den = Traj_Data_.Max_Jerk * (Traj_Data_.End_Vel_ + Traj_Data_.Start_Vel_);
					Tj1_ = num / den;
				}
				//Out_[0] = Tj1;
				//Out_[1] = Ta;
				//Out_[2] = Tj2;
				//Out_[3] = Td;
				//Out_[4] = Tv;
				return true;
			}
		}
		//Out_[0] = Tj1;
		//Out_[1] = Ta;
		//Out_[2] = Tj2;
		//Out_[3] = Td;
		//Out_[4] = Tv;
		return true;
	}


}

