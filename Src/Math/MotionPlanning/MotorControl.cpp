
#include <math.h>
#include <iostream>
#include <iomanip>

#include "MotorControl.h"
#include "MathDataDefine.h"


//初始化轴数量统计
int CNC::MotorControl::AxisNumCount = 0;

CNC::MotorControl::MotorControl(unsigned int EncoderUnit_, unsigned int EncoderType_, unsigned int Move_Type_,bool Virtral_,double Ratio_, double Pitch_)
{

    PhysicsAxis = std::make_shared<BaseMotorData>(Virtral_);
    PhysicsAxis->SetEncoderData(EncoderUnit_, EncoderType_);
    PhysicsAxis->ResetState();

    PosGenera = std::make_shared<PosGenerator>();

    //必须在轴数据后再new

    PhysicsAxis->MoveType = Move_Type_;

    if (PhysicsAxis->MoveType == LINEAR)
    {
        PhysicsAxis->Ratio = Ratio_;
        PhysicsAxis->Pitch = Pitch_;
        PhysicsAxis->DataUpper = 0.001;//速度误差在[-0.001,0.001] deg/s
        PhysicsAxis->DataLower = -0.001;
    }
    if (PhysicsAxis->MoveType == ROTATE)
    {
        PhysicsAxis->Ratio = Ratio_;
        PhysicsAxis->Pitch = 1;
        PhysicsAxis->DataUpper = 0.1;//速度误差在[-0.1,0.1] deg/s
        PhysicsAxis->DataLower = -0.1;
    }

    TargetPos = 0.0;

    OverSpeed = false;

    SimSpeedMode = true;//真实速度执行计算

    IsVitrualAxis = Virtral_;

    commandMode = COMMAND_MOTOR_NONE;

    ResCmd = 0.0;

    Initial = false;

    IsHomed = false;

    //PosGenera->DataOutPutMode = false;

    IsWheelJog = false;

    AxisNumCount++;

}

CNC::MotorControl::MotorControl(const MotorControl &in):
    IsHomed(in.IsHomed),SimSpeedMode(in.SimSpeedMode),IsVitrualAxis(in.IsVitrualAxis),commandMode(in.commandMode),
    IsWheelJog(in.IsWheelJog),OverSpeed(in.OverSpeed),Initial(in.Initial),ResCmd(in.ResCmd),
    TargetPos(in.TargetPos),LastCmdPos(in.LastCmdPos)
{
    PhysicsAxis = std::make_shared<BaseMotorData>(*in.PhysicsAxis);
    PosGenera = std::make_shared<PosGenerator>(*in.PosGenera);
}

CNC::MotorControl::~MotorControl()
{
}

bool CNC::MotorControl::SetSpeedData(double Jerk_, double ACC_, double DEC_, double Vel_)
{
    bool res=false;
    if (PhysicsAxis->GetControlMode() == MODE_CSV && PhysicsAxis->MoveType == ROTATE)
    {
        res=PosGenera->SetSpeedData(Jerk_*360,ACC_*360, DEC_*360,Vel_*360);
    }
    else
    {
        res=PosGenera->SetSpeedData(Jerk_,ACC_, DEC_,Vel_);
    }

    return res;
}

bool CNC::MotorControl::SetMaxSpeedData(double MaxJerk_, double MaxACC_, double MaxDEC_, double MaxVel_)
{
    bool res=false;
    if (PhysicsAxis->GetControlMode() == MODE_CSV && PhysicsAxis->MoveType == ROTATE)
    {
        res=PosGenera->SetMaxSpeedData(MaxJerk_*360, MaxACC_*360, MaxDEC_*360, MaxVel_*360);
    }
    else
    {
        res=PosGenera->SetMaxSpeedData(MaxJerk_, MaxACC_, MaxDEC_, MaxVel_);
    }

    return res;
}

bool CNC::MotorControl::SetJogSpeedData(double ACC_, double Vel_)
{
    bool res=false;
    if (PhysicsAxis->GetControlMode() == MODE_CSV && PhysicsAxis->MoveType == ROTATE)
    {
        res=PosGenera->SetJogSpeedData(ACC_*360, Vel_*360);
    }
    else
    {
        res=PosGenera->SetJogSpeedData(ACC_, Vel_);
    }
    return res;
}

void CNC::MotorControl::SetFSLimit(double FS_Limit_)
{
    PosGenera->MData.FSLimit = FS_Limit_;
    PhysicsAxis->SetFSLimit(PhysicsAxis->PosScaleChange(FS_Limit_));
}

