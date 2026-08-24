# Manual del Programador

## La decisión de fondo: dos capas

`include/` y `src/` son planos, pero el código está pensado en dos capas y la
separación se respeta con disciplina:

> **La lógica del juego no conoce SFML.** Ni un solo `#include <SFML/...>` en
> las clases de la capa lógica.

No es purismo. Mantiene el modelo del juego independiente de cómo se dibuje, y
por eso se pudo cambiar de SFML 2 a SFML 3, y añadir un segundo motor de dibujo,
sin tocar ni una línea de la lógica.

| Capa | Clases | Depende de |
|------|--------|------------|
| Lógica | `Mascota` y las especies, `Atributo`, `Estado` y los ocho estados, `MaquinaEstados`, `TipoEstado`, `Objeto` y derivadas, `Inventario`, `FabricaMascotas`, `GestorGuardado`, `AdminMenu`, `Utilidades` | nada externo |
| Gráfica | `Juego`, `Pantalla` y derivadas, `RenderMascota` y derivadas, `VistaMascota`, `HojaSprites`, `Animacion`, `Hud`, `Boton`, `BarraAtributo`, `PanelAdmin`, `Tema`, `GestorRecursos`, `Music`, `MusicButton` | de la lógica y de SFML |

La flecha va siempre en un sentido. Si alguna clase de la primera columna
necesita incluir SFML, es señal de que está en la capa equivocada.

## Las jerarquías

El diagrama completo está en [`diagrama_clases.puml`](./diagrama_clases.puml)
(43 clases, 57 relaciones). Estas son las cinco que sostienen el diseño.

### Mascota — herencia y polimorfismo

```
Mascota (abstracta)
 ├── Conejo    saltar()
 └── Castor    roer()
```

Cada especie define sus tasas de desgaste, su sonido, su comida favorita y una
acción exclusiva. La acción exclusiva se resuelve con `dynamic_cast` al construir
la botonera.

**El género no es una clase.** Cualquier especie puede ser macho o hembra, y eso
no cambia su comportamiento: solo cambia la hoja de sprites y cómo se habla de
ella. Por eso `Genero` es un dato de `Mascota` y no una rama más de la jerarquía.
Hacerlo al revés duplicaría cada especie sin añadir nada.

`Mascota::claveArte()` junta las dos cosas y devuelve `"conejo_macho"`, que es el
nombre base del archivo de sprites. Es texto, así que puede vivir en la capa
lógica; quien lo convierte en una ruta es `RenderSprite::rutaDe()`.

**Cuidado con el constructor.** `configurarRasgos()` es virtual pura, pero **no
se puede llamar desde el constructor de `Mascota`**: durante la construcción de
la clase base todavía no existe la parte derivada y el despacho virtual apuntaría
a `Mascota`. Por eso la llama cada constructor hijo en su propio cuerpo.

### Estado — patrón State

```
Estado (abstracta)
 ├── EstadoNormal      ├── EstadoDurmiendo
 ├── EstadoFeliz       ├── EstadoEnferma
 ├── EstadoHambrienta  ├── EstadoJugando
 └── EstadoCansada     └── EstadoMuerta
```

Cada estado es una clase con `alEntrar()`, `actualizar()` y `alSalir()`, y decide
él mismo a qué otros estados puede pasar. Las reglas comunes de prioridad
(muerte, enfermedad, hambre, cansancio) están en `Estado::transicionCritica()`,
para no repetirlas en los ocho.

**El detalle que no es obvio:** un estado puede pedir un cambio *desde dentro de
su propio* `actualizar()`. Si el cambio se aplicara inmediatamente, se destruiría
el objeto `Estado` mientras uno de sus métodos sigue en la pila. Por eso
`MaquinaEstados` guarda el cambio en `pendiente_` y lo aplica en cuanto
`actualizar()` termina.

`Muerta` es terminal de verdad: `cambiarEstado()` ignora cualquier petición una
vez dentro. La única salida es `Mascota::revivir()`, que se salta el guardia
hablando directamente con la máquina.

### Objeto — Template Method

```
Objeto (abstracta)
 ├── Alimento
 ├── Juguete
 ├── Medicina
 └── ArticuloAseo
```

`usar()` vive en la base y contiene el flujo común: comprobar usos, aplicar,
descontar. Lo único que define cada derivada es `aplicar()`. Así, **añadir una
comida nueva no obliga a tocar `Mascota`**.

### RenderMascota — Strategy

```
RenderMascota (abstracta)
 ├── RenderSprite      dibuja con la hoja de sprites
 └── RenderProcedural  dibuja con figuras geométricas
```

`VistaMascota` es la fachada: busca `assets/images/<especie>.txt` y, si no
existe, cae en el dibujo procedural. El resto del juego no sabe cuál está
activo. Gracias a eso el juego se ve aunque falte el arte.

### Pantalla — State a nivel de aplicación

```
Pantalla (abstracta)
 ├── PantallaMenu
 ├── PantallaOpciones
 ├── PantallaSeleccion
 └── PantallaJuego
```

`Juego` sólo conoce la interfaz y cambia de pantalla cuando la activa se lo pide,
devolviendo un valor de `Pantalla::Transicion`. Añadir una tienda o unos créditos
no obliga a tocar el bucle principal.

El cambio se hace **al final del fotograma**, nunca dentro del `actualizar()` de
la pantalla que lo pide: si no, se destruiría a sí misma mientras se ejecuta.

## Los atributos

`Atributo` es un valor acotado con una tasa de cambio por segundo. La mascota
tiene cinco, y todos siguen la convención **"más alto es mejor"** — por eso la
primera se llama *saciedad* y no *hambre*. Una sola regla mental, y las barras se
dibujan todas igual.

La salud es la excepción: no tiene tasa propia. La calcula
`Mascota::aplicarEfectosSecundarios()` a partir de las otras cuatro. Eso es lo
que convierte cinco números independientes en un sistema.

## La hoja de sprites

Cada combinación de especie y género tiene su propio archivo:
`conejo_macho`, `conejo_hembra`, `castor_macho`, `castor_hembra`, con un `.png`
y un `.txt` que describe los recortes. `HojaSprites` lee el `.txt`, carga la
imagen que este indique y construye una `Animacion` por estado.

Las cuatro hojas están dibujadas sobre una **rejilla exacta de 64×64**: cuatro
columnas por nueve filas. Cada fila es una animación:

| Fila | Qué muestra | Se usa para |
|------|-------------|-------------|
| 0 | reposo, respirando | `Normal` |
| 1 | salto con destello y corazón | `Feliz` |
| 2 | comiendo | `Hambrienta` |
| 3 | ceño fruncido y gota de sudor | `Cansada` |
| 4 | dormido, con las Z | `Durmiendo` |
| 5 | jugando con la pelota | `Jugando` |
| 6 | baño con burbujas | acción `Aseo` |
| 7 | enfermo, con mascarilla (2 poses) | `Enferma` |
| 8 | con aureola (1 pose) | `Muerta` |

El archivo sigue resolviendo dos cosas que la imagen sola no puede:

1. **El PNG no tiene transparencia.** El fondo es un verde plano opaco, así que
   al cargar se recorre la imagen y se vuelve transparente todo píxel cercano a
   ese color, con una tolerancia. Al ser PNG sin pérdida basta con 12; con el
   JPG antiguo hacía falta 30 y aun así dejaba halo.
2. **Faltan poses para algún estado.** Se puede declarar un tinte por estado
   para reutilizar otra animación cambiándole el color.

### Animaciones de acción

No todo lo que dibuja una hoja es un estado. El baño no describe *cómo está* la
mascota, sino algo que **hace un momento y termina**. Esas animaciones se
declaran igual, con un nombre que no es ningún estado, y `HojaSprites` las
guarda en un mapa aparte:

```cpp
const Animacion* accion(const std::string& nombre) const;
```

`RenderSprite::reproducirAccion("Aseo")` cambia a esa animación, marca
`enAccion_` y la deja correr sin bucle. Cuando `Animacion::terminada()` da true,
vuelve a la del estado. Un cambio de estado a mitad de la acción también la
corta: si la mascota se duerme mientras se baña, lo que hay que ver es que se
durmió.

`RenderMascota::reproducirAccion()` **no es virtual pura**: el dibujo procedural
no sabe hacer acciones y no tiene por qué implementarla. Quien la llama no
pregunta antes; si no hay animación, no pasa nada.

### Por qué se ve nítido

Tres cosas, y las tres importan:

- **Sin suavizado** (`setSmooth(false)`): ampliación por vecino más cercano.
- **Factor de aumento entero** (x3 en la partida y en la vista previa). Con un
  factor fraccionario unos píxeles de origen ocupan 3 en pantalla y otros 4, y al
  animarse eso hormiguea.
