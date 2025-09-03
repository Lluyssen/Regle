#pragma once
#include <vector>
#include <functional>
#include "../TypeList/TypeList.hpp"

enum class Severity 
{
    Info,
    Warning,
    Error
};

struct ValidationError
{
    std::string fieldPath;
    std::string message;
    Severity severity;
};

struct ValidationReport
{
    std::vector<ValidationError> errors;
    bool hasErrors(void) const
    {
        return !errors.empty();
    }
};

template <typename Struct>
class RuleRegistry 
{
    public:

        using RuleFn = std::function<bool(const void*)>;
        struct Rule
        {
            std::string id;
            std::string field;
            std::string message;
            Severity severity;
            bool enable = true;
            RuleFn fn;
        };

        static std::vector<Rule>& rules(void)
        {
            static std::vector<Rule> list;
            return list;
        }

        static void addRule(std::string id, const std::string& field, RuleFn fn, std::string message, Severity severity = Severity::Error)
        {
            rules().push_back({std::move(id), field, std::move(message), severity, true, std::move(fn)});
        }

        static void setRuleEnabled(const std::string& id, bool enabled)
        {
            for (auto& r : rules())
            {
                if (r.id == id)
                    r.enable = enabled;
            }
        }
};

template <typename T, typename FieldType>
void addRule(std::string id, const std::string& field, std::function<bool(const FieldType&)> fn, std::string message, Severity severity = Severity::Error)
{
    RuleRegistry<T>::addRule(std::move(id), field, [fn = std::move(fn)](const void* ptr) {
        return fn(*static_cast<const FieldType*>(ptr));
    }, std::move(message), severity);
}

template <typename T>
void enableRule(const std::string& id)
{
    RuleRegistry<T>::setRuleEnabled(id, true);
}

template <typename T>
void disableRule(const std::string& id)
{
    RuleRegistry<T>::setRuleEnabled(id, false);
}

template <typename T>
void validateRecursive(const T& obj, const std::string& prefix, std::vector<ValidationError>& output)
{
    auto tie = getTie(obj);
    StaticForEachWithIndex<typename T::Types>([&](auto tag, int i) {
        using FieldType = typename decltype(tag)::type;
        const char* name = T::FieldNames[i];
        std::string full = prefix.empty() ? name : prefix + "." + name;
        visitTupleAt(i, tie, [&](const auto& value) {
            using V = std::decay_t<decltype(value)>;
            if constexpr (hasTie<V>)
                validateRecursive(value, full, output);
            else
            {
                for (const auto& rule : RuleRegistry<T>::rules())
                {
                    if (rule.enable && rule.field == name && !rule.fn(static_cast<const void*>(&value)))
                        output.push_back({full, rule.message, rule.severity});
                }
            }
        });
    });
}

template <typename T>
ValidationReport validateDeep(const T& obj)
{
    std::vector<ValidationError> all;
    validateRecursive(obj, "", all);
    ValidationReport report;
    report.errors = std::move(all);
    return report;
}