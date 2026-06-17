/**
* @file Event.ixx
 * @brief 事件类
 * @version 1.0
 * @author Chaim
 * @date 2024/09/30
 */

export module CEngine.Base:Event;
import :Object;
import std;

namespace CEngine {
    export template<typename Res, typename... ArgTypes>
    class Event;

    /**
    * @brief <b>事件类</b>
    * @todo 增加删除绑定操作(-=)
    */
    export template<typename Res, typename... ArgTypes>
    class Event<Res(ArgTypes...)> {
        using rRes = std::conditional_t<std::is_void_v<Res>, Res, std::optional<Res> >; // 如果Res为void，则rRes为void，否则为optional<Res>

    public:
        Event() = default;
        ~Event() = default;

        /**
        * <b>绑定普通函数</b>
        * @param func 函数指针
        */
        Event &operator+=(const std::function<Res(ArgTypes...)> func) {
            if constexpr (std::is_void_v<Res>) {
                /* 判断void，减少封装 */
                Functions.emplace(GetNextIdx(), std::move(func));
            } else {
                Functions.emplace(GetNextIdx(), std::move([func](ArgTypes... args) -> rRes {
                    return {std::invoke(func, args...)};
                }));
            }
            return *this;
        }

        /**
        * <b>绑定类函数</b> \n
        * 建议使用公开基础CEngine::Object的类
        * @param t 类型为tuple<对象指针, 类函数指针>
        */
        template<class T>
        Event &operator+=(std::tuple<T *, Res(T::*)(ArgTypes...)> t) {
            Functions.emplace(GetNextIdx(), std::move([t](ArgTypes... args) -> rRes {
                if constexpr (std::is_base_of_v<Object, T>) {
                    /* 如果是Object的子类，增加对象可用判断 */
                    if (const auto obj_ptr = static_cast<Object *>(std::get<0>(t)); obj_ptr == nullptr || !obj_ptr->IsValid()) {
                        // 对象不可用
                        if constexpr (std::is_same_v<Res, void>) return;
                        else return std::nullopt;
                    }
                }
                if constexpr (std::is_same_v<Res, void>) {
                    std::invoke(std::get<1>(t), std::get<0>(t), args...);
                    return;
                } else return std::optional<Res>{std::invoke(std::get<1>(t), std::get<0>(t), args...)};
            }));
            return *this;
        }

        Event &operator-=(std::size_t idx) {
            Functions.erase(idx);
            return *this;
        }

        /**
        * <b>执行绑定在事件中的所有函数</b>
        * @param args 与原函数参数相同
        * @return 返回一个vector<optional<原函数返回类型>>，存储全部返回值，成员可能为<u>nullopt</u>
        */
        [[nodiscard]] auto Invoke(ArgTypes... args) {
            if constexpr (std::is_void_v<Res>) {
                for (auto &f: Functions | std::views::values) {
                    std::invoke(f, args...);
                }
            } else {
                std::vector<std::optional<Res> > res;
                for (auto it = Functions.begin(); it != Functions.end();) {
                    auto r = std::invoke(it->second, args...);
                    if (r == std::nullopt)
                        it = Functions.erase(it); // Object对象已失效
                    else
                        ++it;
                    res.push_back(std::move(r));
                }
                return res;
            }
        }

        /**
         * @brief 获取下一次绑定所使用的索引
         * @return std::size_t 
         */
        std::size_t GetNextIdx() {
            return Functions.empty() ? 0 : Functions.rbegin()->first + 1;
        }

    private:
        std::map<std::size_t, std::function<rRes(ArgTypes...)> > Functions;
    };
}
