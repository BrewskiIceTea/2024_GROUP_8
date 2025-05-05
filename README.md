# VR Model Viewer

![Application_GUI](https://github.com/user-attachments/assets/0edff21c-2a36-4d0e-a044-bc55f6ca0583)

A Qt/VTK-based application for viewing 3D STL models in both desktop and VR environments.


## Documentation
Full online documentation is available [here.](https://brewskiicetea.github.io/2024_GROUP_8/index.html) (generated with Doxygen).
The documentation includes:
- Detailed API reference for all public classes and functions
- Class hierarchies and diagrams
- Descriptions of key modules and their responsibilities


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

## How to use

1. Launch the application
2. Use "Toolbar" > "Open" (Load folder as models) > Select folder containing all .STL files.
3. Select models in the tree view to:
   - Edit properties
   - Toggle visibility
   - Change colors
4. Click "Start VR" to enter VR mode (Will start SteamVR if not running)
5. Use "Stop VR" to exit VR mode


## Project Structure

- `mainwindow.*` - Main application window implementation
- `ModelPart.*` - 3D model part handling
- `ModelPartList.*` - Tree structure for model organization
- `VRRenderThread.*` - VR rendering implementation
- `optiondialog.*` - Model properties dialog
- `style.qss` - Custom style sheet for dark mode



## Requirements

- Qt 6+
- VTK (Visualization Toolkit)
- SteamVR / OpenVR
- CMake 3.16+
- C++17 compiler

---

## Contributors

- Ben Harle - [@Beannwing](https://github.com/Beannwing) | eeybh1@exmail.nottingham.ac.uk
- Kai Brewster - [@BrewskiIceTea](https://github.com/BrewskiIceTea) | eeykb3@nottingham.ac.uk
- Christian Ogatis - [@ChristianOgatis](https://github.com/BrewskiIceTea) | eeyco5@nottingham.ac.uk



## License

This project includes:
- Qt/VTK components under their respective licenses
- Custom styling based on DevSec Studio's work (MIT License)
- Original code for the VR viewer implementation
