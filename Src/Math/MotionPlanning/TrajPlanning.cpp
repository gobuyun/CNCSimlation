//#include "stdafx.h"
#include <algorithm>
#include <iostream>
#include "TrajPlanning.h"


CNC::TrajPlanningMethod::TrajPlanningMethod()
{

}

CNC::TrajPlanningMethod::~TrajPlanningMethod()
{
}

void CNC::TrajPlanningMethod::Reset()
{
	Traj_Data_.Start_Pos_ = 0.0;
	Traj_Data_.End_Pos_ = 0.0;
	Traj_Data_.Start_Vel_ = 0.0;
	Traj_Data_.End_Vel_ = 0.0;
	Traj_Data_.Max_Vel = 0.0;
	Traj_Data_.Max_ACC = 0.0;
	Traj_Data_.Max_DEC = 0.0;
	Traj_Data_.Max_Jerk = 0.0;

	Traj_Data_.Traj_Time_ = 0.0;
	Traj_Data_.Online_Ts_ = 0.0;
	Traj_Data_.Ta_ = 0.0;
	Traj_Data_.Td_ = 0.0;
	Traj_Data_.Tv_ = 0.0;
	Traj_Data_.Limit_Vel_ = 0.0;

	Traj_Data_.PosData.clear();
	Traj_Data_.VelData.clear();
	Traj_Data_.AccData.clear();
	Traj_Data_.JerkData.clear();

	Traj_Data_.Is_Generated_ = false;
	Traj_Data_.Is_Traj_End_ = false;
}

void CNC::TrajPlanningMethod::SetTargetPos(double Start_, double End_)
{
	Traj_Data_.Start_Pos_ = Start_;
	Traj_Data_.End_Pos_ = End_;
}

void CNC::TrajPlanningMethod::SetTargetVel(double Start_, double End_)
{
	Traj_Data_.Start_Vel_ = Start_;
	Traj_Data_.End_Vel_ = End_;
}

void CNC::TrajPlanningMethod::SetLimitVel(double Vmax_, double Amax_)
{
	Traj_Data_.Max_Vel = Vmax_;
	Traj_Data_.Max_ACC = Amax_;
	Traj_Data_.Max_DEC = -Traj_Data_.Max_ACC;
}
void CNC::TrajPlanningMethod::SetLimitVel(double Vmax_, double Amax_, double Jmax_)
{
	Traj_Data_.Max_Vel = Vmax_;
	Traj_Data_.Max_ACC = Amax_;
    Traj_Data_.Max_DEC = -Traj_Data_.Max_ACC;
    Traj_Data_.Max_Jerk = Jmax_;
}

bool CNC::TrajPlanningMethod::OnTaPart() const
{
    return Traj_Data_.Online_Ts_<=Traj_Data_.Ta_;
}

bool CNC::TrajPlanningMethod::OnTaTvPart() const
{
    return (Traj_Data_.Online_Ts_<=(Traj_Data_.Ta_ + Traj_Data_.Tv_) && Traj_Data_.Online_Ts_>Traj_Data_.Ta_);
}

bool CNC::TrajPlanningMethod::OnTaTvTdPart() const
{
    return (Traj_Data_.Online_Ts_>(Traj_Data_.Ta_ + Traj_Data_.Tv_) && Traj_Data_.Online_Ts_<=Traj_Data_.Ta_ + Traj_Data_.Tv_ + Traj_Data_.Td_);
}

CNC::DataInfo CNC::TrajPlanningMethod::GetMaxData(std::vector<double> Data_)
{
	DataInfo Temp;
	std::vector<double>::iterator itMax = std::max_element(Data_.begin(), Data_.end());
	Temp.Index_ = std::distance(Data_.begin(), itMax);
	Temp.Value_ = *itMax;
	return Temp;
}

CNC::DataInfo CNC::TrajPlanningMethod::GetMinData(std::vector<double> Data_)
{
	DataInfo Temp;
	std::vector<double>::iterator itMax = std::min_element(Data_.begin(), Data_.end());
	Temp.Index_ = std::distance(Data_.begin(), itMax);
	Temp.Value_ = *itMax;
	return Temp;
}
