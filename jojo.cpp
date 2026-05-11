#include "jojo.h"

Jojo::Jojo() : Personaje()
{

    setPixmap(QPixmap(":/sprites/SpritesJojoChampionship/Jotaro Kujo sprites.png").scaled(60, 90));
    estaDefendiendo = false;
    tiempoAtaque = 0;
}

void Jojo::moverse()
{
    if (estaDefendiendo) {
        vx = 0; // No se mueve si se defiende
    }

    // 1. GRAVEDAD: Siempre tiramos hacia abajo
    vy += aceleracion_y;

    // 2. MOVIMIENTO VERTICAL (Salto/Caída)
    if (!verificarColision(x(), y() + vy)) {
        setPos(x(), y() + vy);
        enSuelo = false;
    } else {
        if (vy > 0) enSuelo = true; // Si bajaba y chocó, tocó piso
        vy = 0;
    }

    // 3. MOVIMIENTO HORIZONTAL
    if (!verificarColision(x() + vx, y())) {
        setPos(x() + vx, y());
    }
}

void Jojo::saltar()
{
    if (enSuelo && !estaDefendiendo) {
        vy = -15; // Impulso hacia arriba
        enSuelo = false;
    }
}

void Jojo::atacar() {

    aumentarCarga(5);
    qDebug() << "Atacando... Carga actual:" << barradeCarga;

}

void Jojo::habilidadEspecial() {
    if (barradeCarga >= 100) {
        qDebug() << "¡STAR PLATINUM: Za Warudo!";

        barradeCarga = 0;
    } else {
        qDebug() << "Yare yare daze. Carga necesaria: 100. Actual:" << barradeCarga;
    }
}

void Jojo::defensa()
{
    estaDefendiendo = !estaDefendiendo;
    if (estaDefendiendo) {
        qDebug() << "Jojo se está cubriendo.";
        // Reducir daño recibido en la lógica de colisión futura
    }
}
