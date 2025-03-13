#ifndef AXISSTEPSPORT_H
#define AXISSTEPSPORT_H

#include "Math/MotionPlanning/MotorControl.h"
#include "MachineControl/Common.hpp"

#include <queue>
#include <mutex>

namespace MachineControl{

struct AxisDataElenent {
    double pos = INVAILD_POS;
    double speed = INVAILD_SPEED;
};
using AxisRspDataModel = std::vector<AxisDataElenent>;

class AxisStepSport
{
public:
    AxisStepSport();
    void entity();
    void runAutoEntity();
    void handCmd(unsigned int jog_id, int jog_dir);
    void positionCmd(std::vector<double> &axisPs, std::vector<double> &axisPe);

    void execAutoCmd(std::vector<double> &axisVal);
    bool autoCmdCacheIsEmpty();
    void writeAutoCmd(std::vector<std::vector<double>> &axisvAal);
    void readAutoCmd(std::vector<std::vector<double>> &axisvAal);
    void stopAutoCmd();
    void clearAutoCmd();

    void quit();
    bool getAxisRespData(std::queue<AxisRspDataModel>& axisRspDataCache);
    bool getAxisCurVal(std::vector<double>& vals);

protected:
    bool putAxisRespData(const AxisRspDataModel& mData);
    bool setAxisCurVal(const std::vector<double>& vals);

private:
    std::atomic_bool m_quit = false;
    std::vector<CNC::MotorControl::Ptr> Axis;

    std::mutex m_axisMutex;
    std::queue<AxisRspDataModel> m_axisRspDataCache;

    std::mutex m_curAxisDataMutex;
    std::vector<double> m_curAxisPos;
    std::vector<double> m_curAxisSpeed;

    std::atomic_bool m_clear = false;
    std::mutex m_autoCmdMutex;
    int m_autoCmdReadInx = 0;
    int m_autoCmdWriteInx = 1;
    std::vector<std::vector<double>> m_autoCmdCache[2];
};
}



#endif
