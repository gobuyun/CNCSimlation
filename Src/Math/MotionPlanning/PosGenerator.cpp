#include <math.h>
#include "MathDataDefine.h"
#include "PosGenerator.h"


CNC::PosGenerator::PosGenerator()
{
    TPlanning = std::make_shared<TVelPlanning>();
    SPlanning = std::make_shared<SVelPlanning>();
    JogPlanning = std::make_shared<SampleJog>();

    MData.MaxJerk = 0.0;
    MData.MaxACC = 0.0;
    MData.MaxDEC = 0.0;
    MData.MaxVel = 0.0;

    MData.Jerk = 0.0;
    MData.ACC = 0.0;
    MData.DEC = 0.0;
    MData.Vel = 0.0;
    MData.JogAcc = 0.0;
    MData.JogVel = 0.0;

    MData.FSLimit = 0.0;
    MData.RSLimit = 0.0;
    MData.FELimit = 0.0;

    CmdPos = 0.0;
    LastCmdPos = 0.0;
    LastCmdSpeed = 0.0;
    LastCmdAcc = 0.0;
    Theoretical.Vel = 0.0;
    Theoretical.Acc = 0.0;
    Theoretical.Pos = 0.0;
    InterpolationPos = 0.0;

    delta=0.0;

    IsOverSpeed=false;
    IsFollowErr=false;

    PosOffset =0.0;

    DataOutPutMode = false;

    //输出位置锁定上下限
    PosOptionMin=0;
    PosOptionMax=0;
    UsingPosOption=false;

}

CNC::PosGenerator::~PosGenerator()
{
}

void CNC::PosGenerator::SyncActualData(double Vel_, double Pos_)
{
    Actual.Pos = Pos_+PosOffset;
    Actual.Vel = Vel_;
}

void CNC::PosGenerator::CancelMove()
{
    TPlanning->TReset();
    SPlanning->SReset();
}

bool CNC::PosGenerator::CheckMotionData()
{
    if (MData.ACC < DoubleErr || MData.Vel < DoubleErr)
    {
        SetErrNum(ERROR_MSG_ZEROSPEED, PARA_SPEED);
        printf("[Acc = %f Vel = %f]",MData.ACC,MData.Vel);
        return false;
    }

    if (MData.Jerk < DoubleErr || MData.ACC < DoubleErr || MData.DEC < DoubleErr || MData.Vel < DoubleErr)
    {
        SetErrNum(ERROR_MSG_DATA_SET, PARA_SPEED);
        printf("[Jerk = %f Acc = %f Vel = %f]",MData.Jerk,MData.ACC,MData.Vel);
        return false;
    }
    if (MData.MaxJerk < DoubleErr || MData.MaxACC < DoubleErr || MData.MaxDEC < DoubleErr || MData.MaxVel < DoubleErr)
    {
        SetErrNum(ERROR_MSG_DATA_SET, PARA_MAXSPEED);
        printf("[MaxJerk = %f MaxAcc = %f MaxVel = %f]",MData.MaxJerk,MData.MaxACC,MData.MaxVel);
        return false;
    }

    return true;
}

bool CNC::PosGenerator::SetSpeedData(double Jerk_, double ACC_, double DEC_, double Vel_)
{
    if (Jerk_ < 0 || ACC_ < 0 || DEC_ < 0 || Vel_ < 0)
    {
        SetErrNum(ERROR_MSG_DATA_SET, PARA_SPEED);
        printf("[Jerk = %f Acc = %f Vel = %f]",Jerk_,ACC_,Vel_);
        return false;
    }
    if (MData.MaxJerk <= 0 || MData.MaxACC <= 0 || MData.MaxDEC <= 0 || MData.MaxVel <= 0)
    {
        SetErrNum(ERROR_MSG_DATA_SET, PARA_MAXSPEED);
        printf("[Jerk = %f Acc = %f Vel = %f]",MData.MaxJerk,MData.MaxACC,MData.MaxVel);
        return false;
    }

    if (Jerk_ <= MData.MaxJerk)
        MData.Jerk = Jerk_;
    else
        MData.Jerk = MData.MaxJerk;
    if (ACC_ <= MData.MaxACC)
        MData.ACC = ACC_;
    else
        MData.ACC = MData.MaxACC;
    if (DEC_ <= MData.MaxDEC)
        MData.DEC = DEC_;
    else
        MData.DEC = MData.MaxDEC;
    if (Vel_ <= MData.MaxVel)
        MData.Vel = Vel_;
    else
        MData.Vel = MData.MaxVel;

    return true;
}

