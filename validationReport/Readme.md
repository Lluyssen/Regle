# 🚀 Validation Framework en C++17

Ce projet implémente un **moteur de validation générique** basé sur la réflexion statique en C++17.  
Il permet de définir des règles de validation sur des structures C++ introspectées, et de vérifier récursivement les objets pour produire un rapport d’erreurs.

---

## ✨ Fonctionnalités
- Introspection statique via `tie()`, `FieldNames` et `TypeList`.
- Enregistrement de règles de validation par champ (`RuleRegistry`).
- Validation récursive sur des structs imbriquées.
- Rapport détaillé (`ValidationReport`) avec :
  - chemin du champ fautif,
  - message d’erreur,
  - niveau de sévérité (`Info`, `Warning`, `Error`).
- Activation/désactivation dynamique des règles.

---

## 📂 Structure du projet
- **Regle.cpp / Regle.hpp**  
  Définition des structs introspectables (`Adress`, `Person`).
- **RegleRuntime.hpp**  
  - `ValidationError` et `ValidationReport`.  
  - `RuleRegistry` (enregistrement et gestion des règles).
- **RegleRuntime.cpp**  
  - Implémentation de la validation récursive.  
  - Application des règles aux champs introspectés.

---
