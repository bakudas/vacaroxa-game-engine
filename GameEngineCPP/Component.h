#pragma once

class Component
{
public:
    
    // Construtor
    // (quanto mais baixo o updateOrder, mais cedo o componente será atualizado
    Component(class Actor* owner, int updateOrder = 100);

    // Desconstrutor
    virtual ~Component();

    // Atualiza o Component pelo deltatime
    virtual void Update(float deltaTime);

    int GetUpdateOrder() const {return mUpdateOrder;}

protected:

    // Actor dono do component
    class Actor* mOwner;

    // Atualiza a ordem dos components

    int mUpdateOrder;
    
};
