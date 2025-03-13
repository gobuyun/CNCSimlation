#include <math.h>
#include "SampleJog.h"
#include "MathDataDefine.h"

CNC::SampleJog::SampleJog():LowerJogVel(0.001),LowerJogAcc(0.01)
{
	SampleJogActive = 0;
	SampleJogSimSpeed = 0.0;
	SampleJogSimPos = 0.0;
    SampleJogVel = LowerJogVel;
    SampleJogAcc = LowerJogAcc;
	IDLELower = 0.0;
	IDLEUpper = 0.0;
}

CNC::SampleJog::~SampleJog()
{
}

void CNC::SampleJog::StartJog(double Period)
{
	double Vel_temp = 0.0;

	if (SampleJogActive > 0)//forward
	{

		if (SampleJogSimSpeed >= SampleJogVel + DoubleErr)//到达额定速度
		{
			SampleJogSimPos = SampleJogSimSpeed * Period;//位置积分
		}
		else
		{
			//计算每一个周期应该达到的速度
			Vel_temp = (SampleJogSimSpeed + SampleJogAcc * Period);

			if (Vel_temp >= SampleJogVel + DoubleErr)
				Vel_temp = SampleJogVel;// + PhysicsAxis->DataUpper;
			SampleJogSimSpeed = Vel_temp;

			SampleJogSimPos = SampleJogSimSpeed * Period;

		}
	}
	else if (SampleJogActive < 0)//reverse
	{
		if (fabs(SampleJogSimSpeed) >= SampleJogVel + DoubleErr)//到达额定速度
		{
			SampleJogSimPos = SampleJogSimSpeed * Period;//位置积分
		}
		else
		{
			Vel_temp = SampleJogSimSpeed - SampleJogAcc * Period;
			if (fabs(Vel_temp) >= SampleJogVel + DoubleErr)
				Vel_temp = -SampleJogVel;// + PhysicsAxis->DataLower;
			SampleJogSimSpeed = Vel_temp;

			SampleJogSimPos = SampleJogSimSpeed * Period;

		}
	}
	else// wait Idle
	{
		if (SampleJogSimSpeed - IDLELower > -DoubleErr && SampleJogSimSpeed - IDLEUpper < DoubleErr)
		{
			//printf("======== 速度为0 ======== \n");
			SampleJogSimSpeed = 0.0;
			SampleJogSimPos = SampleJogSimSpeed * Period;
		}
		else
		{
			//printf("======== 速度不为0 ======== \n");
			//当速度不为零则以当前设定的减速度减速到0
			if (SampleJogSimSpeed >= DoubleErr) //在正转的时候
			{
				Vel_temp = SampleJogSimSpeed - SampleJogAcc * Period;
				if (Vel_temp <= DoubleErr)
					Vel_temp = 0.0;
				SampleJogSimSpeed = Vel_temp;

				SampleJogSimPos = SampleJogSimSpeed * Period;
			}
			else if (SampleJogSimSpeed <= -DoubleErr) //在反转的时候
			{
				Vel_temp = SampleJogSimSpeed + SampleJogAcc * Period;
				if (Vel_temp >= DoubleErr)
					Vel_temp = 0.0;
				SampleJogSimSpeed = Vel_temp;

				SampleJogSimPos = SampleJogSimSpeed * Period;

			}
		}
	}
}

void CNC::SampleJog::SetIDLERange(double Min_, double Max_)
{
	IDLELower = Min_;
	IDLEUpper = Max_;
}

bool CNC::SampleJog::SetMotionData(double Vel_, double Acc_)
{
	if (Vel_ <= 0 || Acc_ <= 0)
	{
		//默认值必须可以减速
        SampleJogVel = LowerJogVel;
        SampleJogAcc = LowerJogAcc;
		return false;
	}
		
	SampleJogVel = Vel_;
	SampleJogAcc = Acc_;

	return true;
}

double CNC::SampleJog::GetRunningSpeed() const
{
	return SampleJogSimSpeed;
}

double CNC::SampleJog::GetRunningPos() const
{
	return SampleJogSimPos;
}

//double CNC::SampleJog::GetJogSpeed() const
//{
//	return SampleJogVel;
//}
//
//double CNC::SampleJog::GetJogAcc() const
//{
//	return SampleJogAcc;
//}

void CNC::SampleJog::SyncSpeed(double Speed_)
{
	SampleJogSimSpeed = Speed_;
}


