#include"Alignment.h"
#include<fstream>
#include<cstdlib>

using namespace std;

//构造函数
//找到较小的网络和输入网络的最大程度
//输入是网络net1和net2的两个文件
Alignment::Alignment(Network net1, Network net2)
{
	//比较两个网络发现较大的
	if (net1.size > net2.size) {
		reverse = true;
		network1 = net2;
		network2 = net1;
	}
	else {
		reverse = false;
		network1 = net1;
		network2 = net2;
	}
	//最大网络度
	if (network1.maxDeg > network2.maxDeg)
		maxDeg = network1.maxDeg;
	else
		maxDeg = network2.maxDeg;

	blast = new float*[network1.size];
	for (int c = 0; c < network1.size; c++) {
		blast[c] = new float[network2.size];
	}
	for (int c1 = 0; c1 < network1.size; c1++) {
		for (int c2 = 0; c2 < network2.size; c2++) {
			blast[c1][c2] = 0;
		}
	}
}

void Alignment::readblast(string blastFile)
{
	float**temp = new float *[network1.size];
	for (int c = 0; c < network1.size; c++) {
		temp[c] = new float[network2.size];
	}
	for (int c1 = 0; c1 < network1.size; c1++) {
		for (int c2 = 0; c2 < network2.size; c2++) {
			temp[c1][c2] = 0;
		}
	}

	float max = 0;
	//blast value

	ifstream inputFile;
	string token1, token2, line;
	float token3;
	inputFile.open(blastFile.c_str());
	while (getline(inputFile, line)) {
		istringstream tokenier(line);
		getline(tokenier, token1, '\t');
		getline(tokenier, token2, '\t');
		tokenier >> token3;
		if (max < token3)max = token3;
		//temp[network1.mapName[token1]][network2.mapName[token2]]=token3;
		temp[network1.mapName[token1]][network2.mapName[token2]] = token3;
	}

	for (int c1 = 0; c1 < network1.size; c1++)
		for (int c2 = 0; c2 < network2.size; c2++)
			blast[c1][c2] = temp[c1][c2] / max;
}

//生成两个网络的节点之间相对于输入参数a的映射。
//输入参数aa在将分数分配给节点时控制edgeWeight因子。 a应该在0到1之间。

