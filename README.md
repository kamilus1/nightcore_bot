# 🎵 NightcoreK Bot (Beta)

**⚠️ This is a beta version of the README. More detailed documentation will be added in the future.**

NightcoreK is a simple bot that transforms original audio files into nightcore-style music. It leverages the power of **GStreamer** for audio processing and is built using the **Meson** build system.

---

## 📋 To-Do List

This project is still in early development. Here are planned features and improvements:

- [ ] **Add machine learning-based nightcore creation**  
      Use ML models to automatically generate optimal pitch and tempo adjustments.
- [ ] **Add video visualizer for output music**  
      Generate accompanying visuals for the nightcore tracks to create shareable music videos.

---

## 🚀 Requirements

Before building and running the bot, make sure you have the following dependencies installed on your system:

- [GStreamer](https://gstreamer.freedesktop.org/)
- [Meson](https://mesonbuild.com/)

---

## 🛠️ Building the Project

To build the NightcoreK bot:

1. Clone this repository and navigate into the project directory.

2. Set up the build directory with Meson:

    ```bash
    meson build
    ```

3. Compile the project using:

    ```bash
    meson compile -C build
    ```

This will create the executable inside the `build` directory.

---

## ▶️ Running the Bot

After compiling, you can run the bot by using the following command to see the avaialble options. The details of each are described in --help mode. 

```bash
./build/nightcorek --help
