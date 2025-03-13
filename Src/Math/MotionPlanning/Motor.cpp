
#include <math.h>
#include <limits.h>
#include "MathDataDefine.h"
#include "Motor.h"
#include <iostream>

CNC::BaseMotorData::BaseMotorData(bool IsVirtual_)
{
	MotorMotionData.Jerk = 0;
	MotorMotionData.MaxJerk = 0;

	MotorMotionData.ACC = 0;
	MotorMotionData.MaxACC = 0;

	MotorMotionData.DEC = 0;
	MotorMotionData.MaxDEC = 0;

	MotorMotionData.Vel = 0;
	MotorMotionData.MaxVel = 0;
	MotorMotionData.VelFB = 0;
	MotorMotionData.VelSim = 0;

	MotorMotionData.PosOffset = 0;
	MotorMotionData.PosFBRaw = 0;
	MotorMotionData.PosFBNow = 0;
	MotorMotionData.PosFBLast = 0;
	MotorMotionData.PosCMD = 0;

	MotorMotionData.TorqueFB = 0;
	MotorMotionData.TorqueCMD = 0;

    MotorMotionData.EncoderFB = 0;
	MotorMotionData.EncoderFBLast = 0;
	MotorMotionData.EncoderUnit = 0;
	MotorMotionData.EncoderOffset = 0;
	MotorMotionData.EncoderType = 0;
	MotorMotionData.ControlMode = MODE_CSP;

	MotorLimitData.FSLimit = 0;
	MotorLimitData.RSLimit = 0;
	MotorLimitData.FELimit = 0;
	MotorLimitData.IsFSLimit = false;
	MotorLimitData.IsRSLimit = false;


	MotorInputData.FWD_IN = OFF;
	MotorInputData.REV_IN = OFF;
	MotorInputData.DATUM_IN = OFF;
	MotorInputData.FWD_JOG = OFF;
	MotorInputData.REV_JOG = OFF;
	MotorInputData.FWD_MOVE = OFF;
	MotorInputData.REV_MOVE = OFF;
	MotorInputData.Z_Pluse = OFF;
	MotorInputData.IsEnable = OFF;
	MotorInputData.IsError = OFF;
	MotorInputData.IsActive = OFF;
	MotorInputData.IsBlockLimit = OFF;
	MotorInputData.IsOverSpeed = OFF;
	MotorInputData.IsFELimit = OFF;

	MotorCycleCount = 0;

	IsVirtual = IsVirtual_;

	Pitch = 1;
	Ratio = 1;

	IsPowerOn = false;
	IsDefpos = true;

	UsingPosOption = false;
	PosOptionMin = -10000000;
	PosOptionMax = 100000000;

    InfiniteMode=false;
    OptimalPathMode=false;

    IsIDLE = 0;

}

CNC::BaseMotorData::~BaseMotorData()
{
}

bool CNC::BaseMotorData::SetControlMode(unsigned int Mode_)
{
	if (MoveType != ROTATE)
	{
		SetErrNum(ERROR_MSG_DATA_SET, PARA_CMODE);
		return false;
	}
	if (Mode_ > MODE_CST || Mode_< MODE_CSP)
	{
		SetErrNum(ERROR_MSG_DATA_SET, PARA_CMODE);
		return false;
	}
	MotorMotionData.ControlMode = Mode_;
	return true;
}

unsigned int CNC::BaseMotorData::GetControlMode() const
{
	return MotorMotionData.ControlMode;
}

void CNC::BaseMotorData::SetFSLimit(double FS_Limit_)
{
	MotorLimitData.FSLimit = FS_Limit_;
}

bool CNC::BaseMotorData::GetIsFSLimit()
{
	return MotorLimitData.IsFSLimit;;
}

void CNC::BaseMotorData::SetRSLimit(double RS_Limit_)
{
	MotorLimitData.RSLimit = RS_Limit_;
}

bool CNC::BaseMotorData::GetIsRSLimit()
{
	return MotorLimitData.IsRSLimit;;
}

void CNC::BaseMotorData::SetFELimit(double FE_Limit_)
{
	MotorLimitData.FELimit = FE_Limit_;
}

