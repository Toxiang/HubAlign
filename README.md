# HubAlign
原代码连接：https://github.com/hashemifar/HubAlign

编写环境：vs2017 运行环境：linux或cyqwin 
对代码进行了复现 并注释
代码结构：
  ---Network.h 头文件
  ---Alignment.h 头文件
  ---Network.cpp 网络创建部分代码
  ---Alignment.cpp 网络对齐函数
  ---HubAlign.cpp 主函数
  ---资源文件 Test1.tab Test2.tab Test1_Test2.bitscore
 
运行：在linux环境下进行编译  可以使用makefile或单独文件编译（后期有时间添加makefiel）
    g++ Network.h
    g++ Alignment.h
    g++ -c Network.cpp
    g++ -c Alignment.cpp
    g++ -c HubAlign.cpp
    ----生成.o文件
    g++ network.o Alignment.o HubAlign.o -o HubAlign(可更改为任意名字）
 
    ---生成可执行文件
    ./HubAlign Test1.tab Test2.tab -l 0.1 -a 0.7 -d 10 -b Test1_Test2.bitscore
    
    -l : The parameter λ that controls the importance of the edge weight relative to the node weight. Usually λ=0.1 yields a biologically more meaningful alignment. Default value is equal to 0.1.

    -a : the parameter α that controls the contribution of sequence information relative to topological similarity. Default value for this parameter is equal to 0.7. α=1 implies that only topological information is used.

    -d : the parameter d that controls the number of nodes removed from network in the process of making the skeleton.

    -b : similarity file that includes the sequence information of proteins. Here, we used Blast bitscores. The similarity file is a tab-separated file with three columns where each line is in form:

    is the bitscore between two proteins and . You don’t need this file if α=1. Here is a sample invocation when α=1:
    
    ----生成两个输出结果文件 一个对齐结果 一个比对指标
    
    ----查看
    
    cat Test1.tab-Test2.tab.eval
    cat Test.tab_Test2.tab.alignment
 
   
