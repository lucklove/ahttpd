#pragma once
#include <utility>

template <typename> struct AsyncWrapper;

template <typename AsyncFuncT>
AsyncWrapper<AsyncFuncT> asyncWrap(AsyncFuncT func)
{
    return AsyncWrapper<AsyncFuncT>(func);   
}

template <typename AsyncFuncT>
struct AsyncWrapper 
{
public:
    AsyncWrapper(AsyncFuncT func) : async_func_{func}
    {
    }

    template <typename FuncT>
    auto then(FuncT callback)
    {
        return asyncWrap([=](auto... ps)
        {
            async_func_([=](auto... params){ return callback(ps..., params...);});
        });
    }

    auto apply()
    {
        return async_func_();
    }
private:
    AsyncFuncT async_func_;
};
