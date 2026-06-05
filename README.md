# Timbre - Automated Background Music

A lightweight, background audio player designed to make your programming sessions more immersive. This project plays background music that dynamically changes and reacts based on the code you write in real-time.

## Features
* **Reactive Audio:** The music and audio frequencies shift dynamically as you type and navigate through your code.
* **Lightweight Backend:** Written entirely in C for maximum performance and minimal memory footprint during your coding sessions.
* **Seamless Integration:** Uses a background observer to track editor activity without interrupting your workflow.

## Prerequisites

To compile and run this project, you will need:
* A C compiler (like `gcc` via MinGW on Windows, or standard `gcc` on Linux/macOS).
* **[SDL3](https://github.com/libsdl-org/SDL):** The Simple DirectMedia Layer library (version 3) is required for audio playback and thread management.

### Important Windows Requirement
If you are compiling and running this on Windows, you **must** have the `SDL3.dll` file placed in the same folder as your generated executable (`.exe`) and your source code before compiling. 

##  How to Build

1. Clone this repository to your local machine.
2. Ensure you have the SDL3 development libraries installed and `SDL3.dll` in your project folder (if on Windows).
3. Open your terminal in the project directory.
4. Compile the project. If you are using `gcc`, you can use a command similar to this:

(Note: You can also use the included Makefile if available by simply running the make command).

## VS Code Extension Setup
For the audio to react to your code, you must install the custom VS Code tracker included in this repository.
Locate the /timbre-observer-extension folder inside this repository.
Copy the entire folder and paste it into your local VS Code extensions directory:
Windows: Press Win + R, type %USERPROFILE%\.vscode\extensions and press Enter.
Linux / macOS: Open your terminal and navigate to ~/.vscode/extensions.
Completely close and restart Visual Studio Code.
The extension will automatically activate and start broadcasting your cursor data to the C audio engine!

## Usage
Run the compiled executable (music_player.exe on Windows or ./music_player on Linux/macOS).
Ensure you have allowed the application through your Firewall if prompted (it needs to listen on local port 9000).
Open VS Code, start typing, and enjoy the dynamic soundtrack of your code!

## Architecture overview
audio.c / audio.h: Manages the SDL3 audio stream, synthesizer calculations, and multithreading.
observe.c / observe.h: Manages background sockets to listen for IDE/Editor extensions sending real-time cursor line data.
main.c: The core loop that connects the observers to the audio engine.
timbre-observer-extension/: A lightweight Node.js VS Code extension that silently reports cursor movements to the local C server.

Happy Coding! 🎧💻