//bool CNC::BaseMotorData::GetIsFELimit()
//{
//	return MotorLimitData.IsFELimit;
//}

double CNC::BaseMotorData::PosScaleChange(double Data_)
{
	if (MoveType == LINEAR)
		return (Data_ * Ratio) / Pitch * 360.0; //deg
	if (MoveType == ROTATE)
		return ((Data_ * Ratio) / Pitch); //vel = deg/s
		
	return 0.0;
}

double CNC::BaseMotorData::SpeedScaleChange(double Data_)
{
	if (MoveType == LINEAR)
		return (Data_ * Ratio) / Pitch; //vel = rps   == mm/s
	if (MoveType == ROTATE)
		return ((Data_ * Ratio) / Pitch) / 360.0; //vel = deg/s

	return 0.0;
}

double CNC::BaseMotorData::UnitsScaleChange(double Data_)
{
	return ((Data_ * Ratio) / Pitch);
}

bool CNC::BaseMotorData::Defpos(double Pos_)
{
	if (IsIDLE == 0)
	{
		if (MoveType == LINEAR)
			MotorMotionData.PosOffset += -MotorMotionData.PosFBNow + (Pos_ / Pitch) * Ratio * 360.0;
		if (MoveType == ROTATE)
			MotorMotionData.PosOffset += -MotorMotionData.PosFBNow + (Pos_ / Pitch) * Ratio;

		IsDefpos = true;
		return true;
	}
	else
	{
		SetErrNum(ERROR_MSG_UNIDLE, PARA_NONE);
		return false;
	}

	
}

bool CNC::BaseMotorData::CalibrateAbsPos(double Pos_)
{
	if (IsIDLE == 0)
	{
		if (MoveType == LINEAR)
			MotorMotionData.PosOffset += (Pos_ / Pitch) * Ratio * 360.0;
		if (MoveType == ROTATE)
			MotorMotionData.PosOffset += (Pos_ / Pitch) * Ratio;

		IsDefpos = true;
		return true;
	}
	else
	{
		SetErrNum(ERROR_MSG_UNIDLE, PARA_NONE);
		return false;
	}

	
}

void CNC::BaseMotorData::PosOption(double Min_, double Max_)
{
	PosOptionMin = Min_;
	PosOptionMax = Max_;
	UsingPosOption = true;
}

void CNC::BaseMotorData::UpdatePos(const unsigned int *EncoderUnit_)
{
	if (*EncoderUnit_ < pow(2,5))
	{
		//ErrorNum = ERROR_MSG_DATA_SET;
		SetErrNum(ERROR_MSG_DATA_SET, PARA_ENC_UNIT);
        printf("[ENC UNIT = %d]",*EncoderUnit_);
		return;
	}
	//MotorMotionData.PosFBLast = MotorMotionData.PosFBNow ;
	
	MotorMotionData.PosFBNow = MotorMotionData.PosFBRaw * 360.0 + MotorMotionData.PosOffset;//deg or mm

	if (UsingPosOption)
	{
        double delta = PosOptionMax-PosOptionMin;

        if (MotorMotionData.PosFBNow > PosOptionMax)
        {
//			double temp = MotorMotionData.PosFBNow - PosOptionMax;
//			MotorMotionData.PosFBNow = 0;
//			MotorMotionData.PosFBNow = temp + PosOptionMin;
            MotorMotionData.PosFBNow -= delta*static_cast<int64_t>((MotorMotionData.PosFBNow-PosOptionMin)/delta);

            pos_code_now = CalCycleCount();
            IsDefpos = true;
        }
        if (MotorMotionData.PosFBNow < PosOptionMin)
        {
//			double temp = MotorMotionData.PosFBNow - PosOptionMin;
//			MotorMotionData.PosFBNow = 0;
//			MotorMotionData.PosFBNow = temp + PosOptionMax;
            MotorMotionData.PosFBNow += delta*static_cast<int64_t>((PosOptionMax-MotorMotionData.PosFBNow)/delta);
            pos_code_now = CalCycleCount();
            IsDefpos = true;
        }



    }


	if (IsDefpos)
	{
		MotorMotionData.PosFBLast = MotorMotionData.PosFBNow;
	}
}

