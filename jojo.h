#ifndef JOJO_H
#define JOJO_H
#include "personaje.h"
#include <QPixmap>
#include <QList>
#include <QPainter>
#include <QGraphicsDropShadowEffect>

class Jojo : public Personaje
{
public:
    Jojo();
    bool estaAtacando = false;
    bool danoAplicado = false;
    bool puedeAtacar = true;
    int faseCombo = 0;
    enum { Type = UserType + 2 };
    int type() const override { return Type; }
    void setMirandoDerecha(bool derecha) { mirandoDerecha = derecha; }
    void setEsDummy(bool dummy) { esDummy = dummy; }
    void moverse() override;
    void atacar() override;
    void atacarFuerte(int tipo);
    void actualizarAtaque();
    void actualizarAtaquesFuertes();
    void defensa();
    void habilidadEspecial() override;
    void actualizarEspecial();
    void procesarDano(QRectF area, int cantidad);
    void recibirDano(int cantidad);
    void recibirDanoConOrigen(int cantidad, float atacanteX);
    void saltar();
    void setVelocidadX(float v) {
        if (estadoDano == DANO1 || estadoDano == DANO2 ||
            estadoDano == STANDUP || estadoDano == MUERTO) return;
        vx = v;
    }
    void setVelocidadY(float v) {
        if (estadoDano == DANO1 || estadoDano == DANO2 ||
            estadoDano == STANDUP || estadoDano == MUERTO) return;
        vy = v;
    }
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

    // Sobrecarga de moverse: Recibe las velocidades deseadas (vx, vy) desde el input del Nivel 1
    void moverse(float nuevoVx, float nuevoVy);
    // Sobrecarga de atacar: Recibe un tipo de golpe o dirección para interactuar con las pelotas
    void atacar(char tipoGolpe);
    // Sobrecarga de habilidadEspecial: Recibe un factor de escala o porcentaje
    void habilidadEspecial(float factorRalentizacion);
    // Sobrecarga del Constructor
    // Al recibir un QString, el compilador sabe que es el Jotaro del Nivel 1
    Jojo(QString rutaHojaNivel1);

    // Estado de daño recibido
    enum EstadoDano { NORMAL, DANO1, DANO2, STANDUP, MUERTO };
    EstadoDano estadoDano = NORMAL;

private:
    bool estaDefendiendo;
    int tiempoAtaque;
    bool stand = false;
    int frameActualStand = 0;
    short int ralentizadorStand = 0;
    short int ralentizadorJFuerte = 0;
    short int ralentizadorSP = 0;
    int frameJotaroEsp   = 0;
    int frameSPEsp       = 0;
    int ralentJotaroEsp  = 0;
    int ralentSPEsp      = 0;
    bool esDummy = false;

    bool ataqueExentoDeCarga = false;

    // Sprites existentes
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

    // Nuevos sprites de daño
    QList<QPixmap> spritesDANO1;
    QList<QPixmap> spritesDANO2;
    QList<QPixmap> spritesSTANDUP;

    int frameActual;
    int contadorAnimacion;
    bool mirandoDerecha;

    // Variables para sistema de daño
    int danioAcumulado = 0;         // daño acumulado en el ataque actual
    int frameDano = 0;              // frame actual de animación de daño
    short int ralentizadorDano = 0; // ralentizador para animaciones de daño
    bool recibiendoGolpes = false;  // true mientras llegan golpes del ataque actual

    void cargarSprites();
    void activarDano1();
    void activarDano2(bool mitadEmpuje = false);
    void actualizarAnimDano();

    // Sobrecarga de los sprites
    void cargarSprites(QString rutaHoja);

    enum Personalidad { NORMAL_ANIMO, CALCULADOR, ENOJADO };
    Personalidad animoActual = NORMAL_ANIMO;
    int ticksSinAtacar = 0;
    int contadorGolpesRecibidos = 0;
    bool proximoAtaquePotenciado = false;
    QGraphicsDropShadowEffect* elAuraEfecto = nullptr;
    void actualizarAuraVisual();

};
#endif
