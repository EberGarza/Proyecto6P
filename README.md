# VirtualPet

![Pantalla de juego](./assets/images/screenshots/gameplay.png)

## Descripción

**VirtualPet** es una mascota virtual al estilo Tamagotchi / Pou, desarrollada
en C++17 orientado a objetos con la biblioteca SFML. La mascota tiene cinco
necesidades que se van agotando con el tiempo y ocho estados por los que va
pasando según cómo la cuides: si la dejas sin comer se pone hambrienta, si no
duerme se agota, y si la descuidas lo suficiente enferma y muere.

El proyecto está pensado alrededor de una idea: **la lógica del juego no conoce
SFML**. Eso permite compilarla y probarla por separado, y es lo que sostiene
toda la estructura de clases.

## Características Principales

- **Ocho estados con transiciones por prioridad**: cada estado es una clase
  propia y decide él mismo a dónde puede pasar, sin un `if` gigante que crezca
  con el proyecto.
- **Cuatro especies**: Gastly, Perro, Gato y Dragón, cada una con sus propias
  tasas de desgaste, su sonido y una acción exclusiva.
- **Efectos cruzados entre necesidades**: la salud no baja sola, baja cuando
  descuidas la comida, la higiene o el ánimo. Y sube si cuidas bien.
- **Animación por hoja de sprites** descrita en un archivo de texto que se
  puede ajustar sin recompilar.
- **Dibujo de respaldo**: las especies sin arte se dibujan con figuras
  geométricas, así que el juego siempre se ve.
- **Guardado automático** al salir, en un archivo de texto legible.
- **Admin_Menu**: menú de desarrollo para forzar cualquier estado y depurar.

## Requisitos del Sistema

- Sistema Operativo: Windows 10/11 (64 bits)
- Memoria RAM: 512 MB
- Espacio en disco: 20 MB
- Dependencias: SFML 3.0 (incluida en el paquete de entrega)

## Instalación

### Para Usuarios

1. Descarga la carpeta de entrega y descomprímela donde prefieras.
2. Ejecuta `game.exe`.

La carpeta ya trae las DLL necesarias, así que no hace falta instalar nada.

### Para Desarrolladores

1. Clona este repositorio:

```bash
git clone https://github.com/EberGarza/Proyecto6P.git
```

2. Instala MSYS2 y, desde la consola **MSYS2 UCRT64**, las dependencias:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-sfml
```

3. Añade `C:\msys64\ucrt64\bin` al `PATH` de Windows.

4. Compila y ejecuta:

```bash
cd Proyecto6P
mingw32-make run
```

> **Importante:** el juego busca la carpeta `assets/` con rutas relativas, así
> que hay que lanzarlo desde la raíz del proyecto.

## Controles

- **Ratón**:
  - Clic izquierdo: pulsar los botones de acción
- **Teclado**:
  - `1` a `6`: alimentar, jugar, asear, medicar, dormir, acariciar
  - `Enter`: confirmar en la pantalla de selección
  - `Escape`: cerrar el panel de desarrollo, o salir del juego guardando
  - `F1`: abrir y cerrar el Admin_Menu, una vez desbloqueado

## Estructura del Proyecto

```
Proyecto6P/
│
├── assets/              # Recursos del juego
│   ├── fonts/           # Fuentes tipográficas
│   ├── images/          # Hoja de sprites y su archivo de recortes
│   │   └── screenshots/ # Capturas para la documentación
│   └── sound/           # Música y efectos sonoros
│
├── bin/                 # Archivos ejecutables compilados
│
├── docs/                # Documentación
│   ├── Manual_Usuario.md      # Manual del usuario
│   ├── Manual_Programador.md  # Manual del programador
│   ├── diagrama_clases.puml   # Diagrama UML de la arquitectura
│   ├── diagrama_estados.puml  # Diagrama de estados de la mascota
│   └── hoja_animaciones.png   # Revisión visual de la hoja de sprites
│
├── include/             # Archivos de cabecera (.hpp)
│
├── src/                 # Código fuente (.cpp)
│
├── LICENSE
└── Makefile             # Script de compilación
```

### Las dos capas

Aunque `include/` y `src/` son planos, el código está pensado en dos capas y
la separación se respeta con disciplina: **las clases de lógica no incluyen
SFML en ningún archivo**. Sólo la capa gráfica habla con la biblioteca, y la
dependencia va siempre en un sentido.

| Capa | Clases | Depende de |
|------|--------|------------|
| Lógica | `Mascota` y las especies, `Atributo`, `Estado`, `MaquinaEstados`, `TipoEstado`, `Objeto` y derivadas, `Inventario`, `FabricaMascotas`, `GestorGuardado`, `AdminMenu`, `Utilidades` | nada externo |
| Gráfica | `Juego`, `Pantalla` y derivadas, `RenderMascota` y derivadas, `VistaMascota`, `HojaSprites`, `Animacion`, `Hud`, `Boton`, `BarraAtributo`, `PanelAdmin`, `Tema`, `GestorRecursos` | de la lógica y de SFML |

Si una clase de la primera columna necesitara incluir SFML, sería señal de que
está en la capa equivocada.

## Documentación

- [Manual del Usuario](./docs/Manual_Usuario.md) - Cómo jugar y cuidar a la mascota
- [Manual del Programador](./docs/Manual_Programador.md) - Arquitectura y decisiones de diseño
- [Diagrama de Clases](./docs/diagrama_clases.puml) - Las 43 clases y sus relaciones
- [Diagrama de Estados](./docs/diagrama_estados.puml) - Los ocho estados y sus transiciones

## Desarrollo

### Compilación

El proyecto utiliza un Makefile para simplificar la compilación:

```bash
# Compilar el proyecto
mingw32-make

# Compilar y ejecutar
mingw32-make run

# Limpiar archivos compilados
mingw32-make clean
```

> Para entregar el ejecutable hay que copiar junto a `game.exe` las DLL de
> MinGW y SFML que están en `C:\msys64\ucrt64\bin`. Sin ellas Windows no lo
> arranca y el mensaje de error no explica gran cosa.

### Tecnologías utilizadas

- **Lenguaje**: C++17
- **Gráficos**: SFML 3.0
- **Compilador**: MinGW-w64 GCC (entorno UCRT64)
- **Construcción**: Make
- **Documentación**: Markdown, PlantUML

## Capturas de pantalla

![Selección de mascota](./assets/images/screenshots/menu.png)
![Jugando](./assets/images/screenshots/jugando.png)
![Admin_Menu](./assets/images/screenshots/admin_menu.png)

## Colaboradores

- [EberOmarGarzaFuentes] - Desarrollo principal

## Licencia

© 2026 [Inei-Zone] - Todos los derechos reservados

El sprite de Gastly es fan art de un personaje propiedad de Nintendo, Game
Freak y The Pokémon Company, usado aquí sólo con fines educativos. Ver
[LICENSE](./LICENSE).

## Agradecimientos

- A la biblioteca SFML y sus desarrolladores
- A la universidad y profesores que guiaron este proyecto

---

> "Una mascota virtual es una excusa muy buena para practicar herencia,
> polimorfismo y máquinas de estados."
