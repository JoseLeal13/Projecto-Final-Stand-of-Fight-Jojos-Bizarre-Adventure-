#ifndef JOJO_H
#define JOJO_H

#include "personaje.h"

class Jojo : public Personaje
{
public:
    Jojo();

    void moverse() override;
    void atacar() override;
    void defensa();
    void habilidadEspecial() override; // ¡Star Platinum: Za Warudo!

    // Controles de estado
    void saltar();
    void setVelocidadX(float v) { vx = v; }

private:
    bool estaDefendiendo;
    int tiempoAtaque; // Para manejar la duración de la animación
};

#endif