bool CNC::PosGenerator::SetMaxSpeedData(double MaxJerk_, double MaxACC_, double MaxDEC_, double MaxVel_)
{
    if (MaxJerk_ <= 0 || MaxACC_ <= 0 || MaxDEC_ <= 0 || MaxVel_ <= 0)
    {
        SetErrNum(ERROR_MSG_DATA_SET, PARA_MAXSPEED);
        printf("[Jerk = %f Acc = %f Vel = %f]",MaxJerk_,MaxACC_,MaxVel_);
        return false;
    }

    MData.MaxJerk = MaxJerk_;
    MData.MaxACC = MaxACC_;
    MData.MaxDEC = MaxDEC_;
    MData.MaxVel = MaxVel_;

    return true;
}

bool CNC::PosGenerator::SetJogSpeedData(double ACC_, double Vel_)
{
    if (ACC_< 0 ||Vel_< 0)
    {
        SetErrNum(ERROR_MSG_DATA_SET, PARA_SPEED);
        printf("[Acc = %f Vel = %f]",ACC_,Vel_);
        return false;
    }
    if (MData.MaxACC <= 0 || MData.MaxVel <= 0)
    {
        SetErrNum(ERROR_MSG_DATA_SET, PARA_MAXSPEED);
        printf("[Acc = %f Vel = %f]",MData.MaxACC,MData.MaxVel);
        return false;
    }

    if (ACC_ <= MData.MaxACC)
        MData.JogAcc = ACC_;
    else
        MData.JogAcc = MData.MaxACC;

    if (Vel_ <= MData.MaxVel)
        MData.JogVel = Vel_;
    else
        MData.JogVel = MData.MaxVel;

    JogPlanning->SetMotionData(MData.JogVel, MData.JogAcc);

    return true;
}

bool CNC::PosGenerator::CheckIsOverMaxSpeed(double Jerk_, double ACC_, double DEC_, double Vel_)
{
    if (Jerk_ <= MData.MaxJerk && ACC_ <= MData.MaxACC && DEC_ <= MData.MaxDEC && Vel_ <= MData.MaxVel)
    {
        return false;
    }
    else
    {
        SetErrNum(ERROR_MSG_SPEED_OVER, PARA_SPEED);
        return true;
    }

}

double CNC::PosGenerator::GetPos() const
{
    double Temp=0;
    if (DataOutPutMode)
        Temp = Actual.Pos;
    else
        Temp = Theoretical.Pos;

    if(UsingPosOption)
    {
        double RangeDelta = PosOptionMax-PosOptionMin;
        do {

            if(Temp>PosOptionMax)
            {
                Temp -= RangeDelta*static_cast<int64_t>((Temp-PosOptionMin)/RangeDelta);
            }
            else if(Temp<PosOptionMin)
            {
                Temp += RangeDelta*static_cast<int64_t>((PosOptionMax-Temp)/RangeDelta);
            }
        } while (!(Temp<=PosOptionMax && Temp>=PosOptionMin));

        return Temp;

    }
    else
    {
        return Temp;
    }

}

double CNC::PosGenerator::GetNoneTransPos() const
{
    if (DataOutPutMode)
        return Actual.Pos;
    else
        return Theoretical.Pos;
}

double CNC::PosGenerator::GetVel() const
{
    if (DataOutPutMode)
        return Actual.Vel;
    else
        return Theoretical.Vel;
}

