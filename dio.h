#ifndef DIO_H
#define DIO_H

#include "personaje.h"
#include "jojo.h"
#include <QPixmap>
#include <QList>
#include <QPainter>
#include <cmath>
#include <cstdlib>
#include <ctime>

class DIO : public Personaje
{
public:
    DIO(Personaje* jojo);

    bool estaAtacando = false;
    bool danoAplicado = false;
    bool puedeAtacar = true;
    int faseCombo = 0;
    int ticksEspera = 0;
    bool postEspecialJump = false; // Controla el salto de escape tras el Za Warudo
    bool yaHizoPausaLejos = false;

    enum { Type = UserType + 3 };
    int type() const override { return Type; }

    void setMirandoDerecha(bool derecha) { mirandoDerecha = derecha; }
    void moverse() override;
    void atacar() override;
    void atacarFuerte(int tipo);
    void actualizarAtaque();
    void actualizarAtaquesFuertes();
    void defensa();
    void habilidadEspecial() override;
    void actualizarEspecial();
    void procesarDano(QRectF area, int cantidad);
    void recibirDanoConOrigen(int cantidad, float atacanteX);
    void recibirDano(int cantidad);
    void saltar();

    void timeStop();
    void ejecutarCuracion();

    void evaluarHitboxBasico();
    void evaluarHitboxFuerte1();
    void evaluarHitboxFuerte2();
    void evaluarHitboxEspecial();

    enum EstadoDano { NORMAL, DANO1, DANO2, STANDUP, MUERTO };
    EstadoDano estadoDano = NORMAL;

private:
    Personaje* objetivo; // Puntero directo a Jotaro

    // --- Parámetros del Agente Inteligente ---
    int distanciaDeteccion = 500;
    int distanciaAtaque = 45;
    int distanciaFuerte = 85;
    int ticksDecision = 0; // Contador para no cambiar de opinión cada frame

    bool estaDefendiendo;
    int tiempoAtaque;
    bool stand = false;
    int frameActualStand = 0;
    short int ralentizadorStand = 0;
    short int ralentizadorJFuerte = 0;
    short int ralentizadorSP = 0;
    short int frameDioEsp = 0;
    short int frameTWEsp = 0;
    short int ralentDioEsp = 0;
    short int ralentTWEsp = 0;

    short int ticksDefensaRestantes = 0;
    bool modoDefensivoActivo = false;
    bool estaCurando = false;
    short int ticksCuracion = 0;
    short int cooldownCuracionTicks;
    short int comboTimeStopPaso;
    short int ticksPreTimeStop = 0;
    bool preparandoTimeStop = false;

    bool ataqueExentoDeCarga = false;

    // --- Contenedores de Animación (The World Sheet) ---
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
    QList<QPixmap> spritesDANO1;
    QList<QPixmap> spritesDANO2;
    QList<QPixmap> spritesSTANDUP;
    QList<QPixmap> spritesCURACION;
    QList<QPixmap> spritesTIMESTOP;

    int frameActual;
    int contadorAnimacion;
    bool mirandoDerecha;

    // --- Sistema de Daño Recibido ---
    int danioAcumulado = 0;
    int frameDano = 0;
    short int ralentizadorDano = 0;

    void cargarSprites();
    void activarDano1();
    void activarDano2(bool mitadEmpuje = false);
    void actualizarAnimDano();

    // --- Motor de Decisiones ---
    void ejecutarCerebro(float dx, float distancia);
};

#endif // DIO_H
