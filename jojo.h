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

    enum { Type = UserType + 2 }; // Define el ID de la clase
    int type() const override { return Type; } // Devuelve el ID

    //Pruebas con un Jotaro falso
    void setMirandoDerecha(bool derecha) { mirandoDerecha = derecha; }
    void setEsDummy(bool dummy) { esDummy = dummy; }

    void moverse() override;
    void atacar() override;
    void atacarFuerte(int tipo);
    void actualizarAtaque();
    void actualizarAtaquesFuertes();
    void defensa();
    void habilidadEspecial() override; // ¡Star Platinum: Za Warudo!
    void actualizarEspecial();
    // Auxiliares de combate
    void procesarDano(QRectF area, int cantidad);
    void recibirDano(int cantidad);
    void actualizarDano();
    // Controles de estado
    void saltar();
    void setVelocidadX(float v) { vx = v; }
    void setDefensa(bool d) {
        estaDefendiendo = d;
        frameActual = 0;
    }
    void setFrameActual(int f) { frameActual = f; }
    float getVelocidadX() { return vx; }

    void evaluarHitboxBasico();
    void evaluarHitboxFuerte1();
    void evaluarHitboxFuerte2();
    void evaluarHitboxEspecial();

private:
    bool estaDefendiendo;
    int tiempoAtaque; // Para manejar la duración de la animación

    bool stand = false;
    int frameActualStand = 0;

    bool esDummy = false;

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