void CNC::MotorControl::SetRSLimit(double RS_Limit_)
{
    PosGenera->MData.RSLimit = RS_Limit_;
    PhysicsAxis->SetRSLimit(PhysicsAxis->PosScaleChange(RS_Limit_));
}

void CNC::MotorControl::SetFELimit(double FE_Limit_)
{
    PosGenera->MData.FELimit = FE_Limit_;
    PhysicsAxis->SetFELimit(PhysicsAxis->PosScaleChange(FE_Limit_));
}
void CNC::MotorControl::SetPosRange(double Min_, double Max_)
{
    PhysicsAxis->PosOption(PhysicsAxis->PosScaleChange(Min_), PhysicsAxis->PosScaleChange(Max_));
    //同时数值显示需要激活
    PosGenera->PosOption(Min_, Max_);
}
void CNC::MotorControl::SetInfRange(double Min_, double Max_)
{
    PosGenera->PosOption(Min_, Max_);
    PhysicsAxis->InfiniteMode =true;
    PhysicsAxis->OptimalPathMode=true;
    //    SetFSLimit(999999999);
    //    SetRSLimit(-999999999);
}
// 通过位置(单位：几分之几圈)命令生成脉冲命令 增量��?
void CNC::MotorControl::PosCmd2Pluse(double Cmd_, int &PosCMD_)
{
    //因为在速度层里已经含减速比和导程比例转换了 此处不会体现减速比和导��?
    //按照设定速度的转换为脉冲命令 四舍五入取整
    //输入的CMD 单位 几分之几��?
    PosCMD_ = int(Cmd_ * double(PhysicsAxis->GetEncoderUnit()) + 0.5);
}

void CNC::MotorControl::PosAbsCmd2Pluse(double Cmd_, int &PosCMD_)
{
    //输入的CMD 单位角度
    //PosCMD_ = int((Cmd_ / 360.0) * double(PhysicsAxis->GetEncoderUnit()) + 0.5);
    double intPart = 0.0, fractPart= 0.0;
    double cmd = (Cmd_ / 360.0) * double(PhysicsAxis->GetEncoderUnit());
    fractPart = modf(cmd, &intPart);
    ResCmd += fractPart;
    if (fabs(ResCmd) > 1.0)
    {
        PosCMD_ = int(intPart) + SIGN(ResCmd) * 1;
        ResCmd = ResCmd - SIGN(ResCmd) * 1;
    }
    else
    {
        PosCMD_ = int(intPart);
    }

}
//通过速度命令生成脉冲命令  增量��?
void CNC::MotorControl::SpeedCmd2Pluse(double SpeedCmd_, int &PosCMD_, double Period_)
{
    //因为在速度层里已经含减速比和导程比例转换了 此处不会体现减速比和导��?
    //按照设定速度的转换为脉冲命令 四舍五入取整
    PosCMD_ = int(double(PhysicsAxis->GetEncoderUnit()) * (Period_) * SpeedCmd_ + 0.5);
}


void CNC::MotorControl::EstimateLimit()
{
    if (!PhysicsAxis->MotorInputData.IsBlockLimit)
    {

        //        double dis = SIGN(GetSpeed()) *(GetSpeed() * GetSpeed()) / (2 * PosGenera->MData.JogAcc * 0.9) + GetPos();

        double speed = PosGenera->GetVel();
        double acc = PosGenera->MData.JogAcc* 0.9;
        double dis = SIGN(speed) *(speed * speed) / (2 * acc) + PosGenera->GetNoneTransPos();


        if (dis + DoubleErr >= PosGenera->MData.FSLimit && PhysicsAxis->GetIsIDLE() > 0)
        {
            PhysicsAxis->MotorInputData.IsError = true;
        }

        if (dis + DoubleErr <= PosGenera->MData.RSLimit && PhysicsAxis->GetIsIDLE() < 0)
        {
            PhysicsAxis->MotorInputData.IsError = true;
        }
    }

}

double CNC::MotorControl::GetPos() const
{
    //Pos = deg/360 /ratio*pitch
    if (PhysicsAxis->MoveType == LINEAR)
    {
        return ((PhysicsAxis->GetPosFB() / 360.0) / PhysicsAxis->Ratio) * PhysicsAxis->Pitch;
    }

    if (PhysicsAxis->MoveType == ROTATE)
    {
        return ((PhysicsAxis->GetPosFB()) / PhysicsAxis->Ratio);
    }

    return 0.0;
}

