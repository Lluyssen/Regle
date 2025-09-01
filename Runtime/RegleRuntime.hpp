#include <vector>
#include <functional>
#include <iostream>
#include "../TypeList/TypeList.hpp"

template <typename T>
class ObservableField
{
    private:
    
        T value {};
        std::function<void(const T&)> onChange;
    
    public:
    
        ObservableField() = default;
        ObservableField(const T& val) : value(val) {};
        virtual ~ObservableField() = default;
    
        const T& get() const
        {
            return value;
        }
    
        T& get()
        {
            return value;
        }
    
        operator const T&() const
        {
            return value;
        }
    
        ObservableField& operator=(const T& val)
        {
            value = val;
            if (onChange)
                onChange(value);
            return *this;
        }
    
        void bind(std::function<void(const T&)> callback)
        {
            onChange = std::move(callback);
        }
};

template <typename T>
class RuleRegistry
{
    public:

        using RuleFn = std::function<void(const T&)>;

        static std::vector<std::pair<std::string, RuleFn>>& rules()
        {
            static std::vector<std::pair<std::string, RuleFn>> r;
            return r;
        }

        static void addRule(const std::string& field, RuleFn fn)
        {
            rules().emplace_back(field, std::move(fn));
        }
};

template <typename T, typename FieldType>
void addRule(const std::string& field, std::function<bool(const FieldType&)> fn, const std::string& message)
{
    RuleRegistry<T>::addRule(field, [field, fn = std::move(fn), message](const T& obj) {
        auto tie = T::tie(obj);
        StaticForEachWithIndex<typename T::Types>([&](auto typeTag, int i) {
            using tag = decltype(typeTag);
            using ThisField = typename tag::type;
            if (std::is_same<FieldType, ThisField>::value)
            {
                if (T::FieldNames[i] != field)
                    return;
                visitTupleAt(i, tie, [&](const auto& observalbe) {
                    const auto& value = observalbe.get();
                    if (!fn(*((FieldType*)(&value))))
                        std::cout << "[Error] " << field << " : " << message << std::endl;
                });
            }
        });
    });
}

template <typename T>
void setupRunTimeValidation(T& obj)
{
    auto tie = T::tie(obj);
    StaticForEachWithIndex<typename T::Types>([&](auto typeTag, int i) {
        using tag = decltype(typeTag);
        using FieldType = typename tag::type;
        const char* field = T::FieldNames[i];

        visitTupleAt(i, tie, [&](auto& observable) {
            observable.bind([&obj, field](const auto&) {
                for (const auto& rule : RuleRegistry<T>::rules())
                {
                    if (rule.first == field)
                        rule.second(obj);
                }
            });
        });
    });
}