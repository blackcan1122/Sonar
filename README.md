<a id="readme-top"></a>

<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/blackcan1122/Sonar">
    <img src="images/GitHubHeader.jpg" alt="Sonar Logo" width="600" height="400">
  </a>

<h3 align="center">Sonar</h3>

  <p align="center">
    A C++ 2D Game Engine paired with a Pixel Art Submarine Cold War Simulator
    <br />
    <br />
    <a href="https://github.com/blackcan1122/Sonar/issues/new?labels=bug&template=bug-report---.md">Report Bug</a>
    ·
    <a href="https://github.com/blackcan1122/Sonar/issues/new?labels=enhancement&template=feature-request---.md">Request Feature</a>
  </p>
</div>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li><a href="#about-the-project">About The Project</a></li>
    <li><a href="#sonarengine">SonarEngine</a></li>
    <li><a href="#sonargame">SonarGame</a></li>
    <li><a href="#getting-started">Getting Started</a></li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#project-structure">Project Structure</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
  </ol>
</details>

## About The Project

Sonar is a C++ project combining a custom game engine with a Pixel Art Submarine Cold War Simulator. The project will eventually split into two repositories:  one for the engine and one for the game. 

Raylib is currently used for window management and rendering, with plans to replace it with a direct OpenGL/Vulkan backend in future versions.

## SonarEngine

A lightweight, modular game engine providing lifecycle management, resource handling, and event dispatching. 

### Class Hierarchy

```
IObject (Interface)
├── Object
│   ├── Entity (position, velocity, rotation, naval units)
│   │   └── BaseSubmarine / BaseShip
│   │       └── Player
│   ├── BaseUI
│   │   └── Button, TextBox
│   └── World (time-of-day, ambient noise)
```

### Core Systems

| System | Description |
|--------|-------------|
| **GameInstance** | Singleton managing window, resources, event dispatchers, and game loop |
| **GameMode** | Game state manager with `BeginPlay()` and `Update()` lifecycle |
| **Factory** | Object creation with auto-registration and `Initialize()` callback |
| **SoftObjectPath** | Lazy-loading references by name for better memory management |
| **EventDispatcher** | Decoupled event system (Key, Mouse, UI, SaveState, AllPurpose) |
| **ResourceManager** | Async texture loading with reference counting |
| **AssetRegistry** | Centralized asset tracking with unique naming |

### Object Lifecycle

```
Factory:: NewObject<T>()
  → Constructor
  → Register in AssetRegistry (unique name assigned)
  → Subscribe to Key/Mouse dispatchers
  → Initialize()
  → Tick() every frame
  → MarkForDestruction() → Cleanup
```

### Example:  Custom GameMode

```cpp
class MyGameMode : public GameMode
{
    AUTOBODY(MyGameMode)
public:
    virtual void BeginPlay() override
    {
        // Called once when GameMode starts
        m_Player = m_ObjectFactory->NewObject<Player>();
    }

    virtual void Update() override
    {
        GameMode::Update();  // Ticks all objects
    }

private:
    SoftObjectPath<Player> m_Player;
};
```

### Example: Custom Object with Initialize

```cpp
class MyEntity : public Entity
{
    AUTOBODY(MyEntity, Entity)
public:
    virtual void Initialize() override
    {
        // Called by Factory after registration
        // Safe to load resources, setup handlers
    }

    virtual void Tick(float DeltaTime) override { }
    virtual void OnKeyStroke(KeyboardKey Key, Vector2 MousePos) override { }
    virtual void OnMouseButtonPressed(MouseButton Key, Vector2 MousePos) override { }
};
```

## SonarGame

The game layer built on SonarEngine, featuring submarine simulation gameplay. 

### Game Classes

| Class | Inherits | Description |
|-------|----------|-------------|
| **Display** | BaseUI | Render target with resize/move handle support |
| **Waterfall** | Display | Sonar waterfall visualization with double-buffered rendering |
| **Map** | Display | Web Mercator projection map with zoom, pan, entity tracking |
| **Player** | BaseSubmarine | Player-controlled submarine |
| **PlayerUI** | - | HUD for player information |

### Display Features

The `Display` class provides UI panels with built-in interactivity:

```cpp
display->SetResizable(true);      // Enable corner drag-to-resize
display->SetMovable(true);        // Enable drag-to-move
display->SetMinSize(100, 100);    // Minimum dimensions
display->SetPosition({x, y});     // Set position
display->ResizeDisplay(w, h);     // Programmatic resize
```

### GameModes

- **MenuMode** - Main menu with buttons
- **SandboxGameMode** - Development/testing mode (Debug build)
- **OptionsMode** - Settings
- **ShutdownGame** - Cleanup and exit

### Current Features

- Waterfall sonar display (resizable)
- Interactive map with context menus
- Entity system with naval physics (knots, course, turning)
- Player submarine controls
- Multiple submarines on map

## Getting Started

### Prerequisites

All dependencies auto-download via Premake: 
- [Raylib](https://github.com/raysan5/raylib)
- [spdlog](https://github.com/gabime/spdlog)
- [nlohmann/json](https://github.com/nlohmann/json)

### Installation

```bash
git clone https://github.com/blackcan1122/Sonar.git
cd Sonar

# Visual Studio 2022
build-VisualStudio2022.bat

# MinGW-W64
build-MinGW-W64.bat
```

## Usage

```bash
./Sonar           # Starts at Menu
./Sonar -debug    # Sandbox GameMode
```

Default:  1280x720 @ 60 FPS

## Project Structure

```
Sonar/
├── src/
│   ├── main.cpp
│   ├── public/                 # Headers
│   │   ├── Base/               # Engine core
│   │   ├── Entities/           # Game entities
│   │   ├── Events/             # Event types
│   │   ├── GameModes/          # Game states
│   │   ├── Rendering/          # Render utilities
│   │   ├── StateMachines/      # State management
│   │   └── UI/                 # UI components
│   ├── private/                # Implementations
│   └── shaders/                # GLSL shaders
├── resources/                  # Assets & JSON configs
└── images/                     # Repo images
```

## Roadmap

### Engine
- [ ] OpenGL/Vulkan backend (replace Raylib)
- [ ] Audio system
- [ ] Physics system
- [ ] Dynamic GameMode loading via DLLs

### Game
- [ ] Sonar detection algorithms
- [ ] AI submarine behavior
- [ ] Campaign mode
- [ ] Multiplayer

See [open issues](https://github.com/blackcan1122/Sonar/issues) for full list.

## License

See `LICENSE` for information.

## Contact

**Blackcan1122** - [@blackcan1122](https://github.com/blackcan1122)

Project:  [https://github.com/blackcan1122/Sonar](https://github.com/blackcan1122/Sonar)

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- MARKDOWN LINKS -->
[contributors-shield]: https://img.shields.io/github/contributors/blackcan1122/Sonar.svg?style=for-the-badge
[contributors-url]: https://github.com/blackcan1122/Sonar/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/blackcan1122/Sonar.svg?style=for-the-badge
[forks-url]: https://github.com/blackcan1122/Sonar/network/members
[stars-shield]: https://img.shields.io/github/stars/blackcan1122/Sonar.svg? style=for-the-badge
[stars-url]: https://github.com/blackcan1122/Sonar/stargazers
[issues-shield]: https://img.shields.io/github/issues/blackcan1122/Sonar.svg?style=for-the-badge
[issues-url]: https://github.com/blackcan1122/Sonar/issues
