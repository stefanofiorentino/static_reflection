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

struct User
{
    std::string username;
    std::string info;

    constexpr static auto properties = std::make_tuple(Property<User, std::string>{&User::username, "username"},
                                                       Property<User, std::string>{&User::info, "info"});
};

template<std::size_t iteration, typename T>
void doSetData(T &&object, const Json::Value &data)
{
    constexpr auto property = std::get<iteration>(std::decay_t<T>::properties);
    using Type = typename decltype(property)::Type;
    object.*(property.member) = data[property.name].asString();
}

template<std::size_t iteration, typename T, std::enable_if_t<(iteration > 0)>* = nullptr>
void setData(T &&object, const Json::Value &data)
{
    doSetData<iteration>(object, data);
    setData<iteration - 1>(object, data);
}

template<std::size_t iteration, typename T, std::enable_if_t<(iteration == 0)>* = nullptr>
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
    std::cout << "Hello, "<< u.username <<"!" << std::endl;
    std::cout << "Please, visit "<< u.info <<"." << std::endl;
    return 0;
}