void Alignment::align(double lambda,double alpha)
{
	bool flag;//检查较小网络的所有节点是否对齐？

	//temporary
	float temp;
	float a1, a11;
	float a2, a22;
	float MINSCORE = -10000;
	int coeff;
	if (network2.numOfEdge > network1.numOfEdge) {
		coeff = network2.numOfEdge / network1.numOfEdge;
	}
	else {
		coeff = network1.numOfEdge / network2.numOfEdge;
	}
	int maxNode;
	bool *alignNodes1 = new bool[network1.size];//小网络节点比对
	bool *alignNodes2 = new bool[network2.size];//大网络节点的比对
	alignment = new int[network1.size];//比对数组
	float *nodeScore1 = new float[network1.size];
	float *nodeScore2 = new float[network2.size];
	double **alignScore = new double*[network1.size];
	int *best = new int[network1.size];//最佳比对的分数

	//初始化
	for (int c1 = 0; c1 < network1.size; c1++)
		alignScore[c1] = new double[network2.size];
	for (int c1 = 0; c1 < network1.size; c1++)
		alignNodes1[c1] = false;
	for (int c1 = 0; c1 < network2.size; c1++)
		alignNodes2[c1] = false;
	for (int c1 = 0; c1 < network1.size; c1++)
		alignment[c1] = -1;
	for (int c1 = 0; c1 < network1.size; c1++)
		best[c1] = -1;

	ofstream NS;
	//从两个网络初始化nodeScore
	for (int c1 = 0; c1 < network1.size; c1++)
		nodeScore1[c1] = (1 - lambda)*network1.nodeWeight[c1];
	for (int c1 = 0; c1 < network2.size; c1++)
		nodeScore2[c1] = (1 - lambda)*network2.nodeWeight[c1];

	//找到最大得分，
	//finding node score
	for (int c1 = 0; c1 < network1.size; c1++) {
		for (int c2 = 0; c2 < network1.size; c2++)
			nodeScore1[c1] += lambda * network1.edgeWeight[c1][c2];
	}

	for (int c1 = 0; c1 < network2.size; c1++) {
		for (int c2 = 0; c2 < network2.size; c2++)
			nodeScore2[c1] += lambda * network2.edgeWeight[c1][c2];
	}

	//	第一个网络
	float max = -10000;
	for (int c1 = 0; c1 < network1.size; c1++) {
		if (max < nodeScore1[c1])
			max = nodeScore1[c1];
	}

	//	第二个网络
	for (int c1 = 0; c1 < network2.size; c1++) {
		if (max < nodeScore2[c1])
			max = nodeScore2[c1];
	}

	//相对于max进行归一化
	for (int c1 = 0; c1 < network1.size; c1++) {
		nodeScore1[c1] = nodeScore1[c1] / max;
	}

	for (int c1 = 0; c1 < network2.size; c1++) {
		nodeScore2[c1] = nodeScore2[c1] / max;
	}
	//end

	//寻找比对分数
	for(int c1=0;c1<network1.size;c1++)
		for (int c2 = 0; c2 < network2.size; c2++) {
			alignScore[c1][c2] = (nodeScore1[c1] > nodeScore2[c2]) ? nodeScore2[c2] : nodeScore1[c1];
			alignScore[c1][c2] = alpha * (alignScore[c1][c2]);
			alignScore[c1][c2] += (1 - alpha)*blast[c1][c2];//添加相似分数
		}

	int counter = 0;
	flag = true;
	int temp1, temp2;

	int counteralign = 0;
	while (flag) {
		//确定的迭代中有一些未对齐的节点
		//找到alignscore每行的最大值并将其保存在数组“ best”中
		for (int c1 = 0; c1 < network1.size; c1++)
		{
			if (!alignNodes1[c1])
			{
				temp = MINSCORE;
				for(int c2=0;c2<network2.size;c2++)
					if (temp < alignScore[c1][c2] && !alignNodes2) {
						////????????
						if (alignScore[c1][c2] == temp) {
							temp1 = (network1.deg[c1] > network2.deg[c2]) ? network2.deg[c2] / network1.deg[c1] : network1.deg[c1] / network2.deg[c2];
							temp2 = (network1.deg[c1] > network2.deg[best[c1]]) ? network2.deg[best[c1]] / network1.deg[c1] : network1.deg[c1] / network2.deg[best[c1]];
							if (temp1 > temp2) {
								best[c1] = c2;
								temp = alignScore[c1][c2];
							}


		
						}
						else
						{
							best[c1] = c2;
							temp = alignScore[c1][c2];
						}
					}

			}
		}

		//开始对齐
		//找到数组“ best”的最大值，这表示矩阵“ alignScore”中的最佳分数

		temp = MINSCORE;
		flag = false;

		for(int c1=0;c1<network1.size;c1++)
			if (temp < alignScore[c1][best[c1]]&&!alignNodes1[c1]&&!alignNodes2[best[c1]]) {
				flag = true;//仍然有节点没有被对齐
				if (alignScore[c1][best[c1]] == temp) {
					if (network1.deg[c1] > network1.deg[maxNode]) {
						maxNode = c1;
						temp = alignScore[c1][best[c1]];
					}
				}
				else {
					temp = alignScore[c1][best[c1]];
					maxNode = c1;
				}
			}
		if (flag) {
			//在第一个网络中仍有节点没有被对齐
			alignment[maxNode] = best[maxNode];//对齐两个节点

			alignNodes1[maxNode] = true;
			alignNodes2[best[maxNode]] = true;
			//将度为1节点邻居对齐

			for(int j=0;j<network1.deg[maxNode];j++)
				for (int k = 0; k < network2.deg[best[maxNode]]; k++)
					if (!alignNodes1[network1.neighbor[maxNode][j]] && !alignNodes2[network2.neighbor[best[maxNode]][k]])
					{
						if (network1.deg[network1.neighbor[maxNode][j]] == 1 && network2.deg[network2.neighbor[best[maxNode]][k]])
						{
							alignment[network1.neighbor[maxNode][j]] = network2.neighbor[best[maxNode]][k];
							alignNodes1[network1.neighbor[maxNode][j]] = true;
							alignNodes2[network2.neighbor[best[maxNode]][k]] = true;
						}
					}
			//更新对齐分数
			for(int c1 =0;c1<network1.deg[maxNode];c1++)
				for(int c2=0;c2<network2.deg[best[maxNode]];c2++)
					alignScore[network1.neighbor[maxNode][c1]][network2.neighbor[best[maxNode]][c2]] = alignScore[network1.neighbor[maxNode][c1]][network2.neighbor[best[maxNode]][c2]] + (coeff / max);
		}
		counter = counter + 1;
		if (counter % 1000 == 0)
			cout << counter << endl;
	}//end flag

	////内存泄漏
	delete []alignNodes1;
	delete []alignNodes2;
	delete[]nodeScore1;
	delete[]nodeScore2;
	delete[]best;
	for (int i = 0; i < network1.size; i++) {
		delete[]alignScore[i];
	}
	delete[]alignScore;

	evaluate();///计算测量评估
}

