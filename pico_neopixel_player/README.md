# Raspberry Pi Pico Template Project

Make sure you've got `PICO_SDK_PATH` in your environment set to a checkout of
`https://github.com/raspberrypi/pico-sdk.git`. (If you're using an editor, set
this before launching it so the CMake integration will work properly.)

Then:

1. Clone this template
2. If you like, find-and-replace "pico-project" in CMakeLists.txt with a project
   name of your choice
3. To build:

```
mkdir build
cd build
cmake ..
make
```

4. Flash to your Pico with `picotool load pico-project.bin` (change
   `pico-project` to your project's name if you renamed it)