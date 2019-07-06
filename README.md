# Soundtouch

This is a copy of the soundtouch project (https://www.surina.net/soundtouch/) that I slightly modified and used in a course to introduce:

* how to profile code with google-perf

* how to identify performance bottlenecks

* how to parallelize code using OpenMP

## Course description:

* Introduction

* Why multicore architectures?

* Main challenges when migrating code to multicore architectures

* Multicore systems programmability

* OpenMP: intuitive API for programming multicore systems

* **Demo 1**: migration from sequential to parallel code

  * SoundTouch: parallelizing a sound processing library

* Heterogeneous multicore architectures

* Epiphany: a scalable NoC-based multicore accelerator

* **Demo 2**: running parallel code on different multicore architectures

  * Crypt: IDEA encryption and decryption algorithm

* Conclusions

NOTE:

**Demo 1** was executed on a Rasperry Pi 2 using the Soundtouch application.
The Soundtouch application executed on that system is present in this git repository.

**Demo 2** was executed on a [Parallella board](https://github.com/parallella) using the IDEA encryption/decription application.
The IDEA application executed on that system is currently present in [another repo](https://github.com/frobino/openmp_introduction/tree/parallella-dev) .

## Branches description

### remotes/origin/master: 

Master of the original Soundtouch repo.

### remotes/origin/openmp-parallella

Attempt to parallelize for [Parallella board](https://github.com/parallella). This was never fully successful.

### remotes/origin/openmp-rpi

TBD

### remotes/origin/openmp-rpi-demo

TBD

### remotes/origin/openmp-rpi2-demo

The optimized OpenMP implementation showed during the Demo 1.

### remotes/origin/plain

Profiler + all OpenMP pragmas removed from code. This is the pure classical sequential version.

### remotes/origin/profile:

Simple commit addig google-perf profiler start/stop calls around the main loop.
This commit is often included in other branches.
