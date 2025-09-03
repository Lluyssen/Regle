#include "Regle.hpp"
#include <iostream>

struct Adress
{
    std::string city;
    int zip;

    static auto tie(Adress& p)
    {
        return std::tie(p.city, p.zip);
    }

    static auto tie(const Adress& p)
    {
        return std::tie(p.city, p.zip);
    }

    static constexpr std::array<const char*, 2> FieldNames = 
    {
        "city", "zip"
    };

    using Types = TypeList<std::string, int>;
};


struct Person
{
    std::string name;
    int age;
    Adress adress;

    static auto tie(Person& p)
    {
        return std::tie(p.name, p.age, p.adress);
    }

    static auto tie(const Person& p)
    {
        return std::tie(p.name, p.age, p.adress);
    }

    static constexpr std::array<const char*, 3> FieldNames = 
    {
        "name", "age", "adress"
    };

    using Types = TypeList<std::string, int, Adress>;
};

int main(void)
{
    std::cout << "---Premier Test---" << std::endl;

    Adress a{"", -1};
    Person p{"", -5, a};

    addRule<Person, std::string>("EmptyName", "name", [](const std::string& s){
        return !s.empty();
    }, "Name is required", Severity::Error);

    addRule<Person, std::string>("NbrCharacter", "name", [](const std::string& s){
        return s.size() >= 3;
    }, "Name must be at least 3 characters", Severity::Warning);

    addRule<Person, int>("AgeRange", "age", [](int i){
        return i >= 0 && i <= 100;
    }, "Age must be in range 0 to 100");
    
    addRule<Adress, int>("ZipNbr", "zip", [](int z) {
        return z >= 1000;
    }, "zip code must be >= 1000", Severity::Info);

    disableRule<Person>("NbrCharacter");

    ValidationReport report = validateDeep(p);

    for (const auto& e : report.errors)
    {
        if (e.severity == Severity::Error)
            std::cout << "[Error] " << e.fieldPath << ": " << e.message << std::endl;
        else if (e.severity == Severity::Warning)
            std::cout << "[Warning] " << e.fieldPath << ": " << e.message << std::endl;
        else if (e.severity == Severity::Info)
            std::cout << "[Info] " << e.fieldPath << ": " << e.message << std::endl;
    }

    std::cout << "---Deuxieme Test---" << std::endl;
    
    p.name = "dfmskfdlksdfjsdf";
    p.age = 50;

    ValidationReport rr = validateDeep(p);

    for (const auto& e : rr.errors)
    {
        if (e.severity == Severity::Error)
            std::cout << "[Error] " << e.fieldPath << ": " << e.message << std::endl;
        else if (e.severity == Severity::Warning)
            std::cout << "[Warning] " << e.fieldPath << ": " << e.message << std::endl;
        else if (e.severity == Severity::Info)
            std::cout << "[Info] " << e.fieldPath << ": " << e.message << std::endl;
    }

    return 0;
}