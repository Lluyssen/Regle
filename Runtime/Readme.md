# 🔎 C++ Observable Fields & Validation Rules

Ce projet propose un mini-framework en C++ permettant de :
- rendre des champs **observables** (`ObservableField<T>`)
- associer des **règles de validation** à des champs spécifiques
- déclencher automatiquement la validation à chaque modification

L’objectif est de fournir un mécanisme de validation **déclaratif et générique**, basé sur une introspection statique simple (`tie`, `TypeList`).

---

## ✨ Fonctionnalités

- `ObservableField<T>` : wrapper autour d’un type `T` avec notification de changement.
- `RuleRegistry<T>` : registre de règles de validation associées à un type `T`.
- `addRule<T, FieldType>` : déclaration d’une règle de validation sur un champ donné.
- `setupRunTimeValidation(obj)` : lie les champs observables de `obj` aux règles définies.
- Validation déclenchée automatiquement à chaque modification de champ.

---

## 📂 Structure

- **ObservableField**  
  Classe template qui encapsule une valeur et déclenche une fonction callback lors des modifications.

- **RuleRegistry**  
  Contient les règles de validation pour chaque type d’objet.  
  Les règles sont déclarées via `addRule` et stockées par nom de champ.

- **setupRunTimeValidation**  
  Parcourt les champs d’un objet (via `tie` et `FieldNames`) et connecte les règles aux observables.

- **Exemple `Person`**  
  Une struct démonstrative avec deux champs observables (`name`, `age`), leurs métadonnées (`tie`, `FieldNames`, `Types`) et des règles associées.

---

## 🛠 Exemple

```cpp
#include <iostream>
#include <string>
#include <array>
#include "RegleRuntime.hpp"

struct Person {
    ObservableField<std::string> name;
    ObservableField<int> age;

    static auto tie(Person& p) { return std::tie(p.name, p.age); }
    static auto tie(const Person& p) { return std::tie(p.name, p.age); }

    static constexpr std::array<const char*, 2> FieldNames = {"name", "age"};
    using Types = TypeList<std::string, int>;
};

int main() {
    Person p;

    // Déclaration des règles
    addRule<Person, std::string>("name", [](const std::string& s) {
        return !s.empty();
    }, "Name must not be empty");

    addRule<Person, int>("age", [](int a) {
        return a >= 0 && a <= 120;
    }, "Age must be between 0 and 120");

    // Connexion des règles aux champs observables
    setupRunTimeValidation(p);

    // Déclenche la validation automatiquement
    p.name = "Toto";   // ✅ ok
    p.name = "";       // ❌ affiche "[Error] name - Name must not be empty"

    p.age = 42;        // ✅ ok
    p.age = -1;        // ❌ affiche "[Error] age - Age must be between 0 and 120"
}
