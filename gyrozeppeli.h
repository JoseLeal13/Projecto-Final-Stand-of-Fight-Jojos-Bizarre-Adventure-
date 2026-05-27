#ifndef GYROZEPPELI_H
#define GYROZEPPELI_H
#include "personaje.h"

class gyrozeppeli : public Personaje
{
public:
    GyroZeppeli(int posX, int posY); // recibe posición fija
    void lanzarBola();

};

#endif // GYROZEPPELI_H
