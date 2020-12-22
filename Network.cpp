#pragma once

#include "Network.h"
#include "Alignment.h"
#include<iostream>

Network::Network(char *nam) {

	try
	{
		name = nam;
		string fileName = name;
		string line;
		string token;
		int id1, id2;

		MapInt2List mapNeighbor;

		ifstream networkFile(fileName.c_str());
		//ifstream networkFile(fileName);


		if (!networkFile) {
			//没有找到文件
			cout << "Error!" << endl;
			exit(1);
		}

		numOfEdge = 0;
		maxDeg = 0; //网络中最大的度

		while (getline(networkFile, line)) {
			//读网络文件
			istringstream tokenizer(line);
			string token;

			//读取第一行的第一个节点
			getline(tokenizer, token, '\t');
			
			if (token.length() == 0) {
				cout << "Error: 第一行没有节点" << endl;
				exit(1);
			}

			//为网络的第一个节点分配一个ID
			mapName.insert(MapString2Int::value_type(token, (int)mapName.size()));
			id1 = mapName[token];

			//为其邻居创建相应的列表
			mNames.insert(MapInt2Name::value_type(id1, token));
			list<int>neighbs1;
			mapNeighbor.insert(MapInt2List::value_type(id1, neighbs1));

			//读取行的第二个节点

			getline(tokenizer, token, '\t');
			if (token.length() == 0) {
				cout << "第二列没有节点" << endl;
				exit(1);
			}
			if (token.at(token.length() - 1 == 13)) {
				token = token.substr(0, token.length() - 1);
			}

			//为网络的第二个节点分配ID
			mapName.insert(MapString2Int::value_type(token, (int)mapName.size()));
			id2 = mapName[token];

			//为其邻居创建相应的列表
			mNames.insert(MapInt2Name::value_type(id2, token));
			list<int>neighbs2;
			mapNeighbor.insert(MapInt2List::value_type(id2, neighbs2));

			//将第一个节点插入第二个节点的邻居列表，反之亦然。
			mapNeighbor[id1].push_front(id2);
			mapNeighbor[id2].push_front(id1);
		}

		size = mapName.size();//网络节点的数目

		neighbor = new int*[size];
		deg = new int[size];

		list<int>tempList;
		list<int>::iterator it;

		for (int i = 0; i < size; i++) {
			tempList = mapNeighbor[i];
			tempList.sort();
			tempList.unique();

			deg[i] = tempList.size();
			neighbor[i] = new int[deg[i]];

			numOfEdge += deg[i];

			if (deg[i] > maxDeg) {
				maxDeg = deg[i];
			}

			int j;
			for (j = 0, it = tempList.begin(); it != tempList.end(); it++, j++) {
				neighbor[i][j] = *it;
			}
		}

		numOfEdge = numOfEdge / 2;
		cout << nam << ":" << size << "nodes , " << numOfEdge << "edges\n";

		//在找到网络规模后初始化一些变量
		nodeWeight = new float[size];
		newDeg = new int[size];//制作骨架时每个节点的新度
		edgeWeight = new float*[size];//制作骨架时改变的每个边的权重
		remEdge = new int*[size];
		remNode = new int[size];
		for (int c1 = 0; c1 < size; c1++) {
			edgeWeight[c1] = new float[size];
		}
		for (int c1 = 0; c1 < size; c1++) {
			remEdge[c1] = new int[size];
		}

		skeletonInitialValue();

		for (int i = 0; i < size; i++) {
			cout <<i<<": "<<neighbor[i]<<" ;";
		}

	}
	catch (const std::exception&)
	{
		cerr << "导入文件错误" << endl;
		exit(1);
	}

}


//此函数初始化网络的一些参数，这些参数应在对齐过程中确定。 这些参数是边和节点的权重，每个节点的新度首先等于其度，并确定开始时没有删除的节点
void Network::skeletonInitialValue()
{
	for (int c1 = 0; c1 < size; c1++) {
		nodeWeight[c1] = 0;
	}

	for (int c1 = 0; c1 < size; c1++) {
		for (int c2 = 0; c2 < size; c2++) {
			edgeWeight[c1][c2] = 0;
		}
	}

	for (int c1 = 0; c1 < size; c1++) {
		for (int c2 = 0; c2 < deg[c1]; c2++) {
			edgeWeight[c1][neighbor[c1][c2]] = 1;
		}
	}

	for (int c1 = 0; c1 < size; c1++) {
		remNode[c1] = 0;
	}

	for (int c1 = 0; c1 < size; c1++) {
		newDeg[c1] = deg[c1];
	}
}

