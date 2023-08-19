#include"connection.hpp"
#include"connectionPool.hpp"
#include"public.hpp"
#include<thread>
#include<mutex>
#include<chrono>
using namespace std;

 
string sql = "insert into user(name,sex) values('zhangsan','male')"; 
 

// 未使用连接池单线程
void noPoolFun1(int datasize)
{   
    auto begin = chrono::high_resolution_clock::now();

    for (int i = 0; i < datasize; ++i)
    {
        Connection con;
        
        con.connect("127.0.0.1", 3306, "root", "Root_123", "test");
        con.update(sql);
    }
    auto end = chrono::high_resolution_clock::now();

    auto res = chrono::duration_cast<chrono::milliseconds>(end - begin);

    std::cout<<" 未使用连接池，单线程，数据量"<<datasize<<"，耗时：" << res.count() << "ms" << endl;

    std::cout<<"-------------------------------------------------\n";
     
}

// 使用连接池单线程
void usePoolFun1(int datasize)
{
    auto begin1 = chrono::high_resolution_clock::now();

    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    for (int i = 0; i < datasize; ++i)
    { 
        shared_ptr<Connection> sp = cp->getConnection();
        sp->update(sql);
    }
    auto end1 = chrono::high_resolution_clock::now();

    auto res1 = chrono::duration_cast<chrono::milliseconds>(end1 - begin1);

    std::cout << " 使用连接池，单线程，数据量"<<datasize<<"，耗时：" << res1.count() << "ms" << endl;

    std::cout<<"-------------------------------------------------\n";
     
}

// 未使用连接池四线程
void noPoolFun2(int datasize)
{
    int n = datasize / 4;
    mutex mu;
    auto begin = chrono::high_resolution_clock::now();

    Connection con;
    con.connect("127.0.0.1", 3306, "root", "Root_123", "test");

    thread t1([&]()
              {
    ConnectionPool* cp = ConnectionPool::getConnectionPool();
    for(int i = 0; i < n; ++i)
    {          
         {
            lock_guard<mutex>lock(mu);
            con.update(sql);
         }
 
    } });

    thread t2([&]()
              {
    ConnectionPool* cp = ConnectionPool::getConnectionPool();
    for(int i = 0; i < n; ++i)
    {          
        {
            lock_guard<mutex>lock(mu);
            con.update(sql);
         }
 
    } });

    thread t3([&]()
              {
    ConnectionPool* cp = ConnectionPool::getConnectionPool();
    for(int i = 0; i < n; ++i)
    {          
        {
            lock_guard<mutex>lock(mu);
            con.update(sql);
         }
 
    } });

    thread t4([&]()
              {
    ConnectionPool* cp = ConnectionPool::getConnectionPool();
    for(int i = 0; i < n; ++i)
    {      
        {
            lock_guard<mutex>lock(mu);
            con.update(sql);
         }
      
    } });

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    auto end = chrono::high_resolution_clock::now();

    auto res = chrono::duration_cast<chrono::milliseconds>(end - begin);

    std::cout << " 未使用连接池，4线程，数据量"<<datasize <<"，耗时：" << res.count() << "ms" << endl;
}



// 使用连接池四线程
void usePoolFun2(int datasize)
{
    int n = datasize / 4;
    auto begin = chrono::high_resolution_clock::now();
    
    Connection con;
    con.connect("127.0.0.1", 3306, "root", "Root_123", "test"); 
    
    thread t1([&]()
              {
    ConnectionPool* cp = ConnectionPool::getConnectionPool();
    for(int i = 0; i < n; ++i)
    {  
        shared_ptr<Connection> sp = cp->getConnection();
        sp->update(sql);
    } });

    thread t2([&]()
              {
    ConnectionPool* cp = ConnectionPool::getConnectionPool();
    for(int i = 0; i < n; ++i)
    {    
        shared_ptr<Connection> sp = cp->getConnection();
        sp->update(sql);
    } });

    thread t3([&]()
              {
    ConnectionPool* cp = ConnectionPool::getConnectionPool();
   for(int i = 0; i < n; ++i)
    {         
        shared_ptr<Connection> sp = cp->getConnection();
        sp->update(sql);
    } });

    thread t4([&]()
              {
    ConnectionPool* cp = ConnectionPool::getConnectionPool();
    for(int i = 0; i < n; ++i)
    {        
        shared_ptr<Connection> sp = cp->getConnection();
        sp->update(sql);
    } });

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    auto end = chrono::high_resolution_clock::now();
   
    auto res = chrono::duration_cast<chrono::milliseconds>(end-begin);

    std::cout << " 使用连接池，4线程，数据量"<<datasize <<"，耗时：" <<res.count()<<"ms"<<endl;  

}


int main()
{   
    //使用单线程测试数据量 1000、5000、10000
    noPoolFun1(1000);
    noPoolFun1(5000);
    noPoolFun1(10000);

    //使用单线程测试数据量 1000、5000、10000
    usePoolFun1(1000);
    usePoolFun1(5000);
    usePoolFun1(10000);

    //使用4线程测试数据量 1000 
    noPoolFun2(1000);
    usePoolFun2(1000);

    //使用4线程测试数据量 5000 
    noPoolFun2(5000);
    usePoolFun2(5000);

    //使用4线程测试数据量 10000
    noPoolFun2(10000);
    usePoolFun2(10000);

    //使用4线程测试数据量 100000
    noPoolFun2(100000);
    usePoolFun2(100000);

    return 0;
}


