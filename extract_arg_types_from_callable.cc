// g++ -std=c++2a main.cc
// g++ (GCC) 9.1.0
//
#include <functional>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>


template <class T>
T into(std::string const& arg)
{
    return T{};
}

template <>
int into<int>(std::string const& arg) {
    return std::stoi(arg);
}

template <>
std::string into<std::string>(std::string const& arg) {
    return arg;
}


template <class T, class Iter>
T into_tuple_arg(Iter& curr) {
    return into<T>(*curr++);
}


template <typename T, typename Signature>
struct get_arg_tuple_impl;


template <typename T, typename ReturnType, typename... Args>
struct get_arg_tuple_impl<T, ReturnType(T::*)(Args...) const>
{
    using type = std::tuple<std::remove_cv_t<std::remove_reference_t<Args>>...>;
};


template <typename T>
using get_arg_tuple_t =
    typename get_arg_tuple_impl<T, decltype(&T::operator())>::type;


template<class ... Args>
std::tuple<Args...> make_tuple_from_args(
    std::vector<std::string> const& args,
    std::tuple<Args...>)
{
    auto begin = std::begin(args);
    return {into_tuple_arg<Args>(begin)...};
}


template <class FUNC>
void call(FUNC && fn, std::vector<std::string> const& args)
{
    std::apply(fn, make_tuple_from_args(args, get_arg_tuple_t<FUNC>{}));
}


int main()
{
    call(
        [](std::string const& a, int b)
        {
            printf("a = %s, b = %d\n", a.c_str(), b);
        },
        std::vector<std::string>{"foo", "1234"});

    call(
        std::function<void(std::string const& a, int b)>(
            [](std::string const& a, int b)
            {
                printf("a = %s, b = %d\n", a.c_str(), b);
            }),
        std::vector<std::string>{"bar", "5678"});

    return 0;
}
