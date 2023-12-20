# Distributed Processing

A distributed image processing system, operating via a console application, in C++. The premise of this application is to showcase the usage of multithreading, networking, and modern C++ standards. The application operates via the conjunction of two executables, `server` and `client`. The server is intended to be able to host multiple clients simultaneously.

While this application features preprocessing directives to allow operation on Windows, it was built and tested on Arch Linux, and was therefore designed with POSIX-compliant systems in mind. It is possible that this application might not work on Windows, in its current state.

## Setup

1. Clone the repository.

```bash
git clone git@github.com:Khthonian/DistributedProcessing.git
```

2. Make the build directory.

```bash
# Change into repository directory
cd DistributedProcessing/

# Make build directory
mkdir build
```

3. Generate build files.

```bash
# Change into build directory
cd build/

# Generate build files
cmake ..
```

4. Compile the executables.

```bash
make
```

The executables will be generated to a separate directory, `bin`.

### Necessary Packages

The following packages are necessary to run the application:

- `fmt`
- `glew`
- `opencv`
- `vtk`

You can install these packages with the following command (specific to Arch Linux):

```bash
sudo pacman -S fmt glew opencv vtk
```
