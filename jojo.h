#ifndef JOJO_H
#define JOJO_H

#include "personaje.h"
#include <QPixmap>
#include <QList>

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
    void setDefensa(bool d) {
        estaDefendiendo = d;
        frameActual = 0;
    }
    void setFrameActual(int f) { frameActual = f; }
    float getVelocidadX() { return vx; }

private:
    bool estaDefendiendo;
    int tiempoAtaque; // Para manejar la duración de la animación

    // Hojas de sprites recortadas por acción
    QList<QPixmap> spritesQUIETO;
    QList<QPixmap> spritesCAMINAR;
    QList<QPixmap> spritesSALTO;
    QList<QPixmap> spritesDEFENSA;
    QList<QPixmap> spritesATACAR;

    // Variables de control de animación
    int frameActual;
    int contadorAnimacion;
    bool mirandoDerecha;

    // Función interna para cargar y recortar los sprites
    void cargarSprites();
};

#endif