double CNC::MotorControl::GetSpeed() const
{
    if (PhysicsAxis->MoveType == LINEAR)
    {
        if (SimSpeedMode)
            return ((PhysicsAxis->GetSimData()) / PhysicsAxis->Ratio) * PhysicsAxis->Pitch;
        else
            return ((PhysicsAxis->GetVelFB()) / PhysicsAxis->Ratio) * PhysicsAxis->Pitch;
    }

    if (PhysicsAxis->MoveType == ROTATE)
    {
        //rps /减速比 *360 = deg/s
        if (SimSpeedMode)
            return (((PhysicsAxis->GetSimData()) / PhysicsAxis->Ratio) * 360.0);
        else
            return (((PhysicsAxis->GetVelFB()) / PhysicsAxis->Ratio) * 360.0);
    }

    return 0.0;
}

void CNC::MotorControl::Reset()
{

    if (PhysicsAxis->GetError() == ERROR_MSG_FE_WARN)
    {
        PhysicsAxis->MotorInputData.IsFELimit = false;
        PosGenera->IsFollowErr=false;
        PosGenera->ResetPos();
        PosGenera->SetStartCmdPos(PosGenera->GetNoneTransPos());
    }
    if (PhysicsAxis->GetError() == ERROR_MSG_DANGER_SPEED)
    {
        PhysicsAxis->MotorInputData.IsOverSpeed = false;
        PosGenera->ResetErrNum();
        PosGenera->IsOverSpeed = false;
    }
    if (PhysicsAxis->GetError() == ERROR_MSG_POS_DIFF)
    {
        PhysicsAxis->IsDefpos= true;
    }

    PhysicsAxis->ResetState();
}

//虚拟轴或者仿真使��?
void CNC::MotorControl::MoveSimulate(int Cmd_, double Period_)
{
    double Speed_ = (Cmd_ / double(PhysicsAxis->GetEncoderUnit())) / Period_;
    PhysicsAxis->SetFBData(PhysicsAxis->GetEncoderFB() + Cmd_, Speed_, 0.0);
}
//更新虚拟轴速度  通过脉冲计算出速度
void CNC::MotorControl::UpdateSimSpeed(int Cmd_, double Period_)
{
    PhysicsAxis->GetSimData() = (Cmd_ / double(PhysicsAxis->GetEncoderUnit())) / Period_;
}

//更新虚拟轴速度  通过规划轨迹下发速度
void CNC::MotorControl::UpdateSimSpeed(double Vel_)
{
    PhysicsAxis->GetSimData() = PhysicsAxis->SpeedScaleChange(Vel_);
}

bool CNC::MotorControl::GetIsWheelJog() const
{
    return IsWheelJog;
}

bool CNC::MotorControl::ActiveWheelJog(bool Active_)
{
    if (PhysicsAxis->GetIsIDLE() != 0)
    {
        return false;
    }
    IsWheelJog = Active_;
    return true;
}

void CNC::MotorControl::SetCommandMode(CommandMode Mode_)
{
    if (commandMode == Mode_)
    {
        return;
    }

    if (Mode_ < COMMAND_MOTOR_NONE || Mode_ > COMMAND_MOTOR_RAPIDSTOP)
        return;

    //急停前需要同步T型减速规划的速度
    if (Mode_ == COMMAND_MOTOR_RAPIDSTOP && commandMode != COMMAND_MOTOR_RAPIDSTOP)
    {
        PosGenera->JogPlanning->SyncSpeed(PosGenera->GetPlanningSpeed());
    }

    if (PhysicsAxis->GetIsIDLE() != 0)
    {
        if (Mode_>=COMMAND_MOTOR_JOG && Mode_<= COMMAND_ABSOLUTE_TRAJ)
            return;
    }


    commandMode = Mode_;

    //初始化运动参数设��?
    switch (commandMode)
    {
    case COMMAND_MOTOR_NONE:
        break;
    case COMMAND_MOTOR_WHEELJOG:
        break;
    case COMMAND_MOTOR_JOG:
        if (PhysicsAxis->GetIsIDLE() == 0)
        {
            if (PhysicsAxis->MoveType == LINEAR)
            {
                PosGenera->JogPlanning->SetIDLERange(0.001, -0.001);
            }
            if (PhysicsAxis->MoveType == ROTATE)
            {
                PosGenera->JogPlanning->SetIDLERange(0.1, -0.1);
            }

            //PhysicsAxis->ResetState();

            if (!PosGenera->SetJogSpeedData(PosGenera->MData.JogAcc , PosGenera->MData.JogVel))
            {
                //速度设定失败，以默认速度运行
                PhysicsAxis->SetErrNum(ERROR_MSG_ZEROSPEED, PhysicsAxis->PARA_SPEED);
            }

        }
        break;
    case COMMAND_MOTOR_HOMING:
        //SetSpeedData(home);
        PhysicsAxis->ResetState();
        break;
    case COMMAND_MOTOR_SMOVE:
        //SetSpeedData(absspeed);
        PhysicsAxis->ResetState();
        break;
    case COMMAND_MOTOR_TMOVE:
        //SetSpeedData(absspeed);
        PhysicsAxis->ResetState();
        break;

    case COMMAND_INCREMENT_TRAJ:case COMMAND_ABSOLUTE_TRAJ:
        PhysicsAxis->ResetState();
        ResCmd = 0.0;
        PosGenera->ClearPosVelBuffer();
        //PosGenera->SetStartCmdPos(GetPos());
        PosGenera->SetStartCmdPos(PosGenera->GetNoneTransPos());

        break;
    case COMMAND_MOTOR_RAPIDSTOP:

        break;
    }
}

