#pragma once

#include "Network.h"
#include <iostream>
#include <cstdlib>
#include "math.h"
#include <time.h>

using namespace std;

class Alignment {
public:
	//评估对齐方式
	float EC, S3;
	int CCCV, CCCE;

	int *alignment;//两个网络对齐的数组
	int *comp;//结果对齐方式中的连接组件数组
	int maxComp;//结果对齐方式的最大连接部分
	int maxDeg;//两个网络中的最大度
	bool reverse;//决定哪个网络是大网络；
	float **blast;

	Network network1;
	Network network2;

	//构造函数
	//找到较小的网络和输入网络的最大程度
	//输入是网络net1和net2的两个文件
	Alignment(Network net1, Network net2);

	//构造函数
	Alignment(void);

	//析构函数
	~Alignment(void);

	//生成两个网络的节点之间相对于输入参数a的映射。
	//输入参数aa在将分数分配给节点时控制edgeWeight因子。 a应该在0到1之间。
	void align(double lambda, double alpha);

	//计算评估度量EC（边缘正确性），IC（交互正确性），NC（节点正确性），CCCV和CCCE（关于顶点和边缘的最大公共连通子域）
	void evaluate(void);

	//计算CCCV
	//返回路线的最大公共连接子图的顶点数
	int getCCCV(void);

	//计算评估量CCCE
	//返回路线的最大公共连接子图的边数
	int getCCCE(void);

	float getEC(void);

	float getS3(void);

	//输入参数名称确定要写入结果的文件。
	void outputEvaluate(string name);

	//使用输入参数名称在文件中打印对齐方式（映射）
	//输入参数名称确定要写入映射的文件。
	void outputAlignment(string name);

	void readblast(string blastFile);




};