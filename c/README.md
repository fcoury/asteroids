# Asteroids

A small raylib project written in C.

## Prerequisites

- A C compiler
- CMake 3.24 or newer
- Git

## Build

Configure the project:

```sh
cmake -S . -B build
```

This downloads the pinned raylib 6.0 release into `build/_deps` and generates
`build/compile_commands.json` for clangd.

Build the executable:

```sh
cmake --build build
```

## Run

```sh
./build/asteroids
```

On multi-configuration generators such as Visual Studio, the executable may be
inside a configuration directory such as `build/Debug`.

## Editor support

The included `.clangd` file points clangd at the compilation database in
`build`. Configure the project once to resolve raylib headers and compile
flags in compatible editors.

Raylib is downloaded automatically during configuration; it is not vendored in
this repository. An internet connection is therefore required for the first
configuration.
