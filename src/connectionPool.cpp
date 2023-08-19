#include"connectionPool.hpp"
#include"public.hpp"
#include<thread>

//构造函数私有化，单例
ConnectionPool::ConnectionPool()
{
    //加载配置项
    if(!loadConfigFile())
    {
        //加载配置文件失败
        return ;
    }

    //创建初始数量的连接
    for(int i = 0; i < _initSize; ++i)
    {
        Connection* p = new Connection();
        p->connect(_ip,_port,_username,_password,_dbname);
        // 刷新连接的起始空闲时间
        p->refreshAlliveTime();
        _connectionQue.push(p);
        _connectionCnt++;

    }

    //启动一个新的线程，作为连接的生产者,成员方法作为一个单独的线程函数必须绑定this指针
   thread produce(std::bind(&ConnectionPool::produceConnectionTask,this));
    //分离线程，设为守护线程角色
    produce.detach();

    
    // 启动一个新的定时线程，扫描超过maxIdleTime时间的空闲连接，进行回收
   thread scanner(std::bind(&ConnectionPool::scannerConnectionTask,this));
   scanner.detach();

}

 //获取连接池对象实例
ConnectionPool* ConnectionPool::getConnectionPool()
{
    static ConnectionPool pool;
    return &pool;
}

bool ConnectionPool::loadConfigFile()
{
    FILE *pf = fopen("/home/chengyongtao/VScode/MysqlConnectionPool/config/mysqlconnection.cnf","r");
    if(nullptr == pf)
    {
        LOG("mysqlconnection.cnf file is not exit!");
        return false;
    }

    while(!feof(pf))
    {
        char line[1024] = {0};
        fgets(line,1024,pf);
        string str =  line;
        int idx = str.find("=",0);
        //未找到则为无效的配置行
        if(idx == -1)
        {
            continue;
        }

        // dbname=test\n
        int endidx = str.find("\n",idx);//找到换行符
        string key = str.substr(0,idx);
        string value = str.substr(idx+1,endidx-idx-1);

        if(key == "ip")
        {
            _ip = value;
        }
        else if(key == "port")
        {
            _port = atoi(value.c_str());
        }
        else if(key == "dbname")
        {
            _dbname = value;
        }
        else if(key == "username")
        {
            _username = value;
        }
        else if(key == "password")
        {
            _password = value;
        }
        else if(key == "initSize")
        {
            _initSize = atoi(value.c_str());
        }
        else if(key == "maxSize")
        {
            _maxSize = atoi(value.c_str());
        }
        else if(key == "maxIdleTime")
        {
            _maxIdleTime = atoi(value.c_str());
        }
        else if(key == "connectionTimeOut")
        {
            _connectionTimeOut = atoi(value.c_str());
        }

    }
    return true;
}

//从连接池中获取一个可用空闲连接
shared_ptr<Connection>ConnectionPool::getConnection()
{
    unique_lock<mutex> lock(_queueMutex);
    if(_connectionQue.empty())
    {
        //若队列为空，则阻塞等待
        if(cv_status::timeout == cv.wait_for(lock,chrono::milliseconds(_connectionTimeOut)))
        {
            //若阻塞等待达到最大获取连接时间，仍未获取连接则返回空指针
            if(_connectionQue.empty())
            {
                LOG("获取空闲连接超时...获取连接失败！");
                return nullptr;
            }
        }
    }

    //阻塞途中被唤醒或队列不为空，则直接获取连接

    //shared_ptr智能指针析构时，会调用connection析构函数，故需要在这里自定义一下shared_ptr的资源释放方式
    shared_ptr<Connection> sp(_connectionQue.front(),
                              [&](Connection *ptcon)
                              {
                                  unique_lock<mutex> lock(_queueMutex);
                                  // 刷新连接的起始空闲时间
                                  ptcon->refreshAlliveTime();
                                  _connectionQue.push(ptcon);
                              });

    _connectionQue.pop();
    //消费完连接以后，通知生产者线程检查一下，若队列为空，赶紧生产
    cv.notify_all();

    return sp;

}

//运行在独立线程中，专门负责生产新连接
void ConnectionPool::produceConnectionTask()
{
    while(1)
    {
        unique_lock<mutex> lock(_queueMutex);
         
        while(!_connectionQue.empty())
        {
            //若队列不为空阻塞,并释放锁
            cv.wait(lock);
        }

        //连接数量没有达到上限，则继续创建
        if (_connectionCnt < _maxSize)
        {
            Connection *p = new Connection();
            p->connect(_ip, _port, _username, _password, _dbname);
            // 刷新连接的起始空闲时间
            p->refreshAlliveTime();
            _connectionQue.push(p);
            _connectionCnt++;
        }

        //通知消费者线程，可以连接
        cv.notify_all();
    }
}

// 扫描超过maxIdleTime时间的空闲连接，进行回收
void ConnectionPool::scannerConnectionTask()
{
    while(1)
    {
        // 模拟定时效果
        this_thread::sleep_for(chrono::seconds(_maxIdleTime));

        //扫描整个队列，释放多余连接
        unique_lock<mutex>lock(_queueMutex);
        while(_connectionCnt > _initSize)
        {
            //获取队首连接
            Connection *p = _connectionQue.front();
            if(p->getAlliveTime() > (_maxIdleTime*1000))
            {
                _connectionQue.pop();
                _connectionCnt--;
                // 释放连接，调用~Connection()
                delete p;
            }
            else
            {
                //队头元素未超时，则其余都不会超时
                break;
            }
        }
    }
}