double CNC::PosGenerator::GetAcc() const
{
    //    if (DataOutPutMode)
    //        return Actual.Acc;
    //    else
    return Theoretical.Acc;
}

double CNC::PosGenerator::GetJerk() const
{
    //    if (DataOutPutMode)
    //        return Actual.Jerk;
    //    else
    return Theoretical.Jerk;
}
double CNC::PosGenerator::GetPlanningSpeed() const
{
    return Theoretical.Vel;
}

double CNC::PosGenerator::GetInterpolationPos() const
{
    return InterpolationPos;
}

bool CNC::PosGenerator::GetTPlanningPos(double Period_,double &Pos_)
{

    //这里的结果是mm or deg
    if (!TPlanning->OnlinePlanning(Period_, CmdPos, Theoretical.Vel,Theoretical.Acc))
    {
        Theoretical.Jerk=0;
        delta = 0.0;
        return false;
    }
    CheckOverSpeed(CmdPos,Period_,true);

    // 转换为编码器角度值  返回的单位是角度值
    // 得出相对位置角度值
    Pos_ = CmdPos - LastCmdPos;
    delta = Pos_;

    LastCmdPos = CmdPos;

    //更新理论的绝对位置
    Theoretical.Pos = CmdPos;
    //更新记录的插补数据
    InterpolationPos = CmdPos;

    Theoretical.Jerk = (Theoretical.Acc-LastCmdAcc)/Period_;
    LastCmdAcc=Theoretical.Acc;

    return true;
}

bool CNC::PosGenerator::TTrajPlanning(double q0_, double q1_, double v0_, double v1_)
{
    TPlanning->TReset();
    ResetErrNum();
    if (q1_<MData.RSLimit || q1_>MData.FSLimit)
    {
        //超限位没必要运行了
        SetErrNum(ERROR_MSG_TRAJ_OVERLIMIT, PARA_LIMIT);
        return false;
    }
    if (!CheckMotionData())
    {
        return false;
    }
    if (CheckFE())
        return false;

    TPlanning->SetTargetPos(q0_, q1_);
    TPlanning->SetTargetVel(v0_, v1_);
    TPlanning->SetLimitVel(MData.Vel, MData.ACC);
    LastCmdPos = q0_;

    if (!TPlanning->InitialTrajData())
    {
        SetErrNum(ERROR_MSG_TRAJ_FAIL, PARA_SPEED);
        return false;
    }
    //设定临时减速参数
    JogPlanning->SetMotionData(MData.Vel, MData.ACC);
    return true;
}

bool CNC::PosGenerator::GetSPlanningPos(double Period_, double &Pos_)
{
    //这里的结果是mm or deg
    if (!SPlanning->OnlinePlanning(Period_, CmdPos, Theoretical.Vel,Theoretical.Acc))
    {
        Theoretical.Jerk=0;
        delta = 0.0;
        return false;
    }

    CheckOverSpeed(CmdPos,Period_,true);

    Pos_ = CmdPos - LastCmdPos;
    delta = Pos_;
    // 转换为编码器角度值  返回的单位是角度值
    LastCmdPos = CmdPos;

    //更新理论的绝对位置
    Theoretical.Pos = CmdPos;
    //更新记录的插补数据
    InterpolationPos = CmdPos;

    Theoretical.Jerk = (Theoretical.Acc-LastCmdAcc)/Period_;
    LastCmdAcc=Theoretical.Acc;

    return true;
}

bool CNC::PosGenerator::GetSPlanningAbsPos(double Period_, double &Pos_)
{

    if (!SPlanning->OnlinePlanning(Period_, CmdPos, Theoretical.Vel,Theoretical.Acc))
    {
        Theoretical.Jerk=0;
        delta = 0.0;
        return false;
    }

    CheckOverSpeed(CmdPos,Period_,true);
    //直接输出轨迹的绝对位置
    Pos_ = CmdPos;

    delta = CmdPos - LastCmdPos;
    LastCmdPos = CmdPos;

    //更新理论的绝对位置
    Theoretical.Pos = CmdPos;
    //更新记录的插补数据
    InterpolationPos = CmdPos;

    Theoretical.Jerk = (Theoretical.Acc-LastCmdAcc)/Period_;
    LastCmdAcc=Theoretical.Acc;

    return true;
}

