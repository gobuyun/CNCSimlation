#include "AxisStepSport.h"

#include <iostream>

using namespace CNC;
using namespace MachineControl;

AxisStepSport::AxisStepSport()
{
    //建立轴测试
    Axis.resize(5);

    double jerk_xyz = 1,acc_xyz = 1,vel_xyz =1;
    double jerk_abc = 1,acc_abc = 1,vel_abc =1;
    //正负限位
    double fs[5]={999900,999900,999900,999900,999900};
    double rs[5]={-999900,-999900,-999900,-999900,-999900};

    /*  设定轴参数  轴参数 初始化 */
    for (int i = 0; i < Axis.size(); ++i)
    {
        if(i<3)
        {
            Axis.at(i)=std::make_shared<MotorControl>(pow(2,23), ABSOLUTE_ENCODER, LINEAR, true, 1, 1);
            Axis.at(i)->SetMaxSpeedData(jerk_xyz, acc_xyz, acc_xyz, vel_xyz);
            Axis.at(i)->SetSpeedData(jerk_xyz, acc_xyz, acc_xyz, vel_xyz);
            Axis.at(i)->SetJogSpeedData(acc_xyz, vel_xyz);
        }
        else
        {
            Axis.at(i)=std::make_shared<MotorControl>(pow(2,23), ABSOLUTE_ENCODER, ROTATE, true, 1, 1);
            Axis.at(i)->SetMaxSpeedData(jerk_abc, acc_abc, acc_abc, vel_abc);
            Axis.at(i)->SetSpeedData(jerk_abc, acc_abc, acc_abc, vel_abc);
            Axis.at(i)->SetJogSpeedData(acc_abc, vel_abc);
        }
        Axis.at(i)->SetFELimit(1);
        Axis.at(i)->SetFSLimit(fs[i]); //
        Axis.at(i)->SetRSLimit(rs[i]);//
        Axis.at(i)->PhysicsAxis->MotorInputData.IsEnable = ON;
        Axis.at(i)->PosGenera->DataOutPutMode=false;
        Axis.at(i)->SetCommandMode(MotorControl::CommandMode::COMMAND_MOTOR_JOG);
    }

      //这里可以初始化设定轴位置 上电设定x 50 y-50 z-50

      // Axis.at(0)->PhysicsAxis->Defpos(0);
      // Axis.at(1)->PhysicsAxis->Defpos(-0);
      // Axis.at(2)->PhysicsAxis->Defpos(-50);
}

void AxisStepSport::handCmd(unsigned int jog_id, int jog_dir)
{
    //切换点动模式
    Axis.at(jog_id)->SetCommandMode(CNC::MotorControl::CommandMode::COMMAND_MOTOR_JOG);
    if (jog_dir>=1)
    {
        Axis.at(jog_id)->PhysicsAxis->MotorInputData.FWD_JOG = ON;
        Axis.at(jog_id)->PhysicsAxis->MotorInputData.REV_JOG = OFF;
    }else if (jog_dir<=-1)
    {
        Axis.at(jog_id)->PhysicsAxis->MotorInputData.FWD_JOG = OFF;
        Axis.at(jog_id)->PhysicsAxis->MotorInputData.REV_JOG = ON;
    }else
    {
        Axis.at(jog_id)->PhysicsAxis->MotorInputData.FWD_JOG = OFF;
        Axis.at(jog_id)->PhysicsAxis->MotorInputData.REV_JOG = OFF;
    }
}

void AxisStepSport::positionCmd(std::vector<double>& axisPs, std::vector<double>& axisPe)
{
    //可以进行定位运动
    auto s = std::min<int>(axisPs.size(), axisPe.size());
    for (unsigned long jog_id = 0; jog_id<s; ++jog_id)
    {
        auto ps = axisPs[jog_id];
        auto pe = axisPe[jog_id];
        if(Axis.at(jog_id)->PosGenera->STrajPlanning(ps,pe))
        {
            Axis.at(jog_id)->SetCommandMode(CNC::MotorControl::CommandMode::COMMAND_MOTOR_SMOVE);
        }
    }
}

bool AxisStepSport::autoCmdCacheIsEmpty()
{
    std::lock_guard autoLock(m_autoCmdMutex);
    return m_autoCmdCache[m_autoCmdWriteInx].empty();
}

void AxisStepSport::execAutoCmd(std::vector<double> &axisVal)
{
    // run one line
    auto jog_id = 0;
    auto s = std::min<int>(axisVal.size(), Axis.size());
    for (auto i = 0; i<s; ++i)
    {
        auto pos = axisVal[i];
        //第一步先切换模式
        Axis.at(jog_id)->SetCommandMode(CNC::MotorControl::CommandMode::COMMAND_ABSOLUTE_TRAJ);
        //第二步发送插补位置
        Axis.at(jog_id)->PosGenera->SetPosBuffer(pos);
        ++jog_id;
    }
}

