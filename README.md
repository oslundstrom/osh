# OSh

This is the O(h)Sh(it!) shell, a simple shell that can run commands and has three builtin commands.

[![Status](https://github.com/oslundstrom/osh/actions/workflows/ci.yml/badge.svg)](https://github.com/oslundstrom/osh/actions/workflows/ci.yml)

## Getting started

Prerequisites for compiling the project is:

```
make
gcc
```

To compile the project, clone the repo

```
git clone <url>
```

Change directory to the repo and run `make`.

Start the shell by running the binary, `./osh`.

## Testing

The tests are run by triggering the `test` make-target:

```
make test
```

The tests uses the Unity framework, and when running the make-command the first time, the unity repo should be cloned into a directory called `unity`.
