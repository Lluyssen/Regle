#include "RegleRuntime2.hpp"

struct Person
{
    ObservableField<std::string> name;
    ObservableField<int> age;
    
    static auto tie(Person& p)
    {
        return std::tie(p.name, p.age);
    }

    static auto tie(const Person& p)
    {
        return std::tie(p.name, p.age);
    }

    static constexpr std::array<const char*, 2> FieldNames = {"name", "age"};

    using Types = TypeList<std::string, int>;
};

int main(void)
{
    Person p;

    addRule<Person, std::string>("name", [](const std::string& s) {
        return !s.empty();
    }, "need to not be empty");

    addRule<Person, int>("age", [](const int& a) {
        return a > 0 && a <= 120;
    }, "age meust be beetwen 0 and 120");

    setupRunTimeValidation(p);

    p.name = "toto";
    std::cout << p.name.get() << std::endl;
    p.name = "";
    std::cout << p.name.get() << std::endl;
    p.name = "toto";
    std::cout << p.name.get() << std::endl;
    p.age = -1;
    std::cout << p.age.get() << std::endl;
    p.age = 42;
    std::cout << p.age.get() << std::endl;
    return 0;
}