bool CNC::PosGenerator::STrajPlanning(double q0_, double q1_, double v0_, double v1_)
{
    SPlanning->SReset();
    ResetErrNum();
    if (q1_<MData.RSLimit || q1_>MData.FSLimit)
    {
        SetErrNum(ERROR_MSG_TRAJ_OVERLIMIT, PARA_LIMIT);
        return false;
    }
    if (!CheckMotionData())
    {
        return false;
    }
    if (CheckFE())
        return false;
    //以当前单位进行规划 规划的单位要全部统一
    SPlanning->SetTargetPos(q0_, q1_);
    SPlanning->SetTargetVel(v0_, v1_);
    SPlanning->SetLimitVel(MData.Vel, MData.ACC, MData.Jerk);
    LastCmdPos = q0_;

    if (!SPlanning->InitialTrajData())
    {
        SetErrNum(ERROR_MSG_TRAJ_FAIL, PARA_SPEED);
        return false;
    }

    JogPlanning->SetMotionData(MData.Vel, MData.ACC);
    return true;
}


void CNC::PosGenerator::SetSampleJogInput(int Jogdir)
{
    JogPlanning->SampleJogActive = Jogdir;
}

void CNC::PosGenerator::GetJogPlanningPos(double Period_, double &Pos_)
{
    JogPlanning->StartJog(Period_);
    Theoretical.Vel = JogPlanning->GetRunningSpeed();
    //点动为增量位置
    CmdPos = JogPlanning->GetRunningPos();

    CheckOverSpeed(CmdPos,Period_,false);

    Pos_ = CmdPos;

    Theoretical.Pos += CmdPos;

    delta = Pos_;

    InterpolationPos = Pos_;

    Theoretical.Acc = (Theoretical.Vel-LastCmdSpeed)/Period_;
    LastCmdSpeed=Theoretical.Vel;

    Theoretical.Jerk = (Theoretical.Acc-LastCmdAcc)/Period_;
    LastCmdAcc=Theoretical.Acc;
}

bool CNC::PosGenerator::GetContinueTraj(double Period_, double &Pos_)
{
    if (PosBuffer.size() == 0)
    {
        GetContinueVel(Theoretical.Vel);
        Theoretical.Acc = 0;
        Theoretical.Jerk=0;
        return false;
    }

    CmdPos= PosBuffer.front();
    PosBuffer.erase(PosBuffer.begin());

    GetContinueVel(Theoretical.Vel);
    GetContinueAcc(Theoretical.Acc);

    CheckOverSpeed(CmdPos,Period_,false);
    Pos_ = CmdPos;

    Theoretical.Pos += CmdPos;

    delta = CmdPos;

    InterpolationPos = Pos_;

    Theoretical.Jerk = (Theoretical.Acc-LastCmdAcc)/Period_;
    LastCmdAcc=Theoretical.Acc;

    return true;
}

bool CNC::PosGenerator::GetContinueVel(double &Vel_)
{
    if (SpeedBuffer.size() == 0)
        return false;
    Vel_ = SpeedBuffer.front();

    SpeedBuffer.erase(SpeedBuffer.begin());
    return true;
}

void CNC::PosGenerator::SetPosBuffer(double Pos_)
{
    PosBuffer.push_back(Pos_);
}

void CNC::PosGenerator::SetSpeedBuffer(double Speed_)
{
    SpeedBuffer.push_back(Speed_);
}

bool CNC::PosGenerator::GetContinueAcc(double &Acc_)
{
    if (AccBuffer.size() == 0)
        return false;
    Acc_ = fabs(AccBuffer.front());

    AccBuffer.erase(AccBuffer.begin());
    return true;
}

