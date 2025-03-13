
#pragma once
#include "MotionState.h"


namespace CNC
{

class ErrMsg
{
public:
	enum ParameterType
	{
		PARA_NONE = 0,
		PARA_SPEED= 10,
		PARA_MAXSPEED = 20,
		PARA_ENC_UNIT = 30,
		PARA_MECHINE = 40,
		PARA_LIMIT = 50,
		PARA_INPUT = 60,
		PARA_RTCP = 70,
		PARA_CMODE = 80,
		PARA_GEOMETRY = 90,
	};

	ErrMsg() :ErrorNum(ERROR_MSG_NONE), ErrorSubNum(PARA_NONE)
	{}

	virtual ~ErrMsg()
	{}

	virtual void SetErrNum(int Er_, int SubEr_ = ParameterType::PARA_NONE)
	{
		ErrorNum = Er_;
		ErrorSubNum = SubEr_;
	}

	virtual int GetError() const { return ErrorNum; }
	virtual int GetSubError() const { return ErrorSubNum; }

	virtual void ResetErrNum() 
	{ 
		ErrorNum = ERROR_MSG_NONE; 
		ErrorSubNum = PARA_NONE;
	}

	virtual const char* StrError(const int Error_) const
	{
		if (ERROR_MSG_NONE == Error_) return "正常";
		else if (ERROR_MSG_DRIVER_ERR == Error_) return "驱动器报警";
		else if (ERROR_MSG_FS_WARN == Error_) return "正软限位";
		else if (ERROR_MSG_RS_WARN == Error_) return "负软限位";
		else if (ERROR_MSG_FE_WARN == Error_) return "跟随误差过过大";
		else if (ERROR_MSG_FWD_WARN == Error_) return "正硬限位";
		else if (ERROR_MSG_REV_WARN == Error_) return "负硬限位";
		else if (ERROR_MSG_DATA_SET == Error_) return "参数设定异常";
		else if (ERROR_MSG_NOT_ENABLE == Error_) return "未使能";
		else if (ERROR_MSG_MOTION == Error_) return "运动过程中即将超限位";
		else if (ERROR_MSG_ZEROSPEED == Error_) return "未设定运动速度";
		else if (ERROR_MSG_TRAJ_FAIL == Error_) return "单轴轨迹规划失败";
		else if (ERROR_MSG_TRAJ_OVERLIMIT == Error_) return "轨迹规划超限位";
		else if (ERROR_MSG_SPEED_OVER == Error_) return "速度规划参数超轴上限";
		else if (ERROR_MSG_UNIDLE == Error_) return "未静止无法禁止修改";
		else if (ERROR_MSG_DANGER_SPEED == Error_) return "命令超速";
		else if (ERROR_MSG_POS_DIFF == Error_) return "编码器位置与系统位置偏差过大";
		else if (HOME_ERR_SET == Error_) return "回原点设定参数异常";
		else if (HOME_ERR_STATE == Error_) return "回原点状态异常";
		else if (HOME_ERR_MODE == Error_) return "回原点模式选择错误";
		else if (HOME_ERR_AXIS == Error_) return "回原点发生轴故障";
		else if (GROUP_ERR_DATA_SET == Error_) return "速度规划参数超轴上限无法执行五轴联动";
		else if (GROUP_ERR_TRAJ_FAIL == Error_) return "多轴轨迹规划失败";
		else if (GROUP_ERR_AXIS_FAIL == Error_) return "单轴故障无法进行联动规划";
		else if (GROUP_ERR_UPDATE_DATA == Error_) return "速度规划参数没超轴上限但是设定失败无法执行五轴联动";
		else if (GROUP_ERR_NEAR_LIMIT == Error_) return "多轴运动过程中即将超限位";
		else if (GROUP_ERR_RTCP_FAIL == Error_) return "刀尖位置不可到达";
		else if (GROUP_ERR_GET_FK == Error_) return "刀尖位置计算失败";
		else if (GROUP_ERR_CONNECT_AXIS == Error_) return "未绑定轴运动句柄 无法进行多轴操作";
		else if (GROUP_ERR_CONNECT_NUM == Error_) return "绑定轴数量不满足无法执行高阶功能";
		else if (GROUP_ERR_CIRCLE_PAR == Error_) return "圆弧参数异常无法规划轨迹";
		else if (GROUP_ERR_RTCP_MODE == Error_) return "未开启RTCP模式";
		else if (GROUP_ERR_TARGET_SAME == Error_) return "起点终点位置距离太小";
		else if (GROUP_ERR_TARGET_OVER == Error_) return "目标起点位置偏差过大";
		else if (GROUP_ERR_TARGET_VEL == Error_) return "队列即将为空但目标速度不为0";
		else if (GROUP_ERR_TRAJ_EMPTY == Error_) return "队列为空";
		else if (GROUP_ERR_LIMIT_ORVE == Error_) return "轴终点位置不可达";
		else if (GROUP_ERR_STILL_MOVING == Error_) return "联动正在进行无法执行新的运动";
		else if (RTCP_A4_VECTOR_ERROR == Error_) return "第四主轴初始向量设定错误";
		else if (RTCP_A5_VECTOR_ERROR == Error_) return "第五主轴初始向量设定错误";
		else if (RTCP_TOOL_VECTOR_ERROR == Error_) return "主轴初始向量设定错误";
		else if (RTCP_PARAMETER_ERROR == Error_) return "RTCP参数设定异常";
		else if (RTCP_VECTOR_SOLVER_ERROR == Error_) return "无法找到合适的角度解";
		else return "UNKNOWN ERROR";
	}
	virtual const char* StrSubError(const int Error_) const
	{
		if (PARA_NONE == Error_) return "与参数无关";
		else if (PARA_SPEED == Error_) return "运动加加速度、加速度、速度参数";
		else if (PARA_MAXSPEED == Error_) return "运动最大加加速度、加速度、速度参数";
		else if (PARA_ENC_UNIT == Error_) return "编码器位数";
		else if (PARA_MECHINE == Error_) return "机械参数";
		else if (PARA_LIMIT == Error_) return "限位";
		else if (PARA_RTCP == Error_) return "Rtcp参数";
		else if (PARA_CMODE == Error_) return "控制模式";
		else if (PARA_GEOMETRY == Error_) return "几何信息不满足要求";
		else return "未知参数";
	}

private:
	int	ErrorNum;
	int ErrorSubNum;


};

	


}