void AxisStepSport::writeAutoCmd(std::vector<std::vector<double>> &axisvAal)
{
    std::lock_guard autoLock(m_autoCmdMutex);
    m_autoCmdCache[m_autoCmdWriteInx] = axisvAal;
}

void AxisStepSport::readAutoCmd(std::vector<std::vector<double>> &axisvAal)
{
    std::lock_guard autoLock(m_autoCmdMutex);
    axisvAal.swap(m_autoCmdCache[m_autoCmdReadInx]);
    m_autoCmdReadInx = m_autoCmdWriteInx;
    m_autoCmdWriteInx = (m_autoCmdWriteInx+1)%2;
}

void AxisStepSport::stopAutoCmd()
{
    m_clear.store(true);
}

void AxisStepSport::clearAutoCmd()
{
    m_autoCmdReadInx = 0;
    m_autoCmdWriteInx = 1;
    m_autoCmdCache[0].clear();
    m_autoCmdCache[1].clear();
}

void AxisStepSport::runAutoEntity()
{
    //可以进行插补运动
    while (!m_quit.load())
    {
        std::vector<std::vector<double>> axisVals;
        readAutoCmd(axisVals);
        for (auto axisVal : axisVals)
        {
            if (m_clear.load())
            {
                clearAutoCmd();
                m_clear.store(false);
                break;
            }
            // run one line
            auto jog_id = 0;
            auto s = std::min<int>(axisVal.size(), Axis.size());
            for (auto i = 0; i<s; ++i)
            {
                auto pos = axisVal[i];
                //第一步先切换模式
                Axis.at(jog_id)->SetCommandMode(CNC::MotorControl::CommandMode::COMMAND_ABSOLUTE_TRAJ);
                //第二步发送插补位置
                Axis.at(jog_id)->PosGenera->SetPosBuffer(pos);
                ++jog_id;
            }
        }
        _sleep(10);
    }
}

void AxisStepSport::entity()
{
    //找一个线程一直刷
    const double CmdPeriodTime = 1;//控制周期为1ms
    AxisRspDataModel axisRspData;
    axisRspData.resize(Axis.size());
    std::vector<double> posVec;
    std::vector<double> speedVec;
    posVec.resize(Axis.size());
    speedVec.resize(Axis.size());
    while (!m_quit.load())
    {
        //获取反馈数值
        for (unsigned int i = 0; i < Axis.size(); i++)
        {
            AxisDataElenent ele;
            //每周期刷新
            Axis.at(i)->MainControlLogic(CmdPeriodTime);

            //位置
//            std::cout<<"pos="<<Axis.at(i)->PosGenera->GetPos()<<std::endl;
            auto pos = Axis.at(i)->PosGenera->GetPos();
            posVec.at(i) = pos;

            //速度
//            std::cout<<"vel="<<Axis.at(i)->PosGenera->GetVel()<<std::endl;
            auto speed = Axis.at(i)->PosGenera->GetVel();
            speedVec.at(i) = speed;

            int err_num=0;
            err_num = Axis.at(i)->PhysicsAxis->GetError();
            if(err_num != ERROR_MSG_NONE)
            {
                std::cout<<"err_num="<<err_num<<std::endl;
                std::cout<<"why?"<<Axis.at(i)->PhysicsAxis->StrError(err_num)<<std::endl;
            }

            ele.pos = pos;
            ele.speed = speed;
            axisRspData[i] = ele;
        }
        setAxisCurVal(posVec);
        _sleep(10);
    }
}

void AxisStepSport::quit()
{
    m_quit.store(true);
}

bool AxisStepSport::getAxisRespData(std::queue<AxisRspDataModel> &axisRspDataCache)
{
    return false;
    std::lock_guard autoLock(m_axisMutex);
    axisRspDataCache.swap(m_axisRspDataCache);
    return true;
}

bool AxisStepSport::putAxisRespData(const AxisRspDataModel& mData)
{
    return false;
    std::lock_guard autoLock(m_axisMutex);
    m_axisRspDataCache.push(mData);
    return true;
}

bool AxisStepSport::getAxisCurVal(std::vector<double>& vals)
{
    std::lock_guard autoLock(m_axisMutex);
    vals = m_curAxisPos;
    return true;
}

bool AxisStepSport::setAxisCurVal(const std::vector<double>& vals)
{
    std::lock_guard autoLock(m_curAxisDataMutex);
    m_curAxisPos = vals;
    return true;
}