void CNC::PosGenerator::SetAccBuffer(double Acc_)
{
    AccBuffer.push_back(Acc_);
}

bool CNC::PosGenerator::GetRTCPTraj(double Period_, double &Pos_)
{

    if (PosBuffer.empty())
    {
        Pos_ = 0.0;
        delta = Pos_;
        Theoretical.Vel = Pos_ / Period_;
        Theoretical.Acc = 0;
        Theoretical.Jerk=0;
        return false;
    }

    CmdPos = PosBuffer.front();
    PosBuffer.erase(PosBuffer.begin());

    CheckOverSpeed(CmdPos,Period_,true);

    Pos_ = CmdPos - LastCmdPos;
    LastCmdPos = CmdPos;

    delta = Pos_;

    //更新理论的绝对位置
    Theoretical.Pos = CmdPos;

    InterpolationPos = CmdPos;
    //Rtcp 的速度直接微分求取
    Theoretical.Vel = Pos_ / Period_;
    //Rtcp 的加速度直接微分求取
    Theoretical.Acc = (Theoretical.Vel-LastCmdSpeed)/Period_;
    LastCmdSpeed=Theoretical.Vel;

    Theoretical.Jerk = (Theoretical.Acc-LastCmdAcc)/Period_;
    LastCmdAcc=Theoretical.Acc;

    return true;
}

void CNC::PosGenerator::ClearPosVelBuffer()
{
    PosBuffer.clear();
    SpeedBuffer.clear();
}

void CNC::PosGenerator::SetStartCmdPos(double Pos_)
{
    LastCmdPos = Pos_;
}


//void CNC::PosGenerator::SyncAbsPos(double Pos_)
//{
//    Theoretical.Pos = Pos_;
//}

void CNC::PosGenerator::SyncRunningSpeed(double Vel_)
{
    Theoretical.Vel = Vel_;
}

void CNC::PosGenerator::ResetPos()
{
    Theoretical.Pos = Actual.Pos;
    SetStartCmdPos(Actual.Pos);
}

void CNC::PosGenerator::Defpos(double Pos_)
{
    PosOffset = -Actual.Pos + Pos_;
    Actual.Pos += PosOffset;
    ResetPos();
}

void CNC::PosGenerator::CalibrateZeroPos(double Pos_)
{
    PosOffset = Pos_;
    Actual.Pos += PosOffset;
    ResetPos();
}

void CNC::PosGenerator::PosOption(double Min_, double Max_)
{
    PosOptionMin = Min_;
    PosOptionMax = Max_;
    UsingPosOption = true;
}

bool CNC::PosGenerator::CheckOverSpeed(double &Cmd_,double Period_,bool IsAbsPos)
{
//    if (CheckFE())
//        return false;

    double overspeed =0;
    if (IsAbsPos)
        overspeed =fabs(Theoretical.Pos - Cmd_) / Period_;
    else
        overspeed =fabs(Cmd_) / Period_;

    if (overspeed > (MData.Vel * 1.1))// && !IsOverSpeed )
    {
        SetErrNum(ERROR_MSG_DANGER_SPEED, PARA_NONE);
        printf("[Cmd = %f Pos = %f Now = %f Limit = %f]",Cmd_,Theoretical.Pos,overspeed,MData.MaxVel);
        //lock output
        Cmd_ = Theoretical.Pos + delta;
        IsOverSpeed =true;
        return false;
    }
    return true;
}

bool CNC::PosGenerator::CheckFE()
{

    if (fabs(Theoretical.Pos-Actual.Pos) > MData.FELimit)
    {
        IsFollowErr=true;
        SetErrNum(ERROR_MSG_FE_WARN, PARA_NONE);
        printf("[ENC = %f Pos = %f FE = %f]",Actual.Pos,Theoretical.Pos,MData.FELimit);
    }
    else
        IsFollowErr=false;

    return IsFollowErr;
}

