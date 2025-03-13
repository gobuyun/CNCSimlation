#pragma once

#include <stdint.h>
#include "../MotionCommon/ErrMsg.h"

namespace CNC
{
	//伺服的数据类型全部以脉冲为基础单位
	//可以除以编码器位数转化为deg

class BaseMotorData :public ErrMsg
	{
		
	public:
		BaseMotorData(bool IsVirtual_);
		~BaseMotorData();
		//从上层下指令都是mm/s 或者 deg/s 当导程和减速比为1的时候，单位不需要转化
		//bool SetSpeedData(double Jerk_, double ACC_, double DEC_, double Vel_);
		//bool SetMaxSpeedData(double MaxJerk_, double MaxACC_, double MaxDEC_, double MaxVel_);
		void SetEncoderData(unsigned int EncoderUnit_, unsigned int EncoderType_);//必须设置编码器位数和类型
		void SetEncoderOffset(double EncoderOffset_);
		void SetFBData(int EncoderFB_,double VelFB_, double TorqueFB_);
		void SetFBData(double PosFB_Now_, double VelFB_);//给虚拟速度用的
		void SetTargetPos(double Pos_);
		bool SetControlMode(unsigned int Mode_);
		unsigned int GetControlMode() const;
		//获取设定的一些速度参数
		double &GetJerk();//规划加加速度			 单位rps^3
		double &GetACC();//规划加速度			     单位rps^2
		double &GetDEC();//规划减速度			     单位rps^2
		double &GetVel();//规划速度			     单位rps
		double &GetSimData();//给虚拟速度用的
		int &GetPosCmd();//位置命令				 单位脉冲数
		unsigned int GetEncoderUnit() const;	//			 单位脉冲数
        void encchange(int data);

		//获取反馈的值
		int GetEncoderFB() const;//周期脉冲值				 单位脉冲数
		double GetPosFB() const;//根据脉冲值计算出当前角度值  单位deg
		double GetVelFB() const;//脉冲变化每秒			     单位rps
		int GetIsIDLE() const;/* -1 反转 0静止 1正转*/
		//int GetErrorNum() const;

		//刷新Motor的状态，需要调用定时刷新
		void UpdateMotorData();
		void ResetState();

		//获取限位信息
		void SetFSLimit(double FS_Limit_);
		bool GetIsFSLimit();

		void SetRSLimit(double RS_Limit_);
		bool GetIsRSLimit();

		void SetFELimit(double FE_Limit_);
		//bool GetIsFELimit();

		double PosScaleChange(double Data_);  //根据机械结构进行单位转换
		double SpeedScaleChange(double Data_);//根据机械结构进行单位转换
		double UnitsScaleChange(double Data_);//根据机械结构进行单位转换

		bool Defpos(double Pos_);//传入绝对位置将显示位置归零 mm or deg 
		bool CalibrateAbsPos(double Pos_);//矫正绝对值编码器位置
		void PosOption(double Min_, double Max_);//连续位置上下限

		struct InputData
		{
			//信号On/Off
			bool FWD_IN;
			bool REV_IN;
			bool DATUM_IN;
			bool FWD_JOG;
			bool REV_JOG;
			bool FWD_MOVE;
			bool REV_MOVE;
			bool Z_Pluse;
			bool IsEnable;//使能
			int IsActive;//运动启用 0 不动 >0正转 <0 反转
			bool IsError;//上层与下层通讯的故障标识
			bool IsBlockLimit;//true 解除限位限制  false 启用
			bool IsFELimit;
			bool IsOverSpeed;
		}MotorInputData;

		double DataUpper, DataLower;//判断数据的上下限

		double Pitch;//导程
		double Ratio;//减速比
		unsigned int MoveType;
		//是否位置重置 上电默认重置一次
		bool IsDefpos;
        //无限旋转轴
        bool InfiniteMode;
        //最短运动路径模式
        bool OptimalPathMode;
	private:
		//输出位置锁定上下限
		double PosOptionMin;
		double PosOptionMax;
		bool UsingPosOption;

		/* -1 反转 0静止 1正传*/
		int IsIDLE;
		//虚拟轴
		bool IsVirtual;
		//运行圈数计算
		int MotorCycleCount;
		//上电初始化设定
		bool IsPowerOn;
		//根据脉冲变化判断静止
		bool EncIDLE;
		//计算连续位置
		int64_t pos_code_now;

		struct MotionData
		{

			double Jerk;
			double MaxJerk;

			double ACC;
			double MaxACC;

			double DEC;
			double MaxDEC;

			double Vel;//速度命令 当前类不具备控制命令功能，需要继承在外部实现
			double MaxVel;//单位 rpm
			double VelFB;//速度反馈 脉冲/s 需要转换为deg/s
			double VelSim;//模拟的速度

			double PosOffset;//位置偏移矫正
			double PosFBRaw;//编码器当前在几分之几圈 单位 deg
			double PosFBNow;//编码器位置反馈 单位 deg
			double PosFBLast;//上一个周期期望的位置 deg
			int PosCMD;//下发编码器位置的增量值  当前类不具备控制命令功能，需要继承在外部实现

			double TorqueFB;//扭矩反馈
			double TorqueCMD;//当前类不具备控制命令功能，需要继承在外部实现

			int EncoderFB;//脉冲值反馈
			int EncoderFBLast;//上一个周期脉冲值反馈
			unsigned int EncoderUnit;//编码器位数
			double EncoderOffset;//绝对值编码器偏移原点
			unsigned int EncoderType;//编码器类型		 
			unsigned int ControlMode;//控制模式
		}MotorMotionData;

		struct LimitData
		{
			double FSLimit;
			bool IsFSLimit;
			double RSLimit;
			bool IsRSLimit;
			double FELimit;
		}MotorLimitData;


		void UpdatePos(const unsigned int *EncoderUnit_);
		void ContinuePos(int EncoderFB_);
		int64_t CalCycleCount();
	};

}
