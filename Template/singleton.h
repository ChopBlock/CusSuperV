#ifndef SINGLETON_H
#define SINGLETON_H

#endif // SINGLETON_H
#include <iostream>

template<typename T>
/**
 *单例模式
 *1.线程安全
 *2.模板
 */
class SingleTon{


public:
    SingleTon(const SingleTon&)=delete;
    virtual  ~SingleTon()=default;//默认体
    SingleTon&operator=(const SingleTon&)=delete;
    static T& Get_Instance()noexcept(std::is_nothrow_constructible<T>::value){


        static T instace{token()};//magic static 并行阻塞 等初始化完成
        return instace;
    }

protected:
    struct token{};
    SingleTon()noexcept=default;

};
