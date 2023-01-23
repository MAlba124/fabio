#ifndef _TS_DEQUE_HPP
#define _TS_DEQUE_HPP 1

#include <deque>
#include <mutex>
#include <cstdlib>

namespace net::common
{
    template<typename T>
    class TsDeque
    {
    private:
        std::deque<T> q;
        std::mutex m;
    public:
        TsDeque()
            = default;

        ~TsDeque()
            = default;

        void pop_front()
        {
            std::unique_lock<std::mutex> lock(this->m);
            q.pop_front();
        }

        T front()
        {
            std::unique_lock<std::mutex> lock(this->m);
            return q.front();
        }

        void pop_back()
        {
            std::unique_lock<std::mutex> lock(this->m);
            q.pop_back();
        }

        void push_back(const T& x)
        {
            std::unique_lock<std::mutex> lock(this->m);
            q.push_back(x);
        }

        std::size_t size()
        {
            std::unique_lock<std::mutex> lock(this->m);
            return q.size();
        }
    };
}

#endif //_TS_DEQUE_HPP
