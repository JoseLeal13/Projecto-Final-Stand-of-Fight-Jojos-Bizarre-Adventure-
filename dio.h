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
#include <QGraphicsDropShadowEffect>
#include <utility>

class DIO : public Personaje
{
public:
    DIO(Personaje* jojo);
    ~DIO();

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
    void atacar(int tipo);
    void actualizarAtaque();
    void actualizarAtaquesFuertes();
    void defensa();
    void habilidadEspecial() override;
    void actualizarEspecial();
    void procesarDano(QRectF area, int cantidad);
    void recibirDano(int cantidad, float atacanteX);
    void recibirDano(int cantidad);
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

    void timeStop();
    void ejecutarCuracion();

    void evaluarHitboxBasico();
    void evaluarHitboxFuerte1();
    void evaluarHitboxFuerte2();
    void evaluarHitboxEspecial();

    enum EstadoDano { NORMAL, DANO1, DANO2, STANDUP, MUERTO };
    EstadoDano estadoDano = NORMAL;

    bool estaDefendiendo = false;
    int frameActual = 0;
    int contadorAnimacion = 0;
    bool mirandoDerecha;
    QList<QPixmap> spritesQUIETO;

private:
    Personaje* objetivo; // Puntero directo a Jotaro

    // --- Parámetros del Agente Inteligente ---

    int ticksEsperaParadoLejos = 0; // Controlará los 2 segundos (120 ticks) que se queda quieto lejos
    int cooldownAprendizajeTicks = 0; // Candado grande de tiempo para que no abuse de la defensa por aprendizaje
    bool saltoParabolicoEvasion = 0; // Flag para saber que está en el aire escapando

    int distanciaDeteccion = 500;
    int distanciaAtaque = 45;
    int distanciaFuerte = 85;
    int ticksDecision = 0; // Contador para no cambiar de opinión cada frame

    int cooldownDefensaCritica = 0;
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
    bool amagoEspecialCorto;
    short int ticksPreTimeStop = 0;
    bool preparandoTimeStop = false;

    bool ataqueExentoDeCarga = false;

    // --- Contenedores de Animación (The World Sheet) ---
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

    // --- Sistema de Daño Recibido ---
    int danioAcumulado = 0;
    int frameDano = 0;
    short int ralentizadorDano = 0;

    void cargarSprites();
    void activarDano();
    void activarDano(bool mitadEmpuje);
    void actualizarAnimDano();

    // --- Motor de Decisiones ---
    void ejecutarCerebro(float dx, float distancia);

    int ticksJugadorLejos = 0;
    int contadorGolpesRecibidos = 0;
    bool modoHiperagresivoActivo = false;
    bool contraataqueActivo = false;
    int  ticksContraataque = 0;

    // ── SECUENCIA DE CURACIÓN MEJORADA ──
    enum EstadoCuracionSeq {
        CUR_IDLE, CUR_DEFENSA, CUR_SALTANDO, CUR_ATERRIZANDO, CUR_EJECUTANDO, CUR_TIMESTOP
    };
    EstadoCuracionSeq estadoCuracion = CUR_IDLE;
    int ticksDefensaCuracion = 0;

    QGraphicsDropShadowEffect* elAuraEfecto = nullptr;
    void actualizarAuraVisual();

    QSoundEffect* sonidoZaWarudo = nullptr;
};

#endif // DIO_H
