#pragma once
#include<iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "math.h"
#include <time.h>
#include <map>
#include <list>

using namespace std;


class Network {
	typedef map<string, int, less<string>>MapString2Int;
	typedef map<int, list<int>, less<int>>MapInt2List;
	typedef map<int, string, less<int>>MapInt2Name;

	MapInt2Name mNames;

public:
	MapString2Int mapName;

	int **neighbor;//网络每个节点的邻居
	int size; //节点的个数
	int maxDeg;//网路的最大度
	int *deg;//每个节点的度
	int *remNode;//被移除的节点
	int *newDeg;//每一次移除节点后每个点的新度
	float *nodeWeight;//每个节点的权重
	float **edgeWeight;//每条边的权值
	int **remEdge;
	int numOfEdge;//边的数目
	char *name;//网luo的名

	//构造函数，此函数获取网络文件并构造其图。 它找到网络的边数和最大程度，并为每个节点分配一个ID。
	//输入参数nam是网络文件的名称。
	Network(char *nam);

	//构造函数
	Network(void);

	//析构函数
	~Network(void);

	//该功能初始化网络的一些参数，这些参数应在对齐过程中确定。 这些参数是边和节点的权重，每个节点的新度首先等于其度，并确定开始时没有删除的节点
	void skeletonInitialValue();


	//根据输入t修剪网络。 它以不破坏其信息的方式删除确定的节点，以构成网络的骨架。
	//输入参数t是我们要从网络中删除的节点的最大度。 t最多为100。
	//结果是图的骨架
	void makeSkeleton(int t);

	//此函数从网络中删除一级节点
    //结果是一个没有任何一级节点的网络。
	void removeDegOne();

	//此函数删除度等于或小于确定的输入度的节点
	//输入参数度确定要从网络中删除节点。
	//结果是一个网络，其节点的度数小于或等于输入度数。
	void removeDeg(int degree);

	//找到已确定的IF的对应名称。 //输入参数id是我们正在寻找其名称的节点的ID。 //输出是相应节点的名称。
	string getName(int id) {
		return mNames[id];
	};

	//找到已确定名称的对应ID。 //输入参数名称是我们要查找其ID的节点的名称。 //输出是相应节点的ID。
	int getId(string name) {
		return mapName[name];
	};
private:

};
