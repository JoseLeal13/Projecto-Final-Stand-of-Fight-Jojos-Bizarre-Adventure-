#ifndef JOTARO_H
#define JOTARO_H

#include <QDebug>
#include "personaje.h"

class jotaro : public Personaje
{
public:
    jotaro();
    void atacar();
    QRect getAttackHitbox();
};

#endif // JOTARO_H