- **La textura arranca en un píxel entero.** `RenderSprite::ajustarAPixel()`
  redondea la esquina del sprite, no su posición, para no perder el anclaje.

### Por qué el anclaje va sobre la celda

Los puntos `ox oy` de cada cuadro son los mismos en toda la hoja: `32 28`, es
decir un punto fijo de la **celda**, no del dibujo.

Eso es deliberado, y es lo contrario de lo que pedía la hoja antigua. En aquella
las poses no estaban alineadas, así que el anclaje se medía sobre la cara de
cada pose para que la animación no saltara. Aquí el artista dibujó el movimiento
*dentro* de la celda: en la fila de `Feliz` la sombra del suelo se queda quieta y
el cuerpo sube y baja respecto a ella. Anclando por celda, ese salto se conserva.
Si se anclara sobre el cuerpo, el motor lo "corregiría" y la animación quedaría
plana.

La regla general: **anclar donde el artista puso el origen**. Si la hoja está en
rejilla, es la celda; si no lo está, hay que medirlo.

## La interfaz de recreativa

Toda la presentación pasa por `Tema.hpp`, que es a la vez la paleta y una caja de
herramientas de dibujo. Hay una razón para centralizarlo: el aspecto de máquina
recreativa no sale de una imagen de fondo, sino de repetir cuatro gestos en todas
partes. Si cada clase se los dibuja por su cuenta, dejan de coincidir.

| Herramienta | Qué resuelve |
|-------------|--------------|
| `paralelogramo()` | Un `sf::ConvexShape` de cuatro puntos con el lado inclinado. Es la forma de las barras |
| `panelBiselado()` | Devuelve borde, cara y dos filos claros. Da el relieve de plástico de los botones y paneles |
| `dibujarConSombra()` | Dibuja el mismo texto dos veces, la de abajo en negro y desplazada. Lo despega del fondo |
| `conAlfa()` | Un color de la paleta con otra transparencia, sin duplicar constantes |

### Las barras

`BarraAtributo` recibe el `Atributo` y el `dt` del fotograma. El `dt` está ahí
por el **rastro**: la barra guarda dos niveles, el real y uno rezagado que sólo
sabe bajar, a velocidad fija.

```cpp
if (rastro_ > nivel_) rastro_ = std::max(nivel_, rastro_ - kCaidaRastro * dt);
else                  rastro_ = nivel_;
```

Cuando el valor sube, el rastro salta al momento y no se ve nada. Cuando baja de
golpe, se queda atrás un bloque rojo que va cayendo. Es el mismo recurso que usan
esos juegos para que un golpe se lea, y aquí cumple lo mismo: un descuido se
*ve*, no hay que estar mirando el número.

Encima se dibujan muescas cada 26 px, para poder estimar el nivel sin leerlo.

### El escenario

`PantallaJuego` compone el fondo **una sola vez**, en `construirEscenario()`: un
`sf::RenderTexture` donde se repite en mosaico la propia mascota, igual que el
menú. Al salir de su hoja, el fondo cambia con la especie y el género elegidos.
Después es un único `sf::Sprite`, así que repetirlo no cuesta nada por fotograma.

`construirBarrido()` prepara un `sf::VertexArray` de líneas negras
semitransparentes cada 3 px, que se dibuja al final sobre todo lo demás. Imita el
barrido de un monitor de tubo. Al ser vértices y no una textura, no consume
memoria de vídeo ni se deforma si cambia el tamaño de la ventana.

### El anuncio de estado

La pantalla se queda con el último estado que anunció. Cuando `TipoEstado` cambia
respecto al guardado, se lanza el cartel en mitad del escenario con una cuenta
atrás propia. Es la única animación de la interfaz que no depende de la mascota
sino del *cambio*, y por eso necesita recordar el valor anterior.

## Menú, música y opciones

`PantallaMenu`, `PantallaOpciones`, `Music` y `MusicButton` vienen del proyecto
de tercer parcial, adaptados a SFML 3 y a la estructura de pantallas de aquí.

Dos diferencias que conviene tener presentes al comparar los dos proyectos:

- Allí el menú era un método `Mostrar()` con su **propio bucle**, que bloqueaba
  hasta elegir. Aquí es una `Pantalla` más: no bloquea, devuelve una
  `Transicion` y `Juego` decide. Eso permite volver atrás con `Escape`.