//计算评估度量EC（边缘正确性），IC（交互正确性），NC（节点正确性），CCCV和CCCE（关于顶点和边缘的最大公共连通子域）

void Alignment::evaluate(void)
{
	CCCV = getCCCV();
	CCCE = getCCCE();
	EC = getEC();
	S3 = getS3();
}

//计算CCCV
//返回路线的最大公共连接子图的顶点数
int Alignment::getCCCV(void)
{
	int *subGraph;
	int compNum = 1;//连接的组件数目
	int *q = new int[network1.size];//已处理的节点
	comp = new int[network1.size];//确定每个节点所属的连接组件。
	for (int i = 0; i < network1.size; i++)
	{
		comp[i] = network1.size;
		q[i] = i;
	}

	int last = 0;

	//遍历网络中每个节点
	for (int i = 0; i < network1.size; i++)
	{
		if (comp[i] == network1.size)
		{
			q[0] = i;
			comp[i] = compNum;
			compNum++;
			last = 1;
			//将所有已连接的节点查找到尚未在已连接组件中的节点i
			for(int k=0;k<last;k++)
				for(int j=0;j<network1.deg[q[k]];j++)
					//还没有处理的节点
					if (comp[q[k]] < comp[network1.neighbor[q[k]][j]])
					{
						if(alignment[q[k]]!=-1)
							for(int l=0;l<network2.deg[alignment[q[k]]];l++)
								if (network2.neighbor[alignment[q[k]]][l] = alignment[network1.neighbor[q[k]][l]])
								{
									comp[network1.neighbor[q[k]][j]] = comp[q[k]];
									q[last] = network1.neighbor[q[k]][j];
									last++;
								}
					}
		}
	}

	subGraph = new int[compNum - 1];
	for (int i = 0; i < compNum - 1; i++)//连接组件的数组
		subGraph[i] = 0;
	for (int i = 0; i < network1.size; i++)
		subGraph[comp[i] - 1]++;// definit连接的组件中的节点数

	//找到具有最大节点数的组件
	maxComp = 0;
	for (int i = 0; i < compNum - 1; i++)
	{
		if (subGraph[maxComp] < subGraph[i])
			maxComp = i;
	}

	int temp = subGraph[maxComp];

	delete[]subGraph;
	delete[]q;

	return temp;
}

int Alignment::getCCCE(void)
{
	int edgeComp = 0;
	ofstream CC;
	//遍历第一个网络的每个节点
	for (int i = 0; i < network1.size; i++)//对于第二个网络中与节点i对齐的节点的每个邻居l
	{	 //for each neighbor of node i
		for (int j = 0; j < network1.deg[i]; j++)
			//for each neighbor l of a node in second network that is aligned with node i
			if (alignment[i] != -1)
				for (int l = 0; l < network2.deg[alignment[i]]; l++)
					if (network2.neighbor[alignment[i]][l] == alignment[network1.neighbor[i][j]])
						if (comp[i] - 1 == maxComp) {
							edgeComp++;
						}
	}
	return (edgeComp / 2);
}

