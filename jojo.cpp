#include "jojo.h"

Jojo::Jojo() : Personaje()
{

    setPixmap(QPixmap(":/sprites/SpritesJojoChampionship/Jotaro Kujo sprites.png").scaled(60, 90));
    estaDefendiendo = false;
    tiempoAtaque = 0;

    cargarSprites();

    // Inicializar variables de animación
    frameActual = 0;
    contadorAnimacion = 0;
    mirandoDerecha = true;

    // colocar sprite inicial
    setPixmap(spritesQUIETO.at(0));
}

void Jojo::cargarSprites() {
    QImage imagen(":/sprites/SpritesJojoChampionship/Jotaro Kujo sprites.png");
    QRgb colorFondo = imagen.pixel(0,0);
    imagen.setAlphaChannel(imagen.createMaskFromColor(colorFondo, Qt::MaskOutColor));
    QPixmap hoja = QPixmap::fromImage(imagen);

    // --- STAND (7,29 hasta 7,94 de alto | 17,19 hasta 17,94 de largo) ---
    // Interpretando tus puntos: X_inicio=7, Y_inicio=29. Ancho=47, Alto=65.
    for(int i = 0; i < 6; i++) {
        spritesQUIETO.append(hoja.copy(7 + (i * 41), 29, 42, 67));
    }

    // --- WALK (Caminar) ---
    // Mantengo el inicio en 313 que mediste antes para que no se desfase
    for(int i = 0; i < 8; i++) {
        spritesCAMINAR.append(hoja.copy(313 + (i * 48), 29, 47, 67));
    }

    // --- GUARD (165,126 hasta 165,198 de alto | 205,126 hasta 205,198 de largo) ---
    // X=165, Y=126. Ancho=40 (205-165), Alto=72 (198-126)
    for(int i = 0; i < 3; i++) {
        spritesDEFENSA.append(hoja.copy(165 + (i * 40), 126, 39, 72));
    }

    // --- JUMP (417,126 hasta 417,199 | 468,126 hasta 468,199) ---
    // X=417, Y=126. Ancho=51 (468-417), Alto=73 (199-126)
    for(int i = 0; i < 8; i++) {
        spritesSALTO.append(hoja.copy(415 + (i * 48), 126, 47, 72));
    }
}

void Jojo::moverse()
{
    // 1. LÓGICA DE ESTADO (Defensa cancela velocidad horizontal)
    if (estaDefendiendo) {
        vx = 0;
    }

    // 2. FÍSICA: GRAVEDAD
    vy += aceleracion_y;

    // 3. MOVIMIENTO VERTICAL (Salto/Caída)
    if (!verificarColision(x(), y() + vy)) {
        setPos(x(), y() + vy);
        enSuelo = false;
    } else {
        if (vy > 0) enSuelo = true;
        vy = 0;
    }

    // 4. MOVIMIENTO HORIZONTAL
    if (!verificarColision(x() + vx, y())) {
        setPos(x() + vx, y());
    }

    // 5. SELECCIÓN DE ANIMACIÓN (Aquí es donde estaba el lío)
    QList<QPixmap> *animacionActual;
    bool bucle = true;

    if (estaDefendiendo) {
        animacionActual = &spritesDEFENSA;
        bucle = false;
    } else if (!enSuelo) {
        animacionActual = &spritesSALTO;
        bucle = true;
    } else if (vx != 0) {
        animacionActual = &spritesCAMINAR;
        bucle = true;
    } else {
        animacionActual = &spritesQUIETO;
        bucle = true;
    }

    // 6. CONTROL DE VELOCIDAD DE ANIMACIÓN (Ralentizador)
    static int ralentizador = 0;
    ralentizador++;
    if (ralentizador >= 6) {
        ralentizador = 0;
        frameActual++;
    }

    // 7. CONTROL DE BUCLE (Para que camine o respire indefinidamente)
    if (frameActual >= animacionActual->size()) {
        if (bucle) {
            frameActual = 0;
        } else {
            frameActual = animacionActual->size() - 1;
        }
    }

    // 8. ORIENTACIÓN (Mirar a la izquierda o derecha)
    if (vx > 0) mirandoDerecha = true;
    else if (vx < 0) mirandoDerecha = false;

    // 9. RENDERIZADO DEL SPRITE FINAL
    QPixmap frameFinal = animacionActual->at(frameActual);

    if (!mirandoDerecha) {
        // Volteamos la imagen horizontalmente si camina a la izquierda
        frameFinal = frameFinal.transformed(QTransform().scale(-1, 1));
    }

    setPixmap(frameFinal);
}

void Jojo::saltar()
{
    if (enSuelo && !estaDefendiendo) {
        vy = -15; // Impulso hacia arriba
        enSuelo = false;
    }
}

void Jojo::atacar() {

    aumentarCarga(5);
    qDebug() << "Atacando... Carga actual:" << barradeCarga;

}

void Jojo::habilidadEspecial() {
    if (barradeCarga >= 100) {
        qDebug() << "¡STAR PLATINUM: Za Warudo!";

        barradeCarga = 0;
    } else {
        qDebug() << "Yare yare daze. Carga necesaria: 100. Actual:" << barradeCarga;
    }
}

void Jojo::defensa()
{
    estaDefendiendo = !estaDefendiendo;
    if (estaDefendiendo) {
        qDebug() << "Jojo se está cubriendo.";
        // Reducir daño recibido en la lógica de colisión futura
    }
}
