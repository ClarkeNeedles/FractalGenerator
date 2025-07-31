![MIT License](https://img.shields.io/badge/License-MIT-brightgreen)
![C++](https://img.shields.io/badge/Language-C++-blue)

# **Fractal Generator**

An application to generate and explore different types of fractals with CPP, SSE, AVX, and multithreading.

---

## **Table of Contents**

- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
     - [Recording Examples](#recording-examples)
- [How It Works](#how-it-works)
- [License](#license)
- [Contact](#contact)

---

## **Introduction**

> `Fractal Generator` is an app that will generate various mathematical fractals.
> Used to explore fractals and the complexity behind them.
> Timing the generation of different fractals using different size registers.
> Uses WinAPI, CPP, SIMD (SSE and AVX), Multithreading

---

## **Features**

- 🚀 Fast and optimized performance.
- ⏳ Algorithm timing.
- 📹 Record fractal exploration for cool gifs.

---

## **Installation**

### Option 1: Run the Executable (For End Users)
If you simply want to use the application without diving into the source code:

1. **Download the Executable**
   - Go to the [Releases](https://github.com/ClarkeNeedles/FractalGenerator/tree/main/x64/Release) section of this repository.
   - Download the latest version of `FractalGenerator.exe`.

2. **Run the Application**
   - Double-click the downloaded `FractalGenerator.exe` file to launch the application.
   - Follow the on-screen prompts or controls to generate your fractals.

> *Note:* The application requires **no additional setup** unless otherwise stated. Make sure your system meets any runtime requirements (see below).

---

### Option 2: Build and Run from Source Code (For Developers)
If you'd like to modify the application or explore the source code:

1. **Prerequisites**
   - Ensure you have the following tools installed:
     - [Visual Studio](https://visualstudio.microsoft.com/) 2019 or later (Windows) with the following components:
       - **.NET Desktop Development Workload** (if applicable).
       - Other required libraries or SDKs (specific details listed below).
     - [Git](https://git-scm.com/) (to clone the repository).

2. **Clone the Repository**
   Open a terminal or Git Bash and clone the repository:
   ```bash
   git clone https://github.com/yourusername/fractal-generator.git
   cd fractal-generator

3. **Open Solution in Visual Studio**
   - Locate the FractalGenerator.sln file in the cloned repository and open it.

4. **Build and Run**
   - Select the desired build configuration (Debug or Release)
   - Press the run button
   - Customize the code as necessary
  
## **Usage**

Here's how to use the Fractal Generator application:

### Step 1: Launch the Application
After running `FractalGenerator.exe`, you'll see the application interface.

### Step 2: Generate Your Fractals
   - Customize fractal parameters (e.g., language, fractal, gradient) and hit "Render" -> "Generate" to render your fractal.
   - The number at the bottom left is the time it took to generate the given fractal.
   - As you move down the list of languages, the generation time will become shorter and shorter.

### Step 3: Explore Your Fractal
   - Left mouse button: move the fractal around
   - Scroll in/out: zooming in and out of the generated fractal

### Recording
   - If you hit "Render" -> "Start Recording" you will start recording.
   - As long as you do not hit end recording, each time an image is generated whether by clicking "Generate", or using your mouse, it will be added to the output gif.

### Recording Examples
![Alt Text](renders/render1.gif)
![Alt Text](renders/render2.gif)
![Alt Text](renders/render3.gif)
![Alt Text](renders/render4.gif)
![Alt Text](renders/render5.gif)

## **How It Works**

### Languages
    - You have an option to generate any fractal with a range of languages.
	- CPP, SSE, AVX, and multithreading option with each language. 
    - SSE and AVX are single instruction multiple data languages (SIMD).
	- This means that they have larger registers and can process more data at the same time in parallel.
	- CPP uses the generic 64-bit registers, SSE uses 128-bit registers, and AVX uses 256-bit registers.
	- Since the size of registers for each language doubles, in theory, generation time should half.
    - Multithreading then uses the cores available in your CPU to further optimize generation time.

### Fractals
    - Each fractal is a mathematical equation that deals with the real and complex plane.
    - If you simulate the amount of iterations for a given point in the function, then map each iteration to a colour, you get a uniform result to a very random function.

#### Types of Fractals
    - [Mandelbrot](https://paulbourke.net/fractals/mandelbrot/)
    - [Burning Ship](https://paulbourke.net/fractals/burnship/)
    - [Multibrot (Order 5)](https://paulbourke.net/fractals/multimandel/)
    - [Nova](https://paulbourke.net/fractals/nova/)
    - [Pheonix](https://en.wikipedia.org/wiki/Julia_set)

### Gradient
    - All of the gradients are just random ways of messing with the rgb values based on the iterations.

## **License**

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## **Contact**

- **Author**: [Clarke Needles](https://your-portfolio-link.com)  
- 📧 Email: [c.w.needles@gmail.com](mailto:c.w.needles@gmail.com)  
- 🌐 Website: [yourwebsite.com](https://yourwebsite.com) 
