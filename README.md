# OSh

This is the O(h)Sh(it!) shell, a simple shell that can run commands and has three builtin commands.

![Build](https://img.shields.io/endpoint?url=https%3A%2F%2Fgithub.com%2Foslundstrom%2Fosh%2Factions%3Fquery%3Dworkflow%3Aci)

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
