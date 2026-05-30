#ifndef OOP_RESOURCEMANAGER_H
#define OOP_RESOURCEMANAGER_H

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <memory>
#include <iostream>

// Clasă șablon cu sens: gestionează colecții de resurse de tip T (ex: sf::Texture, sf::Font)
template <typename T>
class ResourceManager {
private:
    std::map<std::string, T> resources;

    // Constructor privat pentru a respecta șablonul de proiectare Singleton
    ResourceManager() = default;

public:
    // Ștergem constructorul de copiere și operatorul de atribuire pentru a preveni duplicarea
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // Punctul global de acces la instanța unică a Singleton-ului (pentru tipul T)
    static ResourceManager<T>& getInstance() {
        static ResourceManager<T> instance;
        return instance;
    }

    // Metodă care încarcă o resursă dacă nu există deja, sau o returnează pe cea existentă
    T& getResource(const std::string& filePath) {
        auto it = resources.find(filePath);
        if (it != resources.end()) {
            return it->second;
        }

        // Dacă resursa nu există, o creăm și o încărcăm
        T resource;
        if (!resource.loadFromFile(filePath)) {
            std::cerr << "[ResourceManager Error] Failed to load resource from: " << filePath << "\n";
        }

        resources[filePath] = resource;
        return resources[filePath];
    }
};

#endif // OOP_RESOURCEMANAGER_H