#pragma once 

namespace sylar {
template<typename T>
class Singleton
{
public:
    static T* GetInstance() {
        static T v;
        return &v;
    }
};
}