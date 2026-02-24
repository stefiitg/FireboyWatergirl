#ifndef OOP_GAMEEXCEPTIONS_H
#define OOP_GAMEEXCEPTIONS_H

#include <stdexcept>
#include <string>

// Base game exception type
class GameError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// Thrown when a required resource (texture, sound, etc.) fails to load
class ResourceLoadError : public GameError {
public:
    using GameError::GameError;
};

// Thrown when creating an invalid map (non-positive dimensions, etc.)
class InvalidMapError : public GameError {
public:
    using GameError::GameError;
};

// Thrown when the game window fails to be created
class WindowCreationError : public GameError {
public:
    using GameError::GameError;
};

#endif // OOP_GAMEEXCEPTIONS_H
