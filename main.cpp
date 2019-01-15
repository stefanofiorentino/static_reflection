#include <iostream>
#include <type_traits>
#include <tuple>
#include <jsoncpp/json/json.h>

// https://stackoverflow.com/a/34091881/2794395

template<typename Class, typename T>
struct Property
{
    constexpr Property(T Class::*aMember, const char *aName) :
            member{aMember}, name{aName}
    {}

    using Type = T;

    T Class::*member;
    const char *name;
};

class User
{
    std::string username;
    std::string info;
public:
    constexpr static auto properties = std::make_tuple(Property<User, std::string>{&User::username, "username"},
                                                       Property<User, std::string>{&User::info, "info"});

    const std::string &getUsername() const
    {
        return username;
    }

    void setUsername(const std::string &username)
    {
        User::username = username;
    }

    const std::string &getInfo() const
    {
        return info;
    }

    void setInfo(const std::string &info)
    {
        User::info = info;
    }
};

template< class T >
using decay_t = typename std::decay<T>::type;

template< bool B, class T = void >
using enable_if_t = typename std::enable_if<B,T>::type;

template<std::size_t iteration, typename T>
void doSetData(T &&object, const Json::Value &data)
{
    constexpr auto property = std::get<iteration>(decay_t<T>::properties);
    using Type = typename decltype(property)::Type;
    object.*(property.member) = data[property.name].asString();
}

template<std::size_t iteration, typename T, enable_if_t<(iteration > 0)>* = nullptr>
void setData(T &&object, const Json::Value &data)
{
    doSetData<iteration>(object, data);
    setData<iteration - 1>(object, data);
}

template<std::size_t iteration, typename T, enable_if_t<(iteration == 0)>* = nullptr>
void setData(T &&object, const Json::Value &data)
{
    doSetData<iteration>(object, data);
}

template<typename T>
T fromJson(Json::Value data)
{
    T object;

    setData<std::tuple_size<decltype(T::properties)>::value - 1>(object, data);

    return object;
}

int main()
{
    Json::Value value;
    value["username"] = "fiorentinoing";
    value["info"] = "https://www.linkedin.com/in/fiorentinoing/";
    User u = fromJson<User>(value);
    std::cout << "Hello, "<< u.getUsername() <<"!" << std::endl;
    std::cout << "Please, visit "<< u.getInfo() <<"." << std::endl;
    return 0;
}
