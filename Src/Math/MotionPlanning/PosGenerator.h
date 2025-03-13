#pragma once
#include <memory>
#include <vector>
#include "../MotionCommon/ErrMsg.h"
#include "Traj_S.h"
#include "Traj_T.h"
#include "SampleJog.h"

/*  所有轨迹规划方法通过此类发出数据 */
/*
点动轨迹
回原点轨迹
S型
T型
RTCP几何轨迹

*/
namespace CNC
{


class PosGenerator :public ErrMsg
{
public:
    PosGenerator();
    ~PosGenerator();

    void SyncActualData(double Vel_, double Pos_);

    void CancelMove();
    //速度未设置返回false    运动前检测运动参数
    bool CheckMotionData();
    //设定运动参数
    bool SetSpeedData(double Jerk_, double ACC_, double DEC_, double Vel_);
    bool SetMaxSpeedData(double MaxJerk_, double MaxACC_, double MaxDEC_, double MaxVel_);
    bool SetJogSpeedData(double ACC_, double Vel_);
    bool CheckIsOverMaxSpeed(double Jerk_, double ACC_, double DEC_, double Vel_);
    //获取规划的位置速度加速度和插补数据
    double GetPlanningSpeed() const;

    double GetPos() const;
    double GetNoneTransPos() const;     //读取真实无连续处理的位置
    double GetVel() const;
    double GetAcc() const;
    double GetJerk() const;

    double GetInterpolationPos() const;
    //获取T型轨迹位置速度
    bool GetTPlanningPos(double Period_, double &Pos_);
    bool TTrajPlanning(double q0_, double q1_, double v0_ = 0.0, double v1_= 0.0);
    //获取S型轨迹位置速度
    bool GetSPlanningPos(double Period_, double &Pos_);
    bool GetSPlanningAbsPos(double Period_, double &Pos_);
    bool STrajPlanning(double q0_, double q1_, double v0_ = 0.0, double v1_ = 0.0);

    //获取实时点动位置速度
    void SetSampleJogInput(int Jogdir);
    void GetJogPlanningPos(double Period_, double &Pos_);
    //手动坐标系设定轨迹
    bool GetContinueTraj(double Period_, double &Pos_);
    void SetPosBuffer(double Pos_);
    bool GetContinueVel(double &Vel_);
    void SetSpeedBuffer(double Speed_);
    bool GetContinueAcc(double &Acc_);
    void SetAccBuffer(double Acc_);
    //手动RTCP设定轨迹
    bool GetRTCPTraj(double Period_, double &Pos_);
    //清除缓存
    void ClearPosVelBuffer();
    //获取当前位置做增量命令  只在联动需要激活
    void SetStartCmdPos(double Pos_);
    //初次上电需要同步当前规划位置
    //void SyncAbsPos(double Pos_);

    void SyncRunningSpeed(double Vel_);
    //T轨迹规划方法
    std::shared_ptr<TVelPlanning> TPlanning;
    //S轨迹规划方法
    std::shared_ptr<SVelPlanning> SPlanning;
    //点动规划方法
    std::shared_ptr<SampleJog> JogPlanning;

    //运动规划参数
    struct MotionData
    {
        double MaxJerk;
        double MaxACC;
        double MaxDEC;
        double MaxVel;
        double Jerk;
        double ACC;
        double DEC;
        double Vel;
        double JogAcc;
        double JogVel;
        double FSLimit;
        double RSLimit;
        double FELimit;
    }MData;

    bool IsOverSpeed;
    bool IsFollowErr;
    bool CheckFE();

    void ResetPos();
    void Defpos(double Pos_);//传入绝对位置将显示位置归零 mm or deg
    void CalibrateZeroPos(double Pos_);
    void PosOption(double Min_, double Max_);//连续位置上下限
    //获取位置速度数据模式 true 真实  false 理论规划
    bool DataOutPutMode;

private:
    bool CheckOverSpeed(double &Cmd_,double Period_,bool IsAbsPos);

    bool CheckMotionPara();
    //规划位置
    double CmdPos;
    //上周期目标位置
    double LastCmdPos;
    //上周期目标速度
    double LastCmdSpeed;
    //上周期目标加速度
    double LastCmdAcc;
    //对外输出插补数据
    double InterpolationPos;
    //对外输出增量位置数据
    double delta;
    //连续轨迹位置缓存和规划速度缓存
    std::vector<double> PosBuffer;
    std::vector<double> SpeedBuffer;
    std::vector<double> AccBuffer;

    struct MotorMotionData{
        double Pos;
        double Vel;
        double Acc;
        double Jerk;
    }Actual,Theoretical;

    double PosOffset;

    //输出位置锁定上下限
    double PosOptionMin;
    double PosOptionMax;
    bool UsingPosOption;
};

}