bool CNC::MotorControl::MoveForward()
{
    if (PhysicsAxis->GetControlMode() != MODE_CSV)
    {
        PhysicsAxis->SetErrNum(ERROR_MSG_DATA_SET, PhysicsAxis->PARA_CMODE);
        return false;
    }

    SetCommandMode(CNC::MotorControl::CommandMode::COMMAND_MOTOR_JOG);
    PhysicsAxis->MotorInputData.REV_MOVE = OFF;
    PhysicsAxis->MotorInputData.FWD_MOVE = ON;

    return true;
}

bool CNC::MotorControl::MoveReverse()
{
    if (PhysicsAxis->GetControlMode() != MODE_CSV)
    {
        PhysicsAxis->SetErrNum(ERROR_MSG_DATA_SET, PhysicsAxis->PARA_CMODE);
        return false;
    }

    SetCommandMode(CNC::MotorControl::CommandMode::COMMAND_MOTOR_JOG);
    PhysicsAxis->MotorInputData.REV_MOVE = ON;
    PhysicsAxis->MotorInputData.FWD_MOVE = OFF;

    return true;
}

void CNC::MotorControl::RapidStop()
{
    if (PhysicsAxis->GetError() != ERROR_MSG_NONE)
    {
        if (PhysicsAxis->GetIsIDLE() != 0)
        {
            SetCommandMode(CommandMode::COMMAND_MOTOR_RAPIDSTOP);
        }
    }
}

void CNC::MotorControl::SetJogInput()
{
    if (PhysicsAxis->GetError() == ERROR_MSG_NONE)
    {
        if (PhysicsAxis->GetControlMode() == MODE_CSP)
        {
            if (PhysicsAxis->MotorInputData.FWD_JOG == ON || PhysicsAxis->MotorInputData.REV_JOG == ON)
            {
                if (PhysicsAxis->MotorInputData.FWD_JOG == ON && !PhysicsAxis->GetIsFSLimit())
                {
                    PosGenera->SetSampleJogInput(1);
                }
                else if (PhysicsAxis->MotorInputData.REV_JOG == ON && !PhysicsAxis->GetIsRSLimit())
                {
                    PosGenera->SetSampleJogInput(-1);
                }
                else
                {
                    PosGenera->SetSampleJogInput(0);
                }
            }
            else
            {
                PosGenera->SetSampleJogInput(0);
            }
        }
        else if (PhysicsAxis->GetControlMode() == MODE_CSV)
        {
            if (PhysicsAxis->MotorInputData.FWD_MOVE == ON)
            {
                PosGenera->SetSampleJogInput(1);
            }
            else if (PhysicsAxis->MotorInputData.REV_MOVE == ON)
            {
                PosGenera->SetSampleJogInput(-1);
            }
            else
            {
                PosGenera->SetSampleJogInput(0);
            }
        }
    }
    else
    {
        PosGenera->SetSampleJogInput(0);
    }
}

