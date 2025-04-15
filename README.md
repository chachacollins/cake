# CAKE

A lightweight, easy-to-use build system for scaffolding projects.

> [!WARNING]
> This is not a production-ready application. Use at your own risk.

## Overview

CAKE is a simple build system that provides an intuitive syntax for defining build rules, dependencies, and variables to automate your build process.

## Syntax

### Variables

Variables allow you to define reusable values:

```
$variable_name <- "value"
```

### Rules

Rules define how to build targets:

```
target_name : dependency1 dependency2 => 
    command1,
    command2;
```

Or for rules without dependencies:

```
target_name :=> 
    command1,
    command2;
```

### Phony Rules

Phony rules (not representing actual files) are prefixed with `@`:

```
@clean :=>
    rm -rf bin;
```

## Examples

### Basic CAKEFILE

```
$cc <- "clang"
$compflags <- "-Wpedantic -Wall -Wextra -Werror -std=gnu11 -Iinclude"
$buildfiles <- "src/main.c src/utils.c src/lexer.c src/debug.c src/parser.c src/eval.c"

# Regular file target (not phony)
bin/cake : $buildfiles =>
    mkdir -p bin,
    $cc $buildfiles -o bin/cake $compflags;

# Debug Build
@debug :=> 
    mkdir -p bin,
    $cc -ggdb $buildfiles -o bin/cake $compflags;

# Release Build
@release :=> 
    mkdir -p bin,
    $cc -O3 $buildfiles -o bin/cake $compflags;

# Optimized for size
@small :=>
    mkdir -p bin,
    $cc -Os $buildfiles -o bin/cake $compflags;

# Clean Up
@clean :=> 
    rm -rf bin;
```

## Command Line Usage

```
cake [OPTIONS] TARGET

Options:
  -f FILENAME  Specify build file (default: CAKEFILE)
  -h           Display help message and exit

Arguments:
  TARGET       Name of the target to build
```

## Key Features

- **Variables**: Define once, use everywhere
- **Dependencies**: Specify what targets depend on
- **Phony Targets**: Mark targets that don't produce files
- **Dependency Checking**: Only rebuild what's necessary

## Project Status

CAKE is currently in development and may not have all the features of more established build systems.
