#pragma once

//Signal On/Off
#define ON	1
#define OFF 0

//Encoder type
#define ABSOLUTE_ENCODER	1
#define INCREMENT_ENCODER	2

//Move type
#define LINEAR 1
#define ROTATE 2

//Control Mode
#define MODE_CSP 1
#define MODE_CSV 2
#define MODE_CST 3

//Rtcp Mode type
#define TABLE_TILTING 1//双转台类型
#define SPINDLE_TILTING 2//双摆头类型
#define TABLE_SPINDLE_TILTING 3//一摆一转

//Rotation Type
#define ROTATION_CA 1//第四轴是C第五轴是A
#define ROTATION_CB 2//第四轴是C第五轴是B
#define ROTATION_AB 3
#define ROTATION_BA 4
#define ROTATION_BC 5//第四轴是B第五轴是C

//Motor Error message
#define ERROR_MSG_NONE			0     //正常
#define ERROR_MSG_DRIVER_ERR	1	  //驱动器报警
#define ERROR_MSG_FS_WARN		2	  //正软限位
#define ERROR_MSG_RS_WARN		3	  //负软限位
#define ERROR_MSG_FE_WARN		4	  //跟随误差过过大
#define ERROR_MSG_FWD_WARN		5	  //正硬限位
#define ERROR_MSG_REV_WARN		6	  //负硬限位
#define ERROR_MSG_DATA_SET		7	  //参数设定异常
#define ERROR_MSG_NOT_ENABLE	8	  //未使能
#define ERROR_MSG_MOTION		9	  //运动过程中即将超限位
#define ERROR_MSG_ZEROSPEED		10	  //未设定运动速度
#define ERROR_MSG_TRAJ_FAIL		11	  //轨迹规划失败
#define ERROR_MSG_TRAJ_OVERLIMIT 12	  //轨迹规划超限位
#define ERROR_MSG_SPEED_OVER	13	  //速度超限制
#define ERROR_MSG_UNIDLE		14	  //未静止无法禁止修改
#define ERROR_MSG_DANGER_SPEED  15	  //周期间命令超速
#define ERROR_MSG_POS_DIFF      16	  //编码器位置与记录位置偏差过大

//Home Error message
#define HOME_ERR_MSG_NONE		0//正常
#define HOME_ERR_SET			50//参数异常
#define HOME_ERR_STATE			51//状态异常
#define HOME_ERR_MODE			52//模式选择错误
#define HOME_ERR_AXIS			53//轴故障终止回原

/* HOME Mode Select*/
#define HOME_FINE_Z_PLUSE            1//寻找Z脉冲
#define HOME_FINE_DATUM_IN           2//寻找原点开关
#define HOME_NO_LATCH_MOVE           3//触发原点开关直接停止
/* HOME Option Select*/
#define HOME_NONE_OPTION			 0
#define HOME_ABSOLUTE_ENCODER        1
#define HOME_NO_REHOME               2

//联动运动错误
#define GROUP_ERR_NONE			0//正常
#define GROUP_ERR_DATA_SET		101//速度规划参数超轴上限无法执行五轴联动
#define GROUP_ERR_TRAJ_FAIL		102//轨迹规划失败
#define GROUP_ERR_AXIS_FAIL		103//单轴故障无法进行联动规划
#define GROUP_ERR_UPDATE_DATA	104//速度规划参数没超轴上限但是设定失败无法执行五轴联动
#define GROUP_ERR_NEAR_LIMIT	105//运动过程中即将超限位
#define GROUP_ERR_RTCP_FAIL		106//刀尖位置不可到达，Rtcp规划失败
#define GROUP_ERR_GET_FK		107//刀尖位置计算失败
#define GROUP_ERR_CONNECT_AXIS	108//未绑定轴运动句柄
#define GROUP_ERR_CONNECT_NUM	109//绑定轴数量不足
#define GROUP_ERR_CIRCLE_PAR	110//圆弧参数异常无法规划轨迹
#define GROUP_ERR_RTCP_MODE	    111//未开启RTCP模式
#define GROUP_ERR_TARGET_SAME   112//起点终点位置距离太小
#define GROUP_ERR_TARGET_OVER   113//目标起点位置偏差过大
#define GROUP_ERR_TARGET_VEL    114//队列即将为空但目标速度不为0
#define GROUP_ERR_TRAJ_EMPTY    115//队列为空
#define GROUP_ERR_LIMIT_ORVE    116//轴终点位置不可达
#define GROUP_ERR_STILL_MOVING  117//联动正在进行无法执行新的运动
#define GROUP_ERR_MODIYF_FRAME  118//修改坐标系异常
#define GROUP_ERR_TRAJ_RUNNING  119//正在运行无法清除队列

//Rtcp parameter setting error
#define RTCP_A4_VECTOR_ERROR    200//第四主轴初始向量设定错误
#define RTCP_A5_VECTOR_ERROR	201//第五主轴初始向量设定错误
#define RTCP_TOOL_VECTOR_ERROR	202//主轴初始向量设定错误
#define RTCP_PARAMETER_ERROR	203//RTCP参数设定异常
#define RTCP_VECTOR_SOLVER_ERROR 204//无法找到合适的角度解

