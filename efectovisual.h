#ifndef EFECTOVISUAL_H
#define EFECTOVISUAL_H

#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QObject>
#include <QGraphicsScene>
#include <QPixmap>
#include <QImage>

class EfectoVisual : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
public:
    EfectoVisual(short int x, short int y, const QString& rutaSprite, QGraphicsScene* escena) {
        // 1. Cargamos como QImage para manipular los píxeles de forma directa
        QImage img(rutaSprite);

        if (img.isNull()) {
            return;
        }

        // 2. Convertimos a un formato que soporte canal Alfa (Transparencia)
        img = img.convertToFormat(QImage::Format_ARGB32);

        // 3. ALGORITMO DE FILTRADO POR UMBRAL DE BLANCO
        // Analiza los canales y remueve el recuadro blanco y sus bordes suavizados
        for (int yCoord = 0; yCoord < img.height(); ++yCoord) {
            for (int xCoord = 0; xCoord < img.width(); ++xCoord) {
                QRgb pixel = img.pixel(xCoord, yCoord);

                int r = qRed(pixel);
                int g = qGreen(pixel);
                int b = qBlue(pixel);

                // Si los tres canales están por encima de 220, es el fondo blanco
                // o el degradado claro del borde (anti-aliasing) de la tipografía.
                if (r > 220 && g > 220 && b > 220) {
                    // Lo volvemos 100% transparente (Alpha = 0)
                    img.setPixel(xCoord, yCoord, qRgba(0, 0, 0, 0));
                }
            }
        }

        // 4. Convertimos a Pixmap y escalamos con interpolación suave
        QPixmap pix = QPixmap::fromImage(img);
        QPixmap pixEscalado = pix.scaledToHeight(85, Qt::SmoothTransformation);
        setPixmap(pixEscalado);

        // Centramos el sprite sobre el punto de impacto
        setPos(x - (pixEscalado.width() / 2), y - (pixEscalado.height() / 2));

        // ZValue elevado para que flote por encima de Jotaro, DIO y las rejas de la jaula
        setZValue(2.0);

        escena->addItem(this);
        opacity = 1.0;

        // Timer de la animación de desvanecimiento
        timerAnimacion = new QTimer(this);
        connect(timerAnimacion, &QTimer::timeout, this, &EfectoVisual::actualizarEfecto);
        timerAnimacion->start(20);
    }

private slots:
    void actualizarEfecto() {
        opacity -= 0.04; // Desvanecimiento progresivo
        if (opacity <= 0) {
            timerAnimacion->stop();
            if (scene()) {
                scene()->removeItem(this);
            }
            deleteLater();
        } else {
            setOpacity(opacity);
            setY(y() - 1.8); // Desplazamiento constante hacia arriba
        }
    }

private:
    double opacity;
    QTimer* timerAnimacion;
};

#endif // EFECTOVISUAL_H
