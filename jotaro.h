#ifndef JOTARO_H
#define JOTARO_H

#include "personaje.h"
#include <QPixmap>
#include <QList>
#include <QPainter>
#include <QRectF>

class jotaro : public Personaje
{
public:
    jotaro();

    // Identificador único de tipo (Estilo de tu amigo para detectar quién es quién)
    enum { Type = UserType + 2 };
    int type() const override { return Type; }

    // Métodos obligatorios de la clase base Personaje
    void moverse() override;
    void atacar() override;
    void habilidadEspecial() override;

    // Métodos de control de movimiento que ya usas en tu MainWindow
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();

    // Getters lógicos para tus hitboxes y estados
    QRectF getHitbox();
    QRectF getAttackHitbox();

    void setDireccion(int dir);
    void setAtacando(bool status);
    void setEnMovimiento(bool status);
    void actualizarFrame(int frameActualIndex);

    // Métodos obligatorios de Qt para que el personaje se dibuje SOLO en la escena
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    // Variables de estado de animación
    int anchoHitbox = 70;
    int altoHitbox  = 100;
    int direccion;
    bool enMovimiento;
    bool atacando;
    int frameActual;

    // Contenedores de tus sprites (Mudados desde MainWindow)
    QList<QPixmap> framesQuieto[4];
    QList<QPixmap> framesMovimiento[4];
    QList<QPixmap> framesAtaqueDerecha;
    QList<QPixmap> framesAtaqueIzquierda;

    // Métodos auxiliares de procesamiento de imágenes
    void cargarFrames();
    QPixmap quitarFondo(const QPixmap &original);
    QPixmap reflejarHorizontal(const QPixmap &original);
};

#endif // JOTARO_H
