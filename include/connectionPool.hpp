# ifndef CONNECTIONPOOL_H
# define CONNECTIONPOOL_H

#include"connection.hpp"
#include<string>
#include<queue>
#include<mutex>
#include<atomic>
#include<memory>
#include<functional>
#include<condition_variable>
using namespace std;


/*
实现数据库连接池功能模块
懒汉模式构造单例
*/
class ConnectionPool
{
public:
    //获取连接池对象实例
    static ConnectionPool* getConnectionPool();

    //从连接池中获取一个可用空闲连接
    shared_ptr<Connection> getConnection();

private:
    //构造函数私有化，单例
    ConnectionPool();

    //从配置文件加载配置项
    bool loadConfigFile();

    //运行在独立线程中，专门负责生产新连接
    void produceConnectionTask();

    // 扫描超过maxIdleTime时间的空闲连接，进行回收
    void scannerConnectionTask();

    string _ip;
    unsigned short _port;
    string _dbname;
    string _username;
    string _password;
    int _initSize;//连接池的初始连接量
    int _maxSize;//连接池的最大连接量
    int _maxIdleTime;//连接池的最大空闲时间
    int _connectionTimeOut;//连接池获取连接的超时时间
    

    queue<Connection*> _connectionQue;//存储mysql连接的队列
    mutex _queueMutex;//维护连接队列的线程安全互斥锁
    atomic_int _connectionCnt;//记录所创建的connection连接的总数量
    condition_variable cv;//用于线程间通信(生产者、消费者)的条件变量

};

#endif