void CNC::MotorControl::MainControlLogic(double Period_)
{

    //位置,限位信息刷新
    PhysicsAxis->UpdateMotorData();
    //
    PosGenera->SyncActualData(GetSpeed(),GetPos());
    if (PhysicsAxis->IsDefpos)
    {
        PhysicsAxis->IsDefpos = false;
        PosGenera->ResetPos();
        //PosGenera->CalibrateZeroPos(-0.00132);
        //PosGenera->DataManager->ResetPos();
    }


    //单轴功能单轴管理
    if (commandMode >= COMMAND_MOTOR_JOG && commandMode <= COMMAND_MOTOR_TMOVE)
    {
        if ( commandMode == COMMAND_MOTOR_JOG || commandMode == COMMAND_MOTOR_HOMING || commandMode == COMMAND_MOTOR_WHEELJOG)
        {
            //限位预测停止
            if (abs(PhysicsAxis->GetIsIDLE()))
                EstimateLimit();
        }

        //故障紧急停��?
        RapidStop();
    }



    switch (commandMode)
    {
    case COMMAND_MOTOR_JOG:
        SetJogInput();
        PosGenera->GetJogPlanningPos(Period_, TargetPos);
        PosAbsCmd2Pluse(PhysicsAxis->PosScaleChange(TargetPos), PhysicsAxis->GetPosCmd());

        break;
    case COMMAND_MOTOR_HOMING:
        break;
    case COMMAND_MOTOR_WHEELJOG:
        break;
    case COMMAND_MOTOR_SMOVE:
        if (PosGenera->GetSPlanningPos(Period_, TargetPos))
        {
            PosAbsCmd2Pluse(PhysicsAxis->PosScaleChange(TargetPos), PhysicsAxis->GetPosCmd());
            PosGenera->JogPlanning->SyncSpeed(PosGenera->GetPlanningSpeed());
        }
        else
        {
            PosGenera->SetSampleJogInput(0);
            PosGenera->GetJogPlanningPos(Period_, TargetPos);
            PosAbsCmd2Pluse(PhysicsAxis->PosScaleChange(TargetPos), PhysicsAxis->GetPosCmd());
        }
        break;
    case COMMAND_MOTOR_TMOVE:
        if (PosGenera->GetTPlanningPos(Period_, TargetPos))
        {
            PosAbsCmd2Pluse(PhysicsAxis->PosScaleChange(TargetPos), PhysicsAxis->GetPosCmd());
            PosGenera->JogPlanning->SyncSpeed(PosGenera->GetPlanningSpeed());
        }
        else
        {
            PosGenera->SetSampleJogInput(0);
            PosGenera->GetJogPlanningPos(Period_, TargetPos);
            PosAbsCmd2Pluse(PhysicsAxis->PosScaleChange(TargetPos), PhysicsAxis->GetPosCmd());
        }
        break;
    case COMMAND_INCREMENT_TRAJ:
        if (PosGenera->GetContinueTraj(Period_, TargetPos))
        {
            PosAbsCmd2Pluse(PhysicsAxis->PosScaleChange(TargetPos), PhysicsAxis->GetPosCmd());
        }
        else
        {
            TargetPos = 0.0;
            PosAbsCmd2Pluse(PhysicsAxis->PosScaleChange(TargetPos), PhysicsAxis->GetPosCmd());
        }
        break;
    case COMMAND_ABSOLUTE_TRAJ:

        PosGenera->GetRTCPTraj(Period_, TargetPos);
        PosAbsCmd2Pluse(PhysicsAxis->PosScaleChange(TargetPos), PhysicsAxis->GetPosCmd());

        break;
    case COMMAND_MOTOR_RAPIDSTOP:
        PosGenera->SetSampleJogInput(0);
        PosGenera->GetJogPlanningPos(Period_, TargetPos);
        PosAbsCmd2Pluse(PhysicsAxis->PosScaleChange(TargetPos), PhysicsAxis->GetPosCmd());

        if (PhysicsAxis->GetIsIDLE() == 0)
        {
            SetCommandMode(CommandMode::COMMAND_MOTOR_JOG);
        }
        break;
    default:
        break;

    }
    //根据理论位置和反馈位置计算跟随误��?
    if (PosGenera->IsFollowErr)
    {
        PhysicsAxis->MotorInputData.IsFELimit = true;
        PhysicsAxis->SetErrNum(PosGenera->GetError(), PosGenera->GetSubError());
        std::cout<<PosGenera->StrError(PosGenera->GetError())<<PosGenera->StrSubError(PosGenera->GetSubError())<<std::endl;

    }

    if (PosGenera->IsOverSpeed && !PhysicsAxis->MotorInputData.IsOverSpeed)
    {
        PhysicsAxis->MotorInputData.IsOverSpeed = true;
        PhysicsAxis->SetErrNum(PosGenera->GetError(), PosGenera->GetSubError());
        std::cout<<PosGenera->StrError(PosGenera->GetError())<<PosGenera->StrSubError(PosGenera->GetSubError())<<std::endl;
    }

    //通过规划速度下发关节速度
    //UpdateSimSpeed(PosGenera->GetPlanningSpeed());
    //通过脉冲下发解算当前关节速度
    UpdateSimSpeed(PhysicsAxis->GetPosCmd(), Period_);

    if (IsVitrualAxis)
    {
        //模拟环境下调��? 周期写入位置数据
        MoveSimulate(PhysicsAxis->GetPosCmd(), Period_);
    }


}
