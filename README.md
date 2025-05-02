# VR Model Viewer
---
A Qt/VTK-based application for viewing 3D STL models in both desktop and VR environments.

## Features

- Load and view STL files in desktop mode
- View models in VR using SteamVR/OpenVR
- Tree view organization of loaded models
- Customize model properties:
  - Color (RGB values)
  - Visibility
  - Model name
- Real-time model manipulation in VR
- Dark theme UI with modern styling

## Building

1. Ensure all requirements are installed [here](##-requirements).
2. Clone the repository
3. Create a build directory:
```sh
mkdir build
cd build
```
4. Configure with CMake:
```sh
cmake ..
```
5. Build the project:
```sh
cmake --build .
```

## Usage >>>>>>>>>>>>>>>>EDIT THIS >>>>>>>>>>>>>>>>>

1. Launch the application
2. Use "File > Open File" to load STL models 
3. Select models in the tree view to:
   - Edit properties
   - Toggle visibility
   - Change colors
4. Click "Start VR" to enter VR mode (requires SteamVR to be running)
5. Use "Stop VR" to exit VR mode


## Project Structure

- `mainwindow.*` - Main application window implementation
- `ModelPart.*` - 3D model part handling
- `ModelPartList.*` - Tree structure for model organization
- `VRRenderThread.*` - VR rendering implementation
- `optiondialog.*` - Model properties dialog
- `style.qss` - Custom style sheet for dark mode

## Documentation
Doxygen documentation at 

## Requirements

- Qt 6+
- VTK (Visualization Toolkit)
- SteamVR / OpenVR
- CMake 3.16+
- C++17 compiler

---

## Team

- Ben Harle @Beannwing
- Kai Brewster @BrewskiIceTea
- Christian Ogatis @ChristianOgatis

## License

This project uses a mix of components:
- Qt/VTK components under their respective licenses
- Custom styling based on DevSec Studio's work (MIT License)
- Original code for the VR viewer implementation
