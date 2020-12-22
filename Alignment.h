#pragma once

#include "Network.h"
#include <iostream>
#include <cstdlib>
#include "math.h"
#include <time.h>

using namespace std;

class Alignment {
public:
	//�������뷽ʽ
	float EC, S3;
	int CCCV, CCCE;

	int *alignment;//����������������
	int *comp;//������뷽ʽ�е������������
	int maxComp;//������뷽ʽ��������Ӳ���
	int maxDeg;//���������е�����
	bool reverse;//�����ĸ������Ǵ����磻
	float **blast;

	Network network1;
	Network network2;

	//���캯��
	//�ҵ���С�������������������̶�
	//����������net1��net2�������ļ�
	Alignment(Network net1, Network net2);

	//���캯��
	Alignment(void);

	//��������
	~Alignment(void);

	//������������Ľڵ�֮��������������a��ӳ�䡣
	//�������aa�ڽ�����������ڵ�ʱ����edgeWeight���ӡ� aӦ����0��1֮�䡣
	void align(double lambda, double alpha);

	//������������EC����Ե��ȷ�ԣ���IC��������ȷ�ԣ���NC���ڵ���ȷ�ԣ���CCCV��CCCE�����ڶ���ͱ�Ե����󹫹���ͨ����
	void evaluate(void);

	//����CCCV
	//����·�ߵ���󹫹�������ͼ�Ķ�����
	int getCCCV(void);

	//����������CCCE
	//����·�ߵ���󹫹�������ͼ�ı���
	int getCCCE(void);

	float getEC(void);

	float getS3(void);

	//�����������ȷ��Ҫд�������ļ���
	void outputEvaluate(string name);

	//ʹ����������������ļ��д�ӡ���뷽ʽ��ӳ�䣩
	//�����������ȷ��Ҫд��ӳ����ļ���
	void outputAlignment(string name);

	void readblast(string blastFile);




};