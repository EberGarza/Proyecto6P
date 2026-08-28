# Manual del Usuario

## Qué es VirtualPet

Una mascota virtual que vive en tu ordenador y depende de que la cuides. Tiene
cinco necesidades que se van agotando solas con el tiempo, y su estado de ánimo
y su salud dependen de lo bien que las atiendas. Si la descuidas lo suficiente,
se enferma. Y si la enfermedad no se trata, muere.

## Cómo empezar

Al abrir el juego aparece la pantalla de selección.

![Selección de mascota](../assets/images/screenshots/menu.png)

1. **Elige la especie** pulsando uno de los cuatro botones. La vista previa se
   actualiza al momento y debajo aparece una descripción de su carácter.
2. **Escribe el nombre** directamente con el teclado. Se borra con retroceso y
   admite hasta 14 caracteres.
3. Pulsa **Comenzar** o la tecla `Enter`.

Si ya habías jugado antes, arriba a la derecha aparece **Continuar partida**,
que recupera la mascota tal y como la dejaste.

## Las cinco barras

Todas siguen la misma regla: **cuanto más llena, mejor**.

| Barra | Vacía significa | Se recupera con |
|-------|-----------------|-----------------|
| Saciedad | Está famélica | Alimentar |
| Felicidad | Está deprimida | Jugar, acariciar |
| Energía | Está agotada | Dormir |
| Higiene | Está sucia | Asear |
| Salud | Se muere | Medicar |

La **salud es distinta a las demás**: no baja sola. Baja cuando la saciedad, la
higiene o la felicidad llegan a niveles críticos, y vuelve a subir por su cuenta
si la mascota está bien cuidada. Es decir, descuidar una cosa acaba pagándose en
otra.

## Las acciones

![Pantalla de juego](../assets/images/screenshots/gameplay.png)

Los botones de abajo, o las teclas `1` a `6`:

| Botón | Tecla | Qué hace |
|-------|-------|----------|
| Alimentar | `1` | Sube la saciedad. Ensucia un poco |
| Jugar | `2` | Sube mucho el ánimo, gasta energía y da hambre |
| Asear | `3` | Sube la higiene. A la mascota no le hace gracia |
| Medicar | `4` | Sube la salud. Sabe mal, así que baja el ánimo |
| Dormir | `5` | Se duerme y recupera energía. Vuelve a pulsar para despertarla |
| Acariciar | `6` | Sube un poco el ánimo, gratis |

El **último botón cambia según la especie**: Gastly puede *Asustar*, el dragón
*Escupir fuego* y el gato *Acicalarse*. El perro no tiene acción propia.

Debajo de las barras, la **bitácora** va contando lo que pasa: qué comió, cuándo
se durmió, cuándo se puso enferma.

## Los ocho estados

La mascota está siempre en uno de estos estados, y el juego lo muestra debajo de
su nombre.

| Estado | Cuándo ocurre |
|--------|---------------|
| **Normal** | Todo en orden |
| **Feliz** | La felicidad pasa de 80 |
| **Hambrienta** | La saciedad baja de 20 |
| **Cansada** | La energía baja de 20 |
| **Durmiendo** | Le has dicho que duerma, o se ha desmayado de agotamiento |
| **Enferma** | La salud baja de 30. Sólo mejora con medicina |
| **Jugando** | Estado temporal de unos segundos |
| **Muerta** | La salud llegó a cero. No hay vuelta atrás |

Cuando hay varias necesidades a la vez, mandan por este orden: **muerte,
enfermedad, hambre, cansancio**.

Mientras duerme **no acepta interacciones**. Puedes despertarla, pero si lo
haces antes de tiempo se enfada y pierde ánimo.

## Guardar la partida

El juego **guarda solo al cerrarse** con `Escape` o con la X de la ventana. La
partida queda en un archivo `partida.txt` junto al ejecutable, en texto plano y
legible.

Al volver a abrir, la pantalla de selección ofrece continuar donde lo dejaste.

## Problemas frecuentes

**El juego no arranca y Windows no dice por qué.**
Faltan las DLL. La carpeta de entrega tiene que llevar los archivos `.dll` junto
a `game.exe`. Si compilaste tú, cópialas de `C:\msys64\ucrt64\bin`.

**Arranca pero se ve sin la mascota, o avisa de la fuente.**
Estás ejecutándolo desde otra carpeta. El juego busca `assets/` con rutas
relativas, así que hay que lanzarlo desde la raíz del proyecto.

**Se abre sin fuente bonita.**
Es normal: si no encuentras `assets/fonts/fuente.ttf`, el juego usa una fuente
del sistema. No afecta a nada más.

**Se me murió la mascota.**
No se puede revivir desde el juego. Cierra, borra `partida.txt` y empieza otra.
