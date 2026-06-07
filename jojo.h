#ifndef JOJO_H
#define JOJO_H
#include "personaje.h"
#include <QPixmap>
#include <QList>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <utility>

// Operador * : Multiplica un par STL por un flotante (Para aplicar la ESCALA)
inline std::pair<float, float> operator*(const std::pair<float, float>& par, float escalar) {
    return std::make_pair(par.first * escalar, par.second * escalar);
}

// Operador + : Suma dos pares STL (Para mover o desplazar coordenadas)
inline std::pair<float, float> operator+(const std::pair<float, float>& a, const std::pair<float, float>& b) {
    return std::make_pair(a.first + b.first, a.second + b.second);
}

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
    void atacar(int tipo);
    void actualizarAtaque();
    void actualizarAtaquesFuertes();
    void defensa();
    void habilidadEspecial() override;
    void actualizarEspecial();
    void procesarDano(QRectF area, int cantidad);
    void recibirDano(int cantidad);
    void recibirDano(int cantidad, float atacanteX);
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

    void moverse(float nuevoVx, float nuevoVy);
    void atacar(char tipoGolpe);
    void habilidadEspecial(float factorRalentizacion);
    // Sobrecarga del Constructor
    // Al recibir un QString, el compilador sabe que es el Jotaro del Nivel 1
    Jojo(QString rutaHojaNivel1);

    // Estado de daño recibido
    enum EstadoDano { NORMAL, DANO1, DANO2, STANDUP, MUERTO };
    EstadoDano estadoDano = NORMAL;


    // Bool que indica si estamos en nivel 1 (lógica de jotaro tuya)
    bool esNivel1 = false;

    // Variables de Jotaro Nivel
    int speed;
    int direccion;
    bool enMovimiento;
    bool atacando;
    int vida;
    bool invencible;
    int contadorInvencible;
    bool mostrarHitboxInterna = false;
    int energiaUlti;
    bool ultiActiva;
    int timerUlti;
    int velocidadBonus;
    int timerVelocidad;

    // Métodos de Jotaro Nivel 1
    void cargarFramesNivel1();
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void setDireccion(int dir);
    void setAtacando(bool status);
    void setEnMovimiento(bool status);
    void actualizarFrame(int frameActualIndex);
    void recibirDanio(int cantidad); // Nota: Nivel 2 usa recibirDano (sin 'i')
    void actualizarInvulnerabilidad();
    void cargarEnergia(int cantidad);
    void usarUlti();
    bool estaUltiActiva() const { return ultiActiva; }
    void curar(int cantidad);
    void aumentarVelocidad();
    void actualizarEfectosItems();
    QRectF getHitbox();
    QRectF getAttackHitbox();
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setMostrarHitbox(bool status) { mostrarHitboxInterna = status; update(); }
    int getEnergia() const { return energiaUlti; }
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
    void activarDano();
    void activarDano(bool mitadEmpuje);
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

    //sprites jotaro nivel1
    QList<QPixmap> framesQuieto[4];
    QList<QPixmap> framesMovimiento[4];
    QList<QPixmap> framesAtaqueDerecha;
    QList<QPixmap> framesAtaqueIzquierda;

    QPixmap quitarFondoN1(const QPixmap &original);
    QPixmap reflejarHorizontalN1(const QPixmap &original);

};
#endif
