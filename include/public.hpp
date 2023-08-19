# ifndef PUBLIC_H
# define PUBLIC_H
#include<iostream>
using namespace std;

//日志打印文件、行号、时间、信息
#define LOG(str) \
    cout<<__FILE__<<":"<<__LINE__<<" "<<\
    __TIMESTAMP__<<" : "<<str<<endl;


# endif