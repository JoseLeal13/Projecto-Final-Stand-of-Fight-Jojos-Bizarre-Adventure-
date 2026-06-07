#ifndef STEELBALL_H
#define STEELBALL_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QList>
#include <QPixmap>
#include <cmath>
#include <QImage>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QStyleOptionGraphicsItem> // Añadido para evitar errores con la firma de paint
#include <QDebug>

class SteelBall : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    // Le indicamos a Qt las interfaces que estamos usando para las colisiones
    Q_INTERFACES(QGraphicsItem)

public:
    enum TipoBola {
        VerdeGolpeable,
        RojaEsquivable
    };

    enum TipoTrayectoria {
        Recta,
        Parabolica,
        Senoidal
    };

    SteelBall(TipoBola tipo, TipoTrayectoria trayectoria, qreal posX, qreal posY, int direccionX);

    TipoBola getTipo() const { return tipoActual; }

    // CORRECCIÓN: Se agrega 'const' para que sea compatible con funciones constantes
    QRectF getHitbox() const { return boundingRect(); }

    void recibirGolpe();
    void avanzarFisica();
    void setRalentizada(bool activa) { ralentizada = activa; }

    // Dice si la bola ya esta en modo caida (para que mainwindow la borre cuando salga)
    bool estaCayendo() const { return cayendo; }

    // Métodos gráficos y de colisión nativos de QGraphicsPixmapItem
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    bool collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape) const override;

private:
    TipoBola tipoActual;
    TipoTrayectoria trayectoriaActual;

    void cargarGraficos();
    QPixmap quitarFondo(const QPixmap &original);

    // Variables de movimiento y física
    qreal velocidadX;
    qreal velocidadY;
    int dirX;
    bool ralentizada = false;

    qreal tiempoInterno;
    qreal posYInicial;

    // ── ESTADO DE CAIDA (cuando Jotaro golpea la bola) ──
    // Cuando la bola recibe un golpe no desaparece inmediatamente,
    // sino que cae con gravedad hasta salir de pantalla
    bool cayendo;
    qreal velocidadCaidaY;  // Velocidad vertical durante la caida

    // ── VARIABLES PARA LA ANIMACIÓN DE LA BOLA ──
    QList<QPixmap> framesAnimacion; // Contenedor para los 4 sprites de giro
    int frameActual;                // Índice del frame que se está mostrando (0 a 3)
    int contadorFrames;             // Tick interno de frames
    int retardoFrames;

    // ── EFECTO VISUAL DE GOLPE (destello morado al ser golpeada) ──
    // En vez de crear una clase aparte, guardamos los frames aqui mismo
    // y los dibujamos encima de la bola en nuestro propio paint()
    QList<QPixmap> framesEfectoGolpe; // Los 6 frames de la estrella morada
    int frameEfecto;                  // En qué frame del efecto estamos
    int contadorEfecto;               // Contador de ticks para el efecto
    bool mostrandoEfecto;             // true mientras el destello está activo
    void cargarEfectoGolpe();         // Carga los frames morados del sprite sheet

    // Métodos de geometría esenciales para el motor de Qt
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
};

#endif // STEELBALL_H