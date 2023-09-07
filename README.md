# C3D Engine

## Overview

This repository serves as a foundation for my experimental game and software development projects. It includes the following components:

- 3D Math Implementation using SIMD
- Debug Drawing Utilities
- Custom Memory Allocator
- Basic Boilerplate for Experimental Projects

## Features

### 3D Math Implementation using SIMD

This project includes a highly optimized 3D math library that leverages SIMD (Single Instruction, Multiple Data) instructions to significantly improve performance in math-heavy operations common in game development and graphics programming. This library provides essential mathematical functions and data structures for tasks such as vector and matrix manipulation, transformations, and more.

### Debug Drawing Utilities

Debugging complex applications can be challenging. To ease the process, we've integrated a set of debug drawing utilities into this project. These utilities allow you to visualize important information, such as bounding boxes, collision shapes, or even custom debug information directly in your application's rendering context.

### Custom Memory Allocator

Efficient memory management is crucial for high-performance applications. This project includes a custom memory allocator designed to efficiently manage memory within the engine. It provides features like memory pooling, alignment control, and optimized allocation strategies, helping you reduce memory fragmentation and improve overall performance.

### Basic Boilerplate for Experimental Projects

Starting a new project from scratch can be time-consuming. This repository includes a basic boilerplate that provides a starting point for your experimental projects. It sets up essential components such as a rendering context, input handling, and project structure, allowing you to focus on the unique aspects of your project without reinventing the wheel.

## Getting Started

To get started with this project, follow these steps:

1. Clone the repository to your local machine.
2. Ensure you have the necessary dependencies installed (e.g., `GLFW`, `glad`, `cmake`, `mingw`, `cppvs`).
3. `cd build && cmake .. && make`

## Dependencies and Licensing

This project relies on the following third-party libraries:

- [stb_easy_font](https://github.com/nothings/stb) - License information for this library can be found in the respective repository.
- [GLFW](https://www.glfw.org/) - GLFW is distributed under the [zlib/libpng license](https://www.glfw.org/license.html).
- [glad](https://github.com/Dav1dde/glad) - License information for glad can be found in the `LICENSE` file within the `glad` directory.

Please review the licenses of these libraries before using this project in your own applications.

## Contributing

We welcome contributions to improve and expand this project. Feel free to submit issues, pull requests, or suggestions to help us enhance this foundation for game and software development.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
