#pragma once


namespace CNC
{


class SampleJog
{
public:
	SampleJog();
	~SampleJog();

	void StartJog(double Period);
	void SetIDLERange(double Min_,double Max_);
	bool SetMotionData(double Vel_,double Acc_);
	double GetRunningSpeed() const;
	double GetRunningPos() const;
	//double GetJogSpeed() const;
	//double GetJogAcc() const;
	int SampleJogActive;
	//调用Jog前需要同步规划速度
	void SyncSpeed(double Speed_);
private:
	//点动模拟速度
	double SampleJogSimSpeed;
	//点动积分位置
	double SampleJogSimPos;
	double SampleJogVel;
	double SampleJogAcc;
	double IDLELower, IDLEUpper;
    const double LowerJogVel,LowerJogAcc;

};


}
