#include "jotaro.h"

jotaro::jotaro() {}

void jotaro::atacar()
{
    // por ahora solo imprime en consola
    // después aquí lanzamos el proyectil
    qDebug() << "Jotaro ataca!";
}

QRect jotaro::getAttackHitbox()
{

    if(direccion == 2) // izquierda
        return QRect(x - 90, y + 25, 90, 45);;

    // derecha
    return QRect(x + 60, y + 25, 90, 45);
}