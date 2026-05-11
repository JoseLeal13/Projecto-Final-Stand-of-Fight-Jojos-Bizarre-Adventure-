#include "dio.h"

DIO::DIO(Personaje* jojo) : Personaje()
{
    objetivo = jojo;

    QPixmap hoja(":/sprites/SpritesJojoChampionship/Dio Brando The World sprites.png");
    setPixmap(hoja.copy(10, 10, 70, 90)); // Ajusta estas coordenadas según tu hoja
    setPos(800, 100); // Aparece a la derecha
}

void DIO::moverse()
{
    // 1. Calcular distancia horizontal hacia Jojo
    float dx = objetivo->x() - x();
    float dy = objetivo->y() - y();
    float distancia = std::sqrt(dx*dx + dy*dy);

    // 2. Aplicar gravedad
    vy += aceleracion_y;

    // 3. Lógica de DIO: DECISIÓN
    if (distancia < distanciaDeteccion && distancia > distanciaAtaque) {
        // Persecución: Si Jojo está a la derecha, vx es positivo; si no, negativo.
        vx = (dx > 0) ? 3 : -3;
    } else {
        vx = 0; // Se queda quieto si está muy lejos o muy cerca para atacar
    }

    // 4. MOVIMIENTO VERTICAL Y SALTO AUTOMÁTICO
    setPos(x(), y() + vy);
    if (verificarColision(x(), y())) {
        if (vy > 0) enSuelo = true;
        vy = 0;
    }

    // 5. MOVIMIENTO HORIZONTAL Y SALTO DE OBSTÁCULOS
    if (!verificarColision(x() + vx, y())) {
        setPos(x() + vx, y());
    } else {
        // Si choca con una pared (obstaculo) y está en el suelo, 'salta'
        if (enSuelo) {
            vy = -14;
            enSuelo = false;
        }
    }

    // 6. DECIDIR ATACAR
    if (distancia <= distanciaAtaque) {
        atacar();
    }
}

void DIO::atacar()
{

    if (barradeCarga < 100) barradeCarga += 5;

    if (barradeCarga >= 100) {
        habilidadEspecial();
    }
    //futura logica de restar vida a jojo
}

void DIO::habilidadEspecial()
{

    qDebug() << "DIO: MUDA MUDA MUDA! ZA WARUDO!";
    barradeCarga = 0;

}
