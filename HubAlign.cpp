#include"Alignment.h"
#include<iostream>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<sstream>

using namespace std;

exception er;

int main(int argc, char *argv[])
{
	double lambda = 0.2;//// a是控制边缘权重因子的alpha
	double alpha = 0.7;
	int degree = 10;//控制制作骨架的步骤

	char *name1;
	char *name2;
	char *blastFile = nullptr;
	/*for (int i = 0; i < argc; i++) {
		cout << "argument[" << i << "] is: " << argv[i] << endl;
	}*/
	try {
		if (argc < 3) {
			//cout << "应该有两个输入文件！";
			return -1;
		}
		else {
			int i = 1;//输入参数统计
			name1 = argv[i++];
			name2 = argv[i++];
			cout << argc << " " << argv << endl;
			while (i < argc)
			{
				if ((strlen(argv[i]) == 2) && (argv[i][0] == '-') && (i + 1 < argc)) //wether or not the parameter has started with '-' and has a value
				{
					i++; //to read the value of input parameter
					if (argv[i - 1][1] == 'l')
					{
						lambda = atof(argv[i]);
						if (lambda < 0 || lambda > 1) //the value of a should be between zero and one
						{
							cout << "Error : value of a must be between 0 and 1" << endl;
							return -1;
						}
					}
					else if (argv[i - 1][1] == 'd')
					{
						degree = atoi(argv[i]);
						if (degree > 100)
						{
							cout << "Error : value of t must be between 0 and 100" << endl;
							return -1;
						}
					}
					else if (argv[i - 1][1] == 'b')
					{
						blastFile = argv[i];
					}
					else if (argv[i - 1][1] == 'a')
					{
						alpha = atof(argv[i]);
					}

					i++;// to reach the next input parameter if there is
				}
				else
				{
					cout << strlen(argv[i]) <<" "<< i + 1 <<" "<< argc << endl;
					cout << "Error in argument : " << argv[i] << endl;
					return -1;
				}
			}

		}//end else;


		cout << "\n=============== HubAlign ==================\n";
		//construct the networks
		Network network1(name1);
		Network network2(name2);
		bool reverse = false; //it means size of first input network is bigger than second one

		if (network1.size > network2.size)
			reverse = true;
		//making the skeletons of the networks 

		network1.makeSkeleton(degree);
		network2.makeSkeleton(degree);



		//align two networks with each other
		Alignment alignment(network1, network2);
		if (alpha != 1) {
			alignment.readblast(blastFile);
		}

		alignment.align(lambda, alpha);

		//making the name for output file
		stringstream strm;
		strm << name1 << "-" << name2;
		alignment.outputEvaluate(strm.str());
		alignment.outputEvaluate(strm.str());

	}
	catch(exception &e){
		cout << "Error in argument or input files!" << endl;
		e.what();
		return -1;
	}

	system("pause");
	return 0;
}