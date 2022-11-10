#pragma once
#include <vector>
#include "Game.h"

class Actor
{
public:
    // estados para atualização do actor
    enum State
    {
        EActive,
        EPaused,
        EDead
    };

    // Construtor / Desconstrutor

    Actor(class Game* game);

    virtual ~Actor();

    // Método Update que será chamado do Game
    void Update(float deltaTime);

    // Método para Atualizar todos os Componentes atachados no Actor
    void UpdateComponents(float deltaTime);

    // Método específico de Atualização (passível de sobrecarga)
    virtual void UpdateActor(float deltaTime);

    // Getter / Setters

    // Add / Remove Componentes
    void AddComponent(class Component* component);
    
    void RemoveComponent(class Component* component);
    
    State GetState() { return mState; }

private:
    // Estado do Actor
    State mState;

    // Transform
    Vector2 mPosition;

    float mScale;

    float mRotation;

    // Componentes presos ao Actor

    std::vector<class Component*> mComponent;

    class Game* game;
    
};
