# Distributed Processing

A distributed image processing system, operating via a console application, in C++. The premise of this application is to showcase the usage of multithreading, networking, and modern C++ standards. The application operates via the conjunction of two executables, `server` and `client`. The server is intended to be able to host multiple clients simultaneously.

The video submission can be found at `task/AP Assignment 2 Video.mp4`.

The original repository for this project can be found [here](https://github.com/Khthonian/DistributedProcessing).

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

The following packages were necessary to run the application during production:

- `fmt`
- `glew`
- `opencv`
- `vtk`

You can install these packages with the following command (specific to Arch Linux):

```bash
sudo pacman -S fmt glew opencv vtk
```

## Usage

### Valid Parameters

A total of 14 filters are included in this implementation:

- Resize filter
- Rotate filter
- Flip filter
- Colour adjustment filters:
  - Brightness filter
  - Contrast filter
  - Gamma filter
- Colour conversion filters[^1]:
  - BGR to RGB filter
  - BGR to HSV filter
  - BGR to greyscale filter
  - BGR to YCC filter
  - BGR to HSL filter
- Smoothing filters:
  - Gaussian blur filter
  - Box blur filter
  - Sharpening filter

The corresponding inputs are:

| Filters            | Operation Input | Parameter Input                                                                   |
| ------------------ | --------------- | --------------------------------------------------------------------------------- |
| Resize             | `resize`        | Value > 0 to scale.                                                               |
| Rotate             | `rotate`        | Value ideally between -360 and 360.                                               |
| Flip               | `flip`          | 0 to flip on x-axis, 1 to flip on y-axis, -1 to flip on both.                     |
| Brightness         | `brightness`    | Any value between 0 and 1 to decrease, any value over 1 to increase.              |
| Contrast           | `contrast`      | Any positive value to increase contrast, any negative value to decrease contrast. |
| Gamma              | `gamma`         | Any value greater than 1 to increase, any value between 0 and 1 to decrease.      |
| Colour conversions | `colour`        | `rgb`, `hsv`, `grey`, `ycc`, or `hsl` respectively.                               |
| Smoothing filters  | `smooth`        | `gauss`, `box`, or `sharp` respectively.                                          |

[^1]: OpenCV adheres to the historical BGR colour space standard so all implemented filters are built around that.

### Example Usage

Ideally, the application should be used in conjunction with a terminal multiplexer, e.g. tmux, to allow the user to visualise the terminals for both the server and client executables. As such, the following example will assume a terminal multiplexer is being used.

1. Start the server.

```bash
# Change into the bin directory
cd bin/

# Start server executable
./server
```

2. Execute the client.

```bash
# Change into the bin directory
cd bin/

# Execute the client executable with appropriate inputs
./client 127.0.0.1:12345 ../images/cat2.jpg colour grey
```

The original and modified images for this operation can be seen below:

![Original Image](../misc/exampleOriginal.png "The original image.")

![Modified Image](../misc/exampleModified.png "The modified image.")

The original and modified images will be displayed to the user for 10 seconds, or until the user presses a key. After the display windows are removed, the modified image will be saved in the location of the original image. If an error occurs at any point, a console warning will be output to the user detailing the nature of the error.

### Resetting the Images

The original images are included along with the images intended to be used by the application. To reset them, use the following commands:

1. Change into images directory.

```bash
cd images/
```

2. Copy original images to intended image location.

```bash
# Example using the cat2.jpg image
cp originalCat2.jpg cat2.jpg
```

## Disclaimer

While this application features preprocessing directives to allow operation on Windows, it was built and tested on Arch Linux, and was therefore designed with POSIX-compliant systems in mind. It is possible that this application might not work on Windows, in its current state.
