#ifndef OOP_GAMEEXCEPTIONS_H
#define OOP_GAMEEXCEPTIONS_H

#include <stdexcept>
#include <string>


class GameError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};


class ResourceLoadError : public GameError {
public:
    using GameError::GameError;
};


class InvalidMapError : public GameError {
public:
    using GameError::GameError;
};


class WindowCreationError : public GameError {
public:
    using GameError::GameError;
};

#endif
