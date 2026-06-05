    #ifndef JOTARO_H
    #define JOTARO_H

    #include "personaje.h"
    #include <QPixmap>
    #include <QList>
    #include <QPainter>
    #include <QRectF>
    #include <QDebug>

    class jotaro : public Personaje
    {
    public:
        jotaro();

        enum { Type = UserType + 2 };
        int type() const override { return Type; }

        // Métodos obligatorios de la clase base
        void moverse() override;
        void atacar() override;
        void habilidadEspecial() override;

        // Métodos de control de movimiento adaptados a los buffs
        void moveUp();
        void moveDown();
        void moveLeft();
        void moveRight();

        // Getters lógicos para hitboxes y estados
        QRectF getHitbox();
        QRectF getAttackHitbox();

        void setDireccion(int dir);
        void setAtacando(bool status);
        void setEnMovimiento(bool status);
        void actualizarFrame(int frameActualIndex);

        // ── GESTIÓN DE SALUD, ITEMS Y ULTI ──
        int getVida() const { return vida; }
        bool esInvencible() const { return invencible; }
        void recibirDanio(int cantidad);
        void actualizarInvulnerabilidad();

        // Métodos para la mecánica de la Ulti (Za Warudo)
        int getEnergia() const { return energiaUlti; }
        void cargarEnergia(int cantidad);
        void usarUlti();
        bool estaUltiActiva() const { return ultiActiva; }

        // Métodos para los Ítems consumibles
        void curar(int cantidad);
        void aumentarVelocidad();
        void actualizarEfectosItems();

        // Métodos obligatorios de Qt para renderizado
        QRectF boundingRect() const override;
        QPainterPath shape() const override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

        void setMostrarHitbox(bool status) { mostrarHitboxInterna = status; update(); }

    private:
        int anchoHitbox;
        int altoHitbox;
        int direccion;
        bool enMovimiento;
        bool atacando;
        int frameActual;

        // Variables de salud e inmunidad
        int vida;
        bool invencible;
        int contadorInvencible;
        bool mostrarHitboxInterna = false;

        // ── VARIABLES NUEVAS DE MECÁNICAS (Sincronizadas con jotaro.cpp) ──
        int energiaUlti;
        bool ultiActiva;
        int timerUlti;
        int velocidadBonus;
        int timerVelocidad;

        // Contenedores de sprites de animación
        QList<QPixmap> framesQuieto[4];
        QList<QPixmap> framesMovimiento[4];
        QList<QPixmap> framesAtaqueDerecha;
        QList<QPixmap> framesAtaqueIzquierda;

        void cargarFrames();
        QPixmap quitarFondo(const QPixmap &original);
        QPixmap reflejarHorizontal(const QPixmap &original);
    };

    #endif // JOTARO_H