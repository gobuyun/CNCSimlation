#pragma once

#define DoubleErr 1e-8
#define PathEsp 1e-5
//判断数值正负号
#define SIGN(x) (((x + DoubleErr) < 0) ? -(1) : (1))
//取余数
#define MOD(x,y) (x-int(x/y))
//比较大小返回最小值
#define MIN(x,y) (x < y ? x : y)
