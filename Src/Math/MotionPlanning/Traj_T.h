#pragma once
#include "TrajPlanning.h"

namespace CNC
{

class TVelPlanning: public TrajPlanningMethod
{
public:
	TVelPlanning();
    virtual ~TVelPlanning();

public:

	void TReset(); //参数复位 停止运行或者运行完毕必须复位
	bool GenerateALL(double Period_);//根据参数生成所有点位  非实时规划使用
    virtual bool OnlinePlanning(double Period_, double &Pos_, double &Vel_,double &Acc_) override;//根据参数实时规划使用
	virtual bool InitialTrajData() override;//初始化轨迹数据 检查是否可以生成轨迹

private:
	void SignTransform();
	virtual bool GenerateACCData(double Period_, double &Acc_) override;//生成加速度数据  T型规划不需要用
	virtual bool GenerateVelData(double Period_, double &Vel_) override;//生成速度数据
	virtual bool GeneratePosData(double Period_, double &Pos) override;//生成位置数据
	
	double La; //加速阶段的位移
	double Lv; //匀速阶段的位移
	double Ld; //减速阶段的位移

	double Sign_;

};

}
