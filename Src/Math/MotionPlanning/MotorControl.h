#pragma once


#include "Motor.h"
#include "PosGenerator.h"

namespace CNC
{

class MotorControl
{
public:
    typedef std::shared_ptr<MotorControl> Ptr;

	static int AxisNumCount;

	enum CommandMode
	{
		COMMAND_MOTOR_NONE = 0,
		COMMAND_MOTOR_JOG,
		COMMAND_MOTOR_HOMING,
		COMMAND_MOTOR_WHEELJOG,
		COMMAND_MOTOR_SMOVE,
		COMMAND_MOTOR_TMOVE,
		COMMAND_INCREMENT_TRAJ,
		COMMAND_ABSOLUTE_TRAJ,
		COMMAND_MOTOR_RAPIDSTOP
	};


	MotorControl(unsigned int EncoderUnit_, unsigned int EncoderType_, unsigned int Move_Type_/*直线或旋转 */, bool Virtral_,double Ratio_, double Pitch_ = 1.0);
    MotorControl(const MotorControl& in);
    ~MotorControl();

	bool SetSpeedData(double Jerk_, double ACC_, double DEC_, double Vel_);
	bool SetMaxSpeedData(double MaxJerk_, double MaxACC_, double MaxDEC_, double MaxVel_);
	bool SetJogSpeedData(double ACC_, double Vel_);

	void SetFSLimit(double FS_Limit_);
	void SetRSLimit(double RS_Limit_);
	void SetFELimit(double FE_Limit_);
    //当编码器数值无限疯转容易引起数值溢出，例如主轴，应当选用此模式
	void SetPosRange(double Min_,double Max_);
    //设定无限旋转轴显示数值 正常旋转轴无限旋转模式，圈数不会太多选择此模式
    void SetInfRange(double Min_, double Max_);
	//根据机械参数计算的位置 mm
	double GetPos() const;
	//根据机械参数计算的速度 mm/s
	double GetSpeed() const;

	void MainControlLogic(double Period_);

	void Reset();

	void SetCommandMode(CommandMode Mode_);
	//CSV模式下正反转
	bool MoveForward();
	bool MoveReverse();

	//更新模拟速度 通过规划轨迹下发速度
	void UpdateSimSpeed(double Vel_);
	//设定手轮模式 运行静止才能切换
	bool GetIsWheelJog() const;
	bool ActiveWheelJog(bool Active_);

	std::shared_ptr<BaseMotorData> PhysicsAxis;
	std::shared_ptr<PosGenerator> PosGenera;


	//是否已回原点
	bool IsHomed;
	//模拟速度或真实速度执行
	bool SimSpeedMode;
	//虚拟轴
	bool IsVitrualAxis;

	CommandMode commandMode;

private:
	//手轮模式状态
	bool IsWheelJog;
	//超速故障
	bool OverSpeed;
	//初始化设定参数
	bool Initial;
	//存储小数部分命令
	double ResCmd;
	//当下目标位置  必须是增量位置
	double TargetPos;
	//上周期目标位置
	double LastCmdPos;
	//故障停止
	void RapidStop();
	//点动控制输入
	void SetJogInput();
	//位置和脉冲的比例转化
	void PosCmd2Pluse(double Cmd_, int &PosCMD_);
	//绝对位置和脉冲的比例转化
	void PosAbsCmd2Pluse(double Cmd_, int &PosCMD_);
	//按照设定速度的脉冲增量
	void SpeedCmd2Pluse(double SpeedCmd_, int &PosCMD_, double Period_);
	//根据当前速度预估是否会碰限位 是则马上减速
	void EstimateLimit();
	//模拟运动仿真 输入脉冲
	void MoveSimulate(int Cmd_, double Period_);
	//更新模拟速度 通过脉冲计算出速度
	void UpdateSimSpeed(int Cmd_, double Period_);


};

}


