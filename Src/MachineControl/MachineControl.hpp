#ifndef MACHINECONTROL_HPP
#define MACHINECONTROL_HPP

#include "AxisSport/AxisStepSport.h"

#include <thread>

namespace MachineControl{
    class InnerMachineControl
    {
    public:
        InnerMachineControl()
        {
            m_businness = new AxisStepSport;
            m_worker = new std::thread(&AxisStepSport::entity, m_businness);
            m_gcodeWorker = new std::thread(&AxisStepSport::runAutoEntity, m_businness);
        }
        bool getAxisRspData(std::queue<AxisRspDataModel> &axisRspDataCache)
        {
            return m_businness->getAxisRespData(axisRspDataCache);
        }
        bool getAxisCurVal(std::vector<double>& axisvAal)
        {
            return m_businness->getAxisCurVal(axisvAal);
        }
        void handCmd(unsigned int jog_id, int jog_dir)
        {
            return m_businness->handCmd(jog_id, jog_dir);
        }
        void positionCmd(std::vector<double>& axisPs, std::vector<double>& axisPe)
        {
            return m_businness->positionCmd(axisPs, axisPe);
        }
        void autoCmd(std::vector<std::vector<double>>& axisvAal)
        {
            return m_businness->writeAutoCmd(axisvAal);
        }
        bool autoCmdCacheIsEmpty()
        {
            return m_businness->autoCmdCacheIsEmpty();
        }
        void stopAutoCmd()
        {
            return m_businness->stopAutoCmd();
        }
        void execAutoCmd(std::vector<double> &axisVal)
        {
            return m_businness->execAutoCmd(axisVal);
        }

        ~InnerMachineControl()
        {
            if (!m_worker || !m_businness)
                return;

            m_businness->quit();

            if (m_worker->joinable())
                m_worker->join();

            if (m_gcodeWorker->joinable())
                m_gcodeWorker->join();

            delete m_worker;
            delete m_gcodeWorker;
            delete m_businness;
        }
        static InnerMachineControl* getInstance() {
            static InnerMachineControl machineControlIns;
            return &machineControlIns;
        }
    private:
        std::thread* m_worker = nullptr;
        std::thread* m_gcodeWorker = nullptr;
        AxisStepSport* m_businness = nullptr;
    };

    // inline bool getAxisRspData(std::queue<AxisRspDataModel> &axisRspDataCache)
    // {
    //     return InnerMachineControl::getInstance()->getAxisRspData(axisRspDataCache);
    // }
    inline bool getAxisCurVal(std::vector<double>& axisCurVal)
    {
        return InnerMachineControl::getInstance()->getAxisCurVal(axisCurVal);
    }

    inline void handCmd(unsigned int jog_id, int jog_dir)
    {
        return InnerMachineControl::getInstance()->handCmd(jog_id, jog_dir);
    }
    inline void positionCmd(std::vector<double>& axisPs, std::vector<double>& axisPe)
    {
        return InnerMachineControl::getInstance()->positionCmd(axisPs, axisPe);
    }
    inline void autoCmd(std::vector<std::vector<double>>& axisvAal)
    {
        return InnerMachineControl::getInstance()->autoCmd(axisvAal);
    }
    inline bool autoCmdCacheIsEmpty()
    {
        return InnerMachineControl::getInstance()->autoCmdCacheIsEmpty();
    }
    inline void stopAutoCmd()
    {
        return InnerMachineControl::getInstance()->stopAutoCmd();
    }
    inline void execAutoCmd(std::vector<double> &axisVal)
    {
        return InnerMachineControl::getInstance()->execAutoCmd(axisVal);
    }


    inline void prepare()
    {
    }

    inline void start()
    {
        InnerMachineControl::getInstance();
    }

    inline void quit()
    {
    }
}

#endif // MACHINECONTROL_HPP
