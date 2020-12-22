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
			//û���ҵ��ļ�
			cout << "Error!" << endl;
			exit(1);
		}

		numOfEdge = 0;
		maxDeg = 0; //���������Ķ�

		while (getline(networkFile, line)) {
			//�������ļ�
			istringstream tokenizer(line);
			string token;

			//��ȡ��һ�еĵ�һ���ڵ�
			getline(tokenizer, token, '\t');
			
			if (token.length() == 0) {
				cout << "Error: ��һ��û�нڵ�" << endl;
				exit(1);
			}

			//Ϊ����ĵ�һ���ڵ����һ��ID
			mapName.insert(MapString2Int::value_type(token, (int)mapName.size()));
			id1 = mapName[token];

			//Ϊ���ھӴ�����Ӧ���б�
			mNames.insert(MapInt2Name::value_type(id1, token));
			list<int>neighbs1;
			mapNeighbor.insert(MapInt2List::value_type(id1, neighbs1));

			//��ȡ�еĵڶ����ڵ�

			getline(tokenizer, token, '\t');
			if (token.length() == 0) {
				cout << "�ڶ���û�нڵ�" << endl;
				exit(1);
			}
			if (token.at(token.length() - 1 == 13)) {
				token = token.substr(0, token.length() - 1);
			}

			//Ϊ����ĵڶ����ڵ����ID
			mapName.insert(MapString2Int::value_type(token, (int)mapName.size()));
			id2 = mapName[token];

			//Ϊ���ھӴ�����Ӧ���б�
			mNames.insert(MapInt2Name::value_type(id2, token));
			list<int>neighbs2;
			mapNeighbor.insert(MapInt2List::value_type(id2, neighbs2));

			//����һ���ڵ����ڶ����ڵ���ھ��б���֮��Ȼ��
			mapNeighbor[id1].push_front(id2);
			mapNeighbor[id2].push_front(id1);
		}

		size = mapName.size();//����ڵ����Ŀ

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

		//���ҵ������ģ���ʼ��һЩ����
		nodeWeight = new float[size];
		newDeg = new int[size];//�����Ǽ�ʱÿ���ڵ���¶�
		edgeWeight = new float*[size];//�����Ǽ�ʱ�ı��ÿ���ߵ�Ȩ��
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
		cerr << "�����ļ�����" << endl;
		exit(1);
	}

}


//�˺�����ʼ�������һЩ��������Щ����Ӧ�ڶ��������ȷ���� ��Щ�����Ǳߺͽڵ��Ȩ�أ�ÿ���ڵ���¶����ȵ�����ȣ���ȷ����ʼʱû��ɾ���Ľڵ�
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

//��������t�޼����硣 ���Բ��ƻ�����Ϣ�ķ�ʽɾ��ȷ���Ľڵ㣬�Թ�������ĹǼܡ�
//�������t������Ҫ��������ɾ���Ľڵ�����̶ȡ� t���Ϊ10��
//�����ͼ�ĹǼ�

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
	while (flag) {//������ڶ�Ϊһ�Ľڵ�
		flag = false;
		//�����ڵ�
		for (int c1 = 0; c1 < size; c1++) {
			if (newDeg[c1] == 1) {  
				//�Ƴ�
				for (int c2 = 0; c2 < deg[c1]; c2++) {
					if (remNode[neighbor[c1][c2]] == 0) {
						nodeWeight[neighbor[c1][c2]] += nodeWeight[c1] + edgeWeight[c1][neighbor[c1][c2]];
						newDeg[neighbor[c1][c2]]--;
						newDeg[c1]--;
					}	
				}
				remNode[c1] = 1;//node ���Ƴ�
			}
		}
		//����Ƴ����������Ƿ������һ���ڵ�
		for (int c1 = 0; c1 < size; c1++) {
			if (newDeg[c1] == 1)
				flag = true;
		}
	}
	//ɾ������Ϊ1�Ľڵ��ɾ���ޱߵĽڵ�
	for (int c1 = 0; c1 < size; c1++) {
		if (newDeg[c1] < 1 && remNode[c1] == 0) {
			remNode[c1] = 1;
		}
	}
}

//�˺���ɾ���ȵ��ڻ�С��ȷ��������ȵĽڵ�
//���������ȷ��Ҫ��������ɾ���ڵ㡣
//�����һ�����磬��ڵ�Ķ���С�ڻ�������������

void Network::removeDeg(int degree)
{
	int *ngh = new int[degree];//��Ϊd�Ķ�����ھ�
	int nghsize;//�ھ�����
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
		//����Ƴ����Ƿ�����˶������ڻ�С����������Ľڵ�
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
	//ɾ������Ϊ1�Ľڵ��ɾ���ޱߵĽڵ�
	for (int c1 = 0; c1 < size; c1++) {
		if (newDeg[c1] < 1 && remNode[c1] == 0) {
			remNode[c1] = 1;
		}
	}
	delete[] ngh;
}

//���캯��
Network::Network(void)
{}

//��������
Network::~Network(void)
{}

