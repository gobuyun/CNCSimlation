#pragma once
#include <vector>


namespace CNC
{

struct DataInfo
{

	int Index_;
	double Value_;

	DataInfo()
	{
		Index_ = 0;
		Value_ = 0.0;
	}

};

class TrajPlanningMethod
{
public:
	TrajPlanningMethod();
	virtual ~TrajPlanningMethod();

	void Reset();//参数复位

	void SetTargetPos(double Start_, double End_);//设定位置参数
	void SetTargetVel(double Start_, double End_);//设定速度参数
	void SetLimitVel(double Vmax_, double Amax_);//设定最大限制参数
	void SetLimitVel(double Vmax_, double Amax_,double Jmax_);//设定最大限制参数

	virtual bool InitialTrajData() = 0;//初始化轨迹数据 检查是否可以生成轨迹
	virtual bool GenerateACCData(double Period_, double &Acc_) = 0;//生成加速度数据
	virtual bool GenerateVelData(double Period_, double &Vel_) = 0;//生成速度数据
	virtual bool GeneratePosData(double Period_, double &Pos_) = 0;//生成位置数据
    virtual bool OnlinePlanning(double Period_, double &Pos_, double &Vel_, double &Acc_) = 0;
    bool OnTaPart() const;
    bool OnTaTvPart() const;
    bool OnTaTvTdPart() const;
	//通用轨迹数据
	struct TrajData
	{

		bool Is_Generated_; //轨迹是否生成
		bool Is_Traj_End_; //轨迹是否更新完毕 用于实时规划
		double Start_Pos_; //初始位置
		double End_Pos_;//终止位置
		double Start_Vel_;//起始速度
		double End_Vel_;//终止速度
		double Max_Vel;//最大速度
		double Max_ACC;//最大加速度
		double Max_DEC;//最大减速度
		double Max_Jerk;//最大的加加速度

		double Traj_Time_;//整段轨迹花费的时间
		double Online_Ts_;//实时规划当前时间
		double Ta_; //加速阶段持续的总时间
		double Td_;//减速阶段持续的总时间
		double Tv_;//匀速阶段持续的时间
		double Limit_Vel_;//整段轨迹能够达到的最大速度
		double Limit_ACC_;//整段轨迹能够达到的最大加速度

		std::vector<double> PosData;
		std::vector<double> VelData;
		std::vector<double> AccData;
		std::vector<double> JerkData;

		TrajData()
		{
			Is_Generated_ = false;
			Is_Traj_End_ = false;
			Start_Pos_ = 0.0;
			End_Pos_ = 0.0;
			Start_Vel_ = 0.0;
			End_Vel_ = 0.0;
			Max_Vel = 0.0;
			Max_ACC = 0.0;
			Max_DEC = 0.0;
			Max_Jerk = 0.0;

			Traj_Time_ = 0.0;
			Online_Ts_ = 0.0;
			Ta_ = 0.0;
			Td_ = 0.0;
			Tv_ = 0.0;
			Limit_Vel_ = 0.0;

			PosData.clear();
			VelData.clear();
			AccData.clear();
			JerkData.clear();
		}
	}Traj_Data_;


private:
	DataInfo GetMaxData(std::vector<double> Data_);
	DataInfo GetMinData(std::vector<double> Data_);

};

}

