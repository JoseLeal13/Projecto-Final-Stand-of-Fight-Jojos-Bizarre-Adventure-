#ifndef JOJO_H
#define JOJO_H

#include "personaje.h"
#include <QPixmap>
#include <QList>
#include <QPainter>

class Jojo : public Personaje
{
public:
    Jojo();

    bool estaAtacando = false;
    bool danoAplicado = false;
    bool puedeAtacar = true;
    int faseCombo = 0;



    void moverse() override;
    void atacar() override;
    void atacarFuerte(int tipo);
    void actualizarAtaque();
    void actualizarAtaquesFuertes();
    void defensa();
    void habilidadEspecial() override; // ¡Star Platinum: Za Warudo!
    // Auxiliares de combate
    void procesarDano(QRectF area, int cantidad);
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

    bool stand = false;
    int frameActualStand = 0;

    // Hojas de sprites recortadas por acción
    QList<QPixmap> spritesQUIETO;
    QList<QPixmap> spritesCAMINAR;
    QList<QPixmap> spritesSALTO;
    QList<QPixmap> spritesDEFENSA;
    QList<QPixmap> spritesATACAR;
    QList<QPixmap> spritesBASICO1;
    QList<QPixmap> spritesBASICO2;
    QList<QPixmap> spritesFUERTE1;
    QList<QPixmap> spritesFUERTE2;
    QList<QPixmap> spritesESPECIAL;

    // Variables de control de animación
    int frameActual;
    int contadorAnimacion;
    bool mirandoDerecha;

    // Función interna para cargar y recortar los sprites
    void cargarSprites();
};

#endif
