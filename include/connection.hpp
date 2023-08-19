#ifndef CONNECTION_H
#define CONNECTION_H
#include<mysql/mysql.h>
#include<string>
#include<ctime>
using namespace std;

class Connection
{
public:
    // 初始化数据库连接
    Connection();

    // 释放数据库连接资源
    ~Connection();

    // 连接数据库
    bool connect(string ip,
                 unsigned short port,
                 string user,
                 string password,
                 string dbname);

    // 更新操作
    bool update(string sql);

    // 查询操作
    MYSQL_RES *query(string sql);

    // 更新连接的起始空闲时间点
    void refreshAlliveTime(){ _allivetime = clock(); }

    // 返回存活的时间
    clock_t getAlliveTime() const{ return clock() - _allivetime; }

private:
    // 一条mysql连接
    MYSQL *_conn;
    // 记录进入
    clock_t _allivetime;
};
#endif