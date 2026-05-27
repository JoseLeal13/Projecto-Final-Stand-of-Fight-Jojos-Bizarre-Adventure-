#ifndef STEELBALL_H
#define STEELBALL_H
#include <QRect>
class SteelBall
{
private:
    int x;
    int y;

    bool destruida;

public:
    SteelBall(int x, int y);

    QRect getHitbox();

    bool estaDestruida();

    void recibirGolpe();
};

#endif // STEELBALL_H
