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
| Gráfica | `Juego`, `Pantalla` y derivadas, `RenderMascota` y derivadas, `VistaMascota`, `HojaSprites`, `Animacion`, `Hud`, `Boton`, `BarraAtributo`, `PanelAdmin`, `Tema`, `GestorRecursos` | de la lógica y de SFML |

La flecha va siempre en un sentido. Si alguna clase de la primera columna
necesita incluir SFML, es señal de que está en la capa equivocada.

## Las jerarquías

El diagrama completo está en [`diagrama_clases.puml`](./diagrama_clases.puml)
(43 clases, 57 relaciones). Estas son las cinco que sostienen el diseño.

### Mascota — herencia y polimorfismo

```
Mascota (abstracta)
 ├── Gastly    asustar()
 ├── Perro
 ├── Gato      acicalarse()
 └── Dragon    escupirFuego()
```

Cada especie define sus tasas de desgaste, su sonido, su comida favorita y una
acción exclusiva. La acción exclusiva se resuelve con `dynamic_cast` al construir
la botonera.

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
 ├── PantallaSeleccion
 └── PantallaJuego
```

`Juego` sólo conoce la interfaz y cambia de pantalla cuando la activa se lo pide.
Añadir una tienda o unos créditos no obliga a tocar el bucle principal.

## Los atributos

`Atributo` es un valor acotado con una tasa de cambio por segundo. La mascota
tiene cinco, y todos siguen la convención **"más alto es mejor"** — por eso la
primera se llama *saciedad* y no *hambre*. Una sola regla mental, y las barras se
dibujan todas igual.

La salud es la excepción: no tiene tasa propia. La calcula
`Mascota::aplicarEfectosSecundarios()` a partir de las otras cuatro. Eso es lo
que convierte cinco números independientes en un sistema.

## La hoja de sprites

`HojaSprites` carga un archivo de texto que describe los recortes. Resuelve tres
problemas de una hoja dibujada a mano:

1. **La imagen no tiene canal alfa.** Al cargar se recorre y se vuelve
   transparente todo píxel cercano al color de fondo, con una tolerancia.
2. **Las poses no forman una rejilla exacta.** Cada recorte se declara uno a uno.
3. **Faltan poses para algún estado.** Se reutiliza otra animación con un tinte.

### Por qué se ve nítido

Tres cosas, y las tres importan:

- **Sin suavizado** (`setSmooth(false)`): ampliación por vecino más cercano.
- **Factor de aumento entero** (x4 en la partida, x2 en la vista previa). Con un
  factor fraccionario unos píxeles de origen ocupan 3 en pantalla y otros 4, y al
  animarse eso hormiguea.
- **La textura arranca en un píxel entero.** `RenderSprite::ajustarAPixel()`
  redondea la esquina del sprite, no su posición, para no perder el anclaje.

### Por qué el anclaje va sobre la cara

Los puntos `ox oy` de cada cuadro no son el centro del rectángulo: son el centro
de masa de los píxeles oscuros, es decir **la cara**. Si se usa el centro del
rectángulo, los cuadros que llevan algo al lado (la fruta al comer, la pelota al
jugar) quedan descentrados y la animación salta. Medidos así, `Jugando` saltaba
20 px entre cuadros y `Normal` 19 px.

Los valores **no se estimaron a mano**: se midieron calculando el centro de masa
de los píxeles oscuros de cada recorte. El resultado está en
[`hoja_animaciones.png`](./hoja_animaciones.png), que muestra las ocho
animaciones con una cruz en cada punto de anclaje. Ahí se ve que las caras caen
todas sobre la cruz, que es la condición para que la animación no tiemble.

Si se cambia la hoja de sprites hay que volver a medirlos.

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
especie correcta a partir del campo `especie`.

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
   `game.exe` las DLL de `C:msys64Crt64in`: ademas de las tres de SFML
   hacen falta las del compilador (`libstdc++-6`, `libgcc_s_seh-1`,
   `libwinpthread-1`) y las que SFML arrastra (freetype, harfbuzz, zlib y
   companyia). `objdump -p bin/game.exe` las lista.

3. **SFML tiene que estar compilado con el mismo compilador.** Mezclar builds de
   MinGW distintos da errores de enlazado. Instalarlo por `pacman` en el mismo
   entorno lo evita.

## Cómo añadir cosas

**Una especie nueva:** hereda de `Mascota`, implementa los cuatro métodos
virtuales puros, y añádela a `TipoMascota` y a `FabricaMascotas::crear()`. Nada
más se toca. Si le pones hoja de sprites, créala en
`assets/images/<especie>.txt`; si no, se dibuja con figuras.

**Un estado nuevo:** añádelo a `TipoEstado`, crea la clase que hereda de
`Estado`, regístrala en `MaquinaEstados::crear()` y decide desde qué estados se
llega. Actualiza también `diagrama_estados.puml`.

**Un objeto nuevo:** hereda de `Objeto`, implementa `aplicar()` y añádelo a
`Inventario::inicial()`.