void CNC::BaseMotorData::ContinuePos(int EncoderFB_)
{
	if (!IsPowerOn)
	{
		pos_code_now = (int64_t)EncoderFB_;
		MotorMotionData.EncoderFBLast = EncoderFB_;
		IsPowerOn = true;
	}

	int64_t judge_tmp = (int64_t)EncoderFB_ - (int64_t)MotorMotionData.EncoderFBLast;

	if (abs(judge_tmp)>INT_MAX)
	{
		int64_t delta_code = 2 * (int64_t)INT_MAX - abs((int64_t)EncoderFB_) - abs((int64_t)MotorMotionData.EncoderFBLast) + 2;
		int sign = (judge_tmp>0) - (judge_tmp<0);
		pos_code_now = pos_code_now - sign * delta_code;
	}
	else
	{
		pos_code_now = pos_code_now + judge_tmp;
	}

	MotorMotionData.PosFBRaw = ((double)pos_code_now) / MotorMotionData.EncoderUnit;
	MotorMotionData.EncoderFBLast = EncoderFB_;

//	if (abs(MotorMotionData.EncoderFB - MotorMotionData.EncoderFBLast) + DoubleErr <= 10)
//		EncIDLE = true;
//	else
//		EncIDLE = false;
    EncIDLE = true;

    MotorMotionData.EncoderFBLast = EncoderFB_;//记录上一次脉冲值

}

int64_t CNC::BaseMotorData::CalCycleCount()
{
	double raw = (MotorMotionData.PosFBNow - MotorMotionData.PosOffset) / 360.0;

	return int64_t(raw* MotorMotionData.EncoderUnit);
}

void CNC::BaseMotorData::UpdateMotorData()
{
	ContinuePos(MotorMotionData.EncoderFB);
	//ContinuePos(MotorMotionData.EncoderFB + MotorMotionData.EncoderOffset, &MotorMotionData.EncoderFBLast, MotorMotionData.EncoderUnit, &MotorMotionData.PosFBRaw, IsIDLE, &MotorCycleCount);
	UpdatePos(&MotorMotionData.EncoderUnit);

    //csv模式下不需要限位检测
    if (MotorMotionData.ControlMode == MODE_CSV || InfiniteMode)
	{
		MotorInputData.IsBlockLimit = true;
    }

    if (MotorMotionData.VelSim - DataLower > -DoubleErr && MotorMotionData.VelSim - DataUpper < DoubleErr
        && EncIDLE && fabs(MotorMotionData.VelFB) < 0.001)//rps < 0.01
	{
		IsIDLE = 0;
    }
	else
	{
		if (MotorMotionData.VelSim + DoubleErr > DataUpper)
		{
			IsIDLE = 1;
		}
		else if (MotorMotionData.VelSim + DoubleErr < DataLower)
		{
			IsIDLE = -1;
		}
	}

	if (!MotorInputData.IsEnable)
	{
		SetErrNum(ERROR_MSG_NOT_ENABLE, PARA_INPUT);
	}
	// LimitData Update
	if (MotorLimitData.FSLimit + DoubleErr <= MotorMotionData.PosFBNow && !MotorInputData.IsBlockLimit)
	{
		MotorLimitData.IsFSLimit = true;
	}
	else
	{
		MotorLimitData.IsFSLimit = false;
	}
	if (MotorLimitData.RSLimit + DoubleErr >= MotorMotionData.PosFBNow && !MotorInputData.IsBlockLimit)
	{
		MotorLimitData.IsRSLimit = true;
	}
	else
	{
		MotorLimitData.IsRSLimit = false;
	}

	if (IsIDLE > 0)
	{
		if (MotorLimitData.IsFSLimit)
		{
			SetErrNum(ERROR_MSG_FS_WARN, PARA_NONE);
		}
	}
	if (IsIDLE < 0)
	{
		if (MotorLimitData.IsRSLimit)
		{
			SetErrNum(ERROR_MSG_RS_WARN, PARA_NONE);
		}
	}
	//if (IsIDLE != 0)
	//{
	//	//printf("FE = %f\n", MotorMotionData.PosFBNow - MotorMotionData.PosFBLast);
	//	if ((fabs(MotorMotionData.PosFBNow - MotorMotionData.PosFBLast) + DoubleErr) > MotorLimitData.FELimit)
	//	{
	//		MotorLimitData.IsFELimit = true;
	//		//ErrorNum = ERROR_MSG_FE_WARN;
	//		SetErrNum(ERROR_MSG_FE_WARN, PARA_NONE);
	//	}
	//	else
	//	{
	//		MotorLimitData.IsFELimit = false;
	//	}

	//}
	if (MotorInputData.FWD_IN == ON && !MotorInputData.IsBlockLimit)
	{
		SetErrNum(ERROR_MSG_FWD_WARN, PARA_INPUT);
	}
	if (MotorInputData.REV_IN == ON && !MotorInputData.IsBlockLimit)
	{
		SetErrNum(ERROR_MSG_REV_WARN, PARA_INPUT);
	}
	if (MotorInputData.IsError)
	{
		SetErrNum(ERROR_MSG_MOTION, PARA_NONE);
	}
	if (MotorMotionData.ControlMode == MODE_CSP)
	{
//		if (MotorInputData.IsFELimit)
//		{
//			SetErrNum(ERROR_MSG_FE_WARN, PARA_NONE);
//		}
//		if (MotorInputData.IsOverSpeed)
//		{
//			SetErrNum(ERROR_MSG_DANGER_SPEED, PARA_NONE);
//		}
	}

}