float Alignment::getEC(void)
{
	int totalscore = 0;
	for (int i = 0; i < network1.size; i++)
	{
		for (int j = 0; j < network1.deg[i]; j++)
		{
			if(alignment[i]!=-1)
				for (int l = 0; l < network2.deg[alignment[i]]; l++)
				{
					if (network2.neighbor[alignment[i]][l] == alignment[network1.neighbor[i][j]])
						totalscore++;
				}
		}
	}
	int minEdge = (network1.numOfEdge > network2.numOfEdge) ? network2.numOfEdge : network1.numOfEdge;
	return ((float)totalscore) / (2 * minEdge);
	
}

float Alignment::getS3(void)
{
	int totalScore = 0;
	int *alignnodes = new int[network1.size];
	int num_edge_net2 = 0;

	for (int i = 0; i < network1.size; i++)
	{
		alignnodes[i] = alignment[i];
		for(int j=0;j<network1.deg[i];j++)
			if (alignment[i] != -1) {
				for (int l = 0; l < network2.deg[alignment[i]]; l++) {
					if (network2.neighbor[alignment[i]][l] == alignment[network1.neighbor[i][j]])
						totalScore++;
				}
			}
	}
	totalScore = totalScore / 2;

	for (int i = 0; i < network1.size; i++)
		if (alignment[i] != -1)
			for (int j = 0; j < network2.deg[alignment[i]]; j++)
				for (int l = 0; l < network1.size; l++)
					if (network2.neighbor[alignment[i]][j] == alignnodes[l])
						num_edge_net2++;
	num_edge_net2 = num_edge_net2 / 2;

	int minEdge = (network1.numOfEdge > network2.numOfEdge) ? network2.numOfEdge : network1.numOfEdge;
	return ((float)totalScore) / (minEdge + float(num_edge_net2) - totalScore);
}

//将评估测量结果打印在带有输入参数名称的文件中
//输入参数名称确定要写入结果的文件。

void Alignment::outputEvaluate(string name)
{
	string outFile = name;
	outFile.append(".eval");
	ofstream outputFile(outFile.c_str());

	//打印控制台
	outputFile << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
	outputFile << "*** CONNECTED COMPONENTS SIZE : " << endl;
	outputFile << "Nodes = " << CCCV << endl;
	outputFile << "Edges = " << CCCE << endl;

	outputFile << "===============================================================" << endl;
	if (reverse)
	{
		outputFile << "G1:  Nodes : " << network2.size << "  - Edges : " << network2.numOfEdge << endl;
		outputFile << "G2:  Nodes : " << network1.size << "  - Edges : " << network1.numOfEdge << endl;
	}
	else
	{
		outputFile << "G1:  Nodes : " << network1.size << "  - Edges : " << network1.numOfEdge << endl;
		outputFile << "G2:  Nodes : " << network2.size << "  - Edges : " << network2.numOfEdge << endl;
	}

	outputFile << "EC : " << EC << endl;
	outputFile << "S3 : " << S3 << endl;
}

//使用输入参数名称在文件中打印对齐方式（映射）
//输入参数名称确定要写入映射的文件。
void Alignment::outputAlignment(string name)
{
	string alignFile = name;

	alignFile.append(".alignment");

	ofstream alignmentFile(alignFile.c_str());
	if (reverse)
		for (int i = 0; i < network1.size; i++)
			alignmentFile << network1.getName(alignment[i]) << ' ' << network2.getName(i) << endl;
	else
	{
		for (int i = 0; i < network1.size; i++)
			alignmentFile << network1.getName(i) << ' ' << network2.getName(alignment[i]) << endl;
	}
}

Alignment::Alignment(void)
{
}

Alignment::~Alignment(void)
{
}

