#ifndef DIO_H
#define DIO_H

#include "personaje.h"
#include <cmath>

class DIO : public Personaje
{
public:
    DIO(Personaje* jojo); // Recibe a Jojo como objetivo

    void moverse() override;
    void atacar() override;
    void habilidadEspecial() override;

private:
    Personaje* objetivo;
    int distanciaDeteccion = 500; // Rango en píxeles para empezar a perseguir
    int distanciaAtaque = 60;    // "MUDA MUDA"
};

#endif