//根据输入t修剪网络。 它以不破坏其信息的方式删除确定的节点，以构成网络的骨架。
//输入参数t是我们要从网络中删除的节点的最大程度。 t最多为10。
//结果是图的骨架

void Network::makeSkeleton(int t)
{
	removeDegOne();
	for (int c1 = 0; c1 < t; c1++) {
		removeDeg(c1);
	}

	int max = 0;
	for (int c = 0; c < size; c++) {
		if (remNode[c] == 0) {
			if (max < newDeg[c]) {
				max = newDeg[c];
			}
		}
	}


}

//this function removes nodes with degree one from the network
//result is a network without any node of degree one.

void Network::removeDegOne()
{
	bool flag = true;
	while (flag) {//如果存在度为一的节点
		flag = false;
		//遍历节点
		for (int c1 = 0; c1 < size; c1++) {
			if (newDeg[c1] == 1) {  
				//移除
				for (int c2 = 0; c2 < deg[c1]; c2++) {
					if (remNode[neighbor[c1][c2]] == 0) {
						nodeWeight[neighbor[c1][c2]] += nodeWeight[c1] + edgeWeight[c1][neighbor[c1][c2]];
						newDeg[neighbor[c1][c2]]--;
						newDeg[c1]--;
					}	
				}
				remNode[c1] = 1;//node 被移除
			}
		}
		//检查移除后网络中是否产生了一级节点
		for (int c1 = 0; c1 < size; c1++) {
			if (newDeg[c1] == 1)
				flag = true;
		}
	}
	//删除度数为1的节点后删除无边的节点
	for (int c1 = 0; c1 < size; c1++) {
		if (newDeg[c1] < 1 && remNode[c1] == 0) {
			remNode[c1] = 1;
		}
	}
}

//此函数删除度等于或小于确定的输入度的节点
//输入参数度确定要从网络中删除节点。
//结果是一个网络，其节点的度数小于或等于输入度数。

void Network::removeDeg(int degree)
{
	int *ngh = new int[degree];//度为d的顶点的邻居
	int nghsize;//邻居数量
	bool flag = true;
	int d = degree;
	float temp;
	while (flag)
	{
		flag = false;
		ngh = new int[d];
		for(int c1=0;c1<size;c1++)
			if (newDeg[c1] = d) {
				temp = nodeWeight[c1];
				nghsize = 0;
				for(int c2=0;c2<deg[c1];c2++)
					if (remNode[neighbor[c1][c2]] == 0) {
						ngh[nghsize] = neighbor[c1][c2];
						nghsize++;
						temp += edgeWeight[c1][neighbor[c1][c2]];
					}
				newDeg[c1] = 0;
				for (int c2 = 0; c2 < nghsize; c2++) {
					newDeg[ngh[c2]]--;
					for (int c3 = c2 + 1; c3 < nghsize; c3++) {
						edgeWeight[ngh[c2]][ngh[c3]] += temp / (d*(d - 1) / 2);
						edgeWeight[ngh[c3]][ngh[c2]] = edgeWeight[ngh[c2]][ngh[c3]];
					}
				}
				remNode[c1] = 1;
			}
		//检查移除后是否产生了度数等于或小于输入度数的节点
		for (int c2 = degree; c2 > 1; c2--)
			if (!flag) {
				for (int c1 = 0; c1 < size; c1++)
					if (newDeg[c1] == c2) {
						flag = true;
						d = c2;
						break;
					}
			}
			else
				break;
		if(!flag)
			for (int c1 = 0; c1 < size; c1++) {
				if (newDeg[c1] == 1) {
					flag = true;
					d = degree;
					removeDegOne();
					break;
				}
			}
	}
	//删除度数为1的节点后删除无边的节点
	for (int c1 = 0; c1 < size; c1++) {
		if (newDeg[c1] < 1 && remNode[c1] == 0) {
			remNode[c1] = 1;
		}
	}
	delete[] ngh;
}

//构造函数
Network::Network(void)
{}

//析构函数
Network::~Network(void)
{}