- `Music` conserva los nombres originales en inglés (`load`, `play`, `setLoop`),
  para que el código se reconozca junto al del otro proyecto. Por dentro sí está
  adaptada: SFML 3 renombró `setLoop()` a `setLooping()` y movió el estado a
  `sf::SoundSource::Status`.

## El Admin_Menu

Herramienta interna. La lógica está en `AdminMenu`, que no depende de SFML;
`PanelAdmin` es sólo su cara visible.

Se desbloquea tecleando `ADMIN` durante la partida; después `F1` abre y cierra el
panel.

Permite forzar cualquiera de los ocho estados, rellenar o vaciar las barras,
matar y revivir, activar inmortalidad, acelerar el reloj hasta x60 y ver un
diagnóstico en vivo. Sirve para depurar, pero sobre todo **para recorrer el
diagrama de estados en segundos** en vez de esperar a que las barras bajen solas.

## Guardado

`GestorGuardado` escribe un archivo de texto con pares `clave=valor`, fácil de
leer y de corregir a mano al depurar. Al cargar, `FabricaMascotas` reconstruye la
especie correcta a partir de los campos `especie` y `genero`.

Las partidas guardadas antes de que existiera el género no traen ese campo. Se
asume `Macho` en vez de rechazar el archivo: perder una partida por un campo que
no existía cuando se guardó sería peor que empezar con el sprite ajeno.

Para comprobar si el archivo existe se usa `std::filesystem::exists` y no
`ifstream::good()`: lo segundo responde "se pudo abrir", que no es lo mismo y
depende de permisos y bloqueos.

## Compilar

```bash
mingw32-make        # compila bin/game.exe
mingw32-make run    # compila y ejecuta desde la raiz del proyecto
mingw32-make clean  # borra los ejecutables
```

El juego busca `assets/` con rutas relativas, así que hay que lanzarlo desde la
raíz. Por eso el objetivo `run` hace `./bin/game.exe` y no entra en `bin/`.

Para probar rápido un cambio en la lógica sin recorrer el juego entero, el
`Admin_Menu` permite forzar cualquier estado y acelerar el reloj hasta x60.

## Tres trampas del entorno

Documentadas porque cuestan horas la primera vez:

1. **No compiles dentro de OneDrive ni Google Drive.** El `.exe` se reescribe en
   cada build y el cliente de sincronización se mete en medio: acabas ejecutando
   una copia vieja creyendo que tu cambio no hizo nada. Si el proyecto vive en
   una carpeta sincronizada, conviene moverla a una ruta local de verdad antes
   de compilar. Este proyecto vive en Documents por ese motivo.

2. **El `.exe` necesita las DLL de MinGW.** Sin ellas Windows falla con un
   mensaje que no explica gran cosa. Para entregarlo hay que copiar junto a
   `game.exe` las DLL de `C:\msys64\ucrt64\bin`: ademas de las cuatro de SFML
   hacen falta las del compilador (`libstdc++-6`, `libgcc_s_seh-1`,
   `libwinpthread-1`) y las que SFML arrastra (freetype, harfbuzz, zlib y
   companyia). `objdump -p bin/game.exe` las lista.

3. **SFML tiene que estar compilado con el mismo compilador.** Mezclar builds de
   MinGW distintos da errores de enlazado. Instalarlo por `pacman` en el mismo
   entorno lo evita.

## Cómo añadir cosas

**Una especie nueva:** hereda de `Mascota`, implementa los cuatro métodos
virtuales puros, y añádela a `TipoMascota` y a `FabricaMascotas::crear()`. Nada
más se toca. Si le pones hoja de sprites hacen falta **dos**, una por género:
`assets/images/<especie>_macho.txt` y `<especie>_hembra.txt`. Si no las hay, se
dibuja con figuras y el juego sigue funcionando.

**Un estado nuevo:** añádelo a `TipoEstado`, crea la clase que hereda de
`Estado`, regístrala en `MaquinaEstados::crear()` y decide desde qué estados se
llega. Actualiza también `diagrama_estados.puml`.

**Un objeto nuevo:** hereda de `Objeto`, implementa `aplicar()` y añádelo a
`Inventario::inicial()`.

**Una pantalla nueva:** hereda de `Pantalla`, añade su valor a
`Pantalla::Transicion` y engánchala en el `switch` de `Juego::cambiarPantalla()`.
Devuelve la transición desde `manejarEvento()` o `actualizar()`; el cambio lo
hace `Juego` al terminar el fotograma.
