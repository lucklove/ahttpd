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

    template <typename... FuncTs>
    auto then(FuncTs... callbacks)
    {
        return asyncWrap([=](auto... ps)
        {
            auto callback_wrapper = [=](auto callback){ return [=](auto... params){ return callback(ps..., params...); }; };
            async_func_(callback_wrapper(callbacks)...);
        });
    }

    auto apply()
    {
        return async_func_();
    }
private:
    AsyncFuncT async_func_;
};
