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

	int **neighbor;//����ÿ���ڵ���ھ�
	int size; //�ڵ�ĸ���
	int maxDeg;//��·������
	int *deg;//ÿ���ڵ�Ķ�
	int *remNode;//���Ƴ��Ľڵ�
	int *newDeg;//ÿһ���Ƴ��ڵ��ÿ������¶�
	float *nodeWeight;//ÿ���ڵ��Ȩ��
	float **edgeWeight;//ÿ���ߵ�Ȩֵ
	int **remEdge;
	int numOfEdge;//�ߵ���Ŀ
	char *name;//��luo����

	//���캯�����˺�����ȡ�����ļ���������ͼ�� ���ҵ�����ı��������̶ȣ���Ϊÿ���ڵ����һ��ID��
	//�������nam�������ļ������ơ�
	Network(char *nam);

	//���캯��
	Network(void);

	//��������
	~Network(void);

	//�ù��ܳ�ʼ�������һЩ��������Щ����Ӧ�ڶ��������ȷ���� ��Щ�����Ǳߺͽڵ��Ȩ�أ�ÿ���ڵ���¶����ȵ�����ȣ���ȷ����ʼʱû��ɾ���Ľڵ�
	void skeletonInitialValue();


	//��������t�޼����硣 ���Բ��ƻ�����Ϣ�ķ�ʽɾ��ȷ���Ľڵ㣬�Թ�������ĹǼܡ�
	//�������t������Ҫ��������ɾ���Ľڵ�����ȡ� t���Ϊ100��
	//�����ͼ�ĹǼ�
	void makeSkeleton(int t);

	//�˺�����������ɾ��һ���ڵ�
    //�����һ��û���κ�һ���ڵ�����硣
	void removeDegOne();

	//�˺���ɾ���ȵ��ڻ�С��ȷ��������ȵĽڵ�
	//���������ȷ��Ҫ��������ɾ���ڵ㡣
	//�����һ�����磬��ڵ�Ķ���С�ڻ�������������
	void removeDeg(int degree);

	//�ҵ���ȷ����IF�Ķ�Ӧ���ơ� //�������id����������Ѱ�������ƵĽڵ��ID�� //�������Ӧ�ڵ�����ơ�
	string getName(int id) {
		return mNames[id];
	};

	//�ҵ���ȷ�����ƵĶ�ӦID�� //�����������������Ҫ������ID�Ľڵ�����ơ� //�������Ӧ�ڵ��ID��
	int getId(string name) {
		return mapName[name];
	};
private:

};