void CNC::BaseMotorData::ResetState()
{
	MotorInputData.IsError = false;
	ResetErrNum();
}

void CNC::BaseMotorData::SetEncoderData(unsigned int EncoderUnit_, unsigned int EncoderType_)
{
	MotorMotionData.EncoderUnit = EncoderUnit_;
	MotorMotionData.EncoderType = EncoderType_;

}

void CNC::BaseMotorData::SetEncoderOffset(double EncoderOffset_)
{
	MotorMotionData.EncoderOffset = EncoderOffset_;
}

void CNC::BaseMotorData::SetFBData(int EncoderFB_, double VelFB_, double TorqueFB_)
{
	MotorMotionData.EncoderFB = EncoderFB_;
	MotorMotionData.VelFB = VelFB_;
	MotorMotionData.TorqueFB = TorqueFB_;
}

void CNC::BaseMotorData::SetFBData(double PosFB_Now_, double VelFB_)
{
    //只有虚拟轴可以直接下发位置反馈
	if (IsVirtual)
	{
        MotorMotionData.PosFBNow += PosFB_Now_;//位置累计增量
		MotorMotionData.VelFB = VelFB_;
	}

}

void CNC::BaseMotorData::SetTargetPos(double Pos_)
{
	MotorMotionData.PosFBLast = Pos_;
}

double &CNC::BaseMotorData::GetSimData()
{
	return MotorMotionData.VelSim;
}

double &CNC::BaseMotorData::GetJerk()
{
	return MotorMotionData.Jerk;
}

double &CNC::BaseMotorData::GetACC()
{
	return MotorMotionData.ACC;
}

double &CNC::BaseMotorData::GetDEC()
{
	return MotorMotionData.DEC;
}

double &CNC::BaseMotorData::GetVel()
{
	return MotorMotionData.Vel;
}

int &CNC::BaseMotorData::GetPosCmd()
{
	return MotorMotionData.PosCMD;
}

unsigned int CNC::BaseMotorData::GetEncoderUnit() const
{
    return MotorMotionData.EncoderUnit;
}

void CNC::BaseMotorData::encchange(int data)
{
    MotorMotionData.EncoderFB=data;
}

int CNC::BaseMotorData::GetEncoderFB() const
{
	return MotorMotionData.EncoderFB;
}

double CNC::BaseMotorData::GetPosFB() const
{
	return MotorMotionData.PosFBNow;
}

double CNC::BaseMotorData::GetVelFB() const
{
	return MotorMotionData.VelFB;
}

int CNC::BaseMotorData::GetIsIDLE() const
{
	return IsIDLE;
}


