<a id="readme-top"></a>

<br />
<div align="center">
  <a href="https://github.com/blackcan1122/Sonar">
    <img src="images/GitHubHeader.jpg" alt="Sonar Logo" width="600" height="400">
  </a>

<h3 align="center">Sonar</h3>

  <p align="center">
    A C++ 2D Game Engine and Pixel Art Cold War Submarine Simulator
    <br />
    <br />
    <a href="https://github.com/blackcan1122/Sonar/issues/new? labels=bug&template=bug-report---. md">Report Bug</a>
    ·
    <a href="https://github.com/blackcan1122/Sonar/issues/new?labels=enhancement&template=feature-request---.md">Request Feature</a>
  </p>
</div>

<details>
  <summary>Table of Contents</summary>
  <ol>
    <li><a href="#about-the-project">About The Project</a></li>
    <li><a href="#sonarengine">SonarEngine</a></li>
    <li>
      <a href="#architecture-deep-dive">Architecture Deep Dive</a>
      <ul>
        <li><a href="#reflection-system">Reflection System</a></li>
        <li><a href="#event-system">Event System</a></li>
        <li><a href="#gamemode--automatic-object-management">GameMode & Automatic Object Management</a></li>
        <li><a href="#softobjectpath">SoftObjectPath</a></li>
        <li><a href="#ui-system">UI System</a></li>
      </ul>
    </li>
    <li><a href="#sonargame">SonarGame</a></li>
    <li><a href="#getting-started">Getting Started</a></li>
    <li><a href="#project-structure">Project Structure</a></li>
    <li><a href="#whats-next">What's Next</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
  </ol>
</details>

## About The Project

Sonar is two things in one repo:  a custom 2D game engine built from scratch in C++, and a Cold War submarine simulator built on top of it. Think of it as learning how the sausage gets made by actually making the sausage, then using that sausage to make a pretty decent sandwich.

The engine and game are currently living together, but the plan is to eventually split them into separate repositories once things mature a bit more.  For now, keeping them together makes iteration faster. 

## SonarEngine

The engine side handles all the boring-but-essential stuff so the game doesn't have to worry about it. 

### The Class Hierarchy

Everything inherits from `IObject` and builds up from there: 

```
IObject (Interface)
├── Object
│   ├── Entity (position, velocity, rotation, naval units)
│   │   └── BaseSubmarine / BaseShip
│   │       └── Player
│   ├── BaseUI
│   │   └── Button, TextBox, Display, ContextMenu
│   ├── LayoutManager
│   │   └── GridLayoutManager
│   └── World (time-of-day, ambient noise, signals)
```

### Core Systems Overview

| System | What It Does |
|--------|-------------|
| **GameInstance** | The singleton that runs the show - window, resources, event dispatchers, game loop |
| **GameMode** | The star of the show - automatically manages object lifecycle, ticking, rendering, and cleanup |
| **SoftObjectPath** | Safe object references by name - no manual pointer management needed |
| **SClass / Reflection** | Runtime type information with property exposure and class hierarchy traversal |
| **EventDispatcher** | Type-safe, decoupled event system with support for member function callbacks |
| **UI System** | Fluent API for buttons, text boxes, displays, context menus, and layout management |
| **ResourceManager** | Async texture loading with reference counting |
| **AssetRegistry** | Tracks all objects with unique naming |

## Architecture Deep Dive

### Reflection System

The engine has a custom reflection system built around `SClass`. Every class that uses the `AUTOBODY` or `ROOTBODY` macro gets registered in a global class registry with full type information.

**How it works:**

```cpp
// AUTOBODY macro generates: 
// - ThisClass typedef for the current class
// - Static SClass instance linked to parent's SClass
// - Super typedef for easy parent access
// - StaticClass() and GetStaticClass() for runtime type info

class Player : public BaseSubmarine
{
    AUTOBODY(Player, BaseSubmarine)
    // Now you have: 
    // - Player:: StaticClass() returns the SClass*
    // - player->GetStaticClass() returns the same
    // - Super is typedef'd to BaseSubmarine
};
```

**Property Exposure:**

You can expose member variables for runtime inspection:

```cpp
class Entity : public Object
{
    AUTOBODY(Entity, Object)

protected:
    Vector2 m_Position = {0, 0};
    float m_AccelerationRate = 2.f;

    // These register the properties with the reflection system
    EXPOSE_PROPERTY(Vector2, m_Position);
    EXPOSE_FLOAT(m_AccelerationRate);
};
```

Then at runtime:

```cpp
// Find a class by name
SClass* EntityClass = SClass::FindClass("Entity");

// Get all properties (including inherited ones)
std::vector<IProperty*> props = EntityClass->GetAllProperties();

// Get/set property values on an instance
float* accel = entity->GetStaticClass()->GetPropertyValue<float>("m_AccelerationRate", entity);
entity->GetStaticClass()->SetPropertyValue("m_AccelerationRate", entity, 5.0f);
```

**Class Hierarchy Traversal:**

```cpp
// Check inheritance at runtime
if (someObject->GetStaticClass() << Player::StaticClass())
{
    // someObject is a Player or derives from Player
}
```

### Event System

The event system is built around `EventDispatcher` and type-safe events. Events are dispatched based on their `SClass`, and listeners can be registered with identifiers for easy cleanup.

**Event Types:**

```cpp
// Base event interface
class IEvent { ROOTBODY(IEvent) };

// Concrete events carry data
class KeyEvent : public Event
{
public:
    KeyboardKey KeyPressed;
    Vector2 MousePos;
};

// Events with custom payloads
class AllPurposeEvent : public Event
{
    AUTOBODY(AllPurposeEvent, Event)
public:
    std::shared_ptr<IEventData> Payload = nullptr;
};

// Payload data classes
class DisplayResizeData : public EventData
{
    AUTOBODY(DisplayResizeData, EventData)
public:
    SoftObjectPath<Display> DisplayToResize;
    int NewWidth = 0;
    int NewHeight = 0;
    bool bIsResizeComplete = false;
};
```

**Registering Listeners:**

The dispatcher supports multiple ways to register callbacks:

```cpp
// Lambda
dispatcher.AddListener("MyListener", KeyEvent::StaticClass(),
    [](std::shared_ptr<IEvent> evt) {
        auto keyEvt = std::dynamic_pointer_cast<KeyEvent>(evt);
        // handle it
    });

// Member function (raw pointer)
dispatcher.AddListener("MapClickListener", AllPurposeEvent::StaticClass(),
    this, &SandboxGameMode::OnMapClickedEvent);

// Member function with SoftObjectPath (safe - checks if object still exists)
dispatcher.AddListener("SafeListener", AllPurposeEvent::StaticClass(),
    someObjectPath, &SomeClass::HandleEvent);
```

**Dispatching Events:**

```cpp
auto event = std::make_shared<KeyEvent>();
event->KeyPressed = KEY_SPACE;
event->MousePos = GetMousePosition();

// Broadcast to all listeners of this event type
dispatcher. Dispatch(event);

// Or dispatch to a specific listener by identifier
dispatcher.Dispatch(event, "SpecificListener", true);
```

**Global Dispatchers:**

The engine provides global dispatchers for common events:

```cpp
// These are available via GameInstance
GameInstance:: KeyDispatcher        // Keyboard input
GameInstance::MouseDispatcher      // Mouse input
GameInstance::UIEventDispatcher    // UI interactions
GameInstance::AllPurposeDispatcher // General purpose
```

### GameMode & Automatic Object Management

This is where the magic happens. `GameMode` is the central manager that makes everything "just work." You don't manually tick objects, manage their lifecycle, or worry about cleanup - the GameMode handles all of it.

**The Philosophy:**

Create objects with `NewObject<T>()` from your GameMode, and everything else is automatic: 
- Objects get registered in the AssetRegistry with unique names
- Objects are automatically subscribed to key/mouse events
- Objects tick every frame in the correct order (based on TickGroup)
- Objects marked for destruction are cleaned up at the end of the frame
- When the GameMode is destroyed, all its objects are cleaned up

**Creating Objects:**

```cpp
class SandboxGameMode : public GameMode
{
    AUTOBODY(SandboxGameMode, GameMode)

public:
    virtual void BeginPlay() override
    {
        // Just create objects - GameMode handles the rest
        PlayerOne = NewObject<Player>();
        PlayerOne. TryLoad()->SetEntityLocation({0, 0});
        PlayerOne.TryLoad()->SetDisplayName("U-521");

        MapDisplay = NewObject<Map>(400, 400);
        WaterfallDisplay = NewObject<Waterfall>(360, 300, 10);

        // Wire up events between objects
        if (auto map = MapDisplay. TryLoad())
        {
            map->MapEventDispatcher->AddListener(
                "Map Click Listener",
                AllPurposeEvent::StaticClass(),
                this, &SandboxGameMode::OnMapClickedEvent
            );
        }
    }

    // No need to override Update() unless you need custom logic
    // The base GameMode:: Update() ticks all registered objects automatically

private:
    SoftObjectPath<Player> PlayerOne;
    SoftObjectPath<Map> MapDisplay;
    SoftObjectPath<Waterfall> WaterfallDisplay;
};
```

**Tick Groups:**

Objects tick in a specific order based on their TickGroup:

```cpp
enum class ETickGroup
{
    DefaultTick,  // Normal game objects
    PostTick,     // After main tick
    Rendering,    // UI elements (BaseUI sets this by default)
    MAX           // Physics, etc.
};

// Set in constructor or wherever
m_TickGroup. SetTickGroup(ETickGroup:: Rendering);
```

**Object Lifecycle:**

```
NewObject<T>()
  → Constructor
  → Registered in GameMode's object maps
  → Registered in AssetRegistry (unique name generated)
  → Subscribed to Key/Mouse dispatchers
  → Initialize() called
  → Tick() called every frame (automatically!)
  → MarkForDestruction() when done
  → Cleaned up at end of frame (listeners removed, unregistered, deleted)
```

**Calling Parent Methods:**

The `AUTOBODY` macro defines `Super` as a typedef to the parent class:

```cpp
class MyGameMode : public GameMode
{
    AUTOBODY(MyGameMode, GameMode)

public:
    virtual void Update() override
    {
        // Do your stuff first
        HandleCustomLogic();

        // Then call parent (ticks all objects, handles cleanup)
        Super::Update();
    }
};
```

### SoftObjectPath

`SoftObjectPath<T>` is a safe way to reference objects without dealing with raw pointers or worrying about dangling references. It stores the object's registered name and resolves it through the AssetRegistry when needed.

**Why use it instead of pointers:**

- No dangling pointer risk - if the object is destroyed, `TryLoad()` returns nullptr
- Automatic type casting with `Cast<U>()`
- Works seamlessly with the event system
- Hashable - can be used as map keys
- Objects don't need to exist at declaration time

**Usage:**

```cpp
class MyGameMode : public GameMode
{
    // Declare references - objects don't exist yet
    SoftObjectPath<Player> m_Player;
    SoftObjectPath<Map> m_Map;

    void BeginPlay() override
    {
        // Create objects - now the paths are valid
        m_Player = NewObject<Player>();
        m_Map = NewObject<Map>(400, 400);
    }

    void DoSomething()
    {
        // Safe access - returns nullptr if object was destroyed
        if (auto player = m_Player.TryLoad())
        {
            player->SetCourse(180);
        }

        // Check validity without loading
        if (m_Map.IsValid())
        {
            // ... 
        }

        // Type casting
        SoftObjectPath<Entity> asEntity = m_Player.Cast<Entity>();
    }
};
```

**With Event Listeners:**

```cpp
// The dispatcher version that takes SoftObjectPath automatically
// checks if the object still exists before calling the callback
dispatcher.AddListener(
    "SafeCallback",
    SomeEvent::StaticClass(),
    mySoftObjectPath,  // SoftObjectPath<T>
    &T::HandleEvent    // Member function
);
```

### UI System

The engine includes a custom UI system built on top of `BaseUI`. It's still barebone but functional, with a fluent API design that makes building UI elements clean and readable.

#### BaseUI

All UI elements inherit from `BaseUI`, which automatically sets the tick group to `Rendering` so UI is processed after game logic: 

```cpp
class BaseUI : public Object
{
    AUTOBODY(BaseUI, Object)
public:
    BaseUI() { m_TickGroup.SetTickGroup(ETickGroup:: Rendering); }
};
```

#### Button

Buttons use a fluent builder pattern - chain method calls to configure everything in one go:

```cpp
// Create and configure a button with method chaining
auto startButton = NewObject<Button>();
startButton. TryLoad()
    ->Construct(100, 200, 200, 50, "Start Game", DARKGRAY)
    .SetEventDispatcher(UIDispatcher)
    .SetEventPayload("StartGame")
    .CenterText()
    .UpdateTextColor(WHITE)
    .UpdateFontSize(18)
    .OnHover([](Button* btn) {
        btn->UpdateColor(GRAY);
    })
    .OnHoverLeave([](Button* btn) {
        btn->UpdateColor(DARKGRAY);
    });

// Rounded buttons
button->Construct(x, y, width, height, "Click Me", BLUE, true, 0.3f);

// With textures and 9-patch support
button->SetTexture(myTexture)
      . SetNPatchInfo(patchInfo)
      .UseNPatchFeature(true)
      .UseTexture(true);
```

#### TextInputBox

Text boxes for displaying or editing text, also with a fluent API:

```cpp
auto textBox = NewObject<TextInputBox>();
textBox.TryLoad()
    ->Construct(50, 100, 300, 40, DARKGRAY)
    .SetInitialText("Enter your name")
    .UpdateFontSize(16)
    .UpdateFontColor(WHITE)
    . CanBeEdited(true)
    .UseWordWrap()
    .UseCenter();

// Get the text content
std::string input = textBox.TryLoad()->GetText();
```

#### Display

`Display` is a render target wrapper that provides resizable, movable panels.  It handles its own `RenderTexture2D` internally and emits events when resized or moved: 

```cpp
class Waterfall : public Display
{
    AUTOBODY(Waterfall, Display)
public:
    Waterfall(int Width, int Height, int TimeFrameInSec);

    virtual void Draw() override
    {
        // Draw to the internal render target
        BeginTextureMode(ActiveRenderTarget);
        ClearBackground(BLACK);
        // ... your rendering code
        EndTextureMode();
    }
};

// Configure display behavior
display->SetResizable(true);      // Enable corner drag-to-resize
display->SetMovable(true);        // Enable header drag-to-move
display->SetMinSize(100, 100);    // Minimum dimensions
display->SetPosition({x, y});

// Listen to resize/move events
display->OnResize. AddListener("ResizeHandler", AllPurposeEvent::StaticClass(),
    layoutManager, &GridLayoutManager::OnDisplayResize);

display->OnMove.AddListener("MoveHandler", AllPurposeEvent::StaticClass(),
    layoutManager, &GridLayoutManager::OnDisplayMove);
```

#### ContextMenu

Right-click context menus with customizable entries:

```cpp
auto contextMenu = NewObject<ContextMenu>();
contextMenu.TryLoad()->AddOnCloseCallback([this]() {
    // Called when menu closes
});

// Add entries
ContextMenuEntry entry;
entry.FontSize = 14;
entry.CloseOnClick = true;
entry.SetCallback([](ContextMenuEntry* self) {
    // Handle click
});
contextMenu. TryLoad()->AddMenuEntry(entry);

// Show at position
contextMenu.TryLoad()->OnConstruct(mousePosition);
```

#### Layout Managers

The `LayoutManager` base class defines an interface for automatic UI layout.  The `GridLayoutManager` implementation provides a grid-based system:

```cpp
// Create a 2x2 grid layout
auto layoutManager = NewObject<GridLayoutManager>(2, 2, windowWidth, windowHeight);

// Register displays with grid cells
layoutManager. TryLoad()->RegisterDisplay(mapDisplay, GridCell{0, 1, 1, 1});      // Row 0, Col 1
layoutManager.TryLoad()->RegisterDisplay(waterfallDisplay, GridCell{0, 0, 1, 1}); // Row 0, Col 0

// Cells can span multiple rows/columns
GridCell largeCell{0, 0, 2, 1}; // Spans 2 rows, 1 column

// Dynamic grid modification
layoutManager.TryLoad()->AddRow();
layoutManager.TryLoad()->AddColumn();
layoutManager.TryLoad()->RemoveRow();

// Query grid state
bool occupied = layoutManager.TryLoad()->IsCellOccupied(0, 1);
GridCell cellAtMouse = layoutManager. TryLoad()->GetCellAtPosition(mousePos);

// Snap displays to grid after dragging
layoutManager.TryLoad()->SnapToGrid(display, mousePos, true);

// Move displays between cells
layoutManager.TryLoad()->MoveDisplayToCell(display, newCell);
```

The grid layout manager automatically: 
- Responds to window resize events
- Handles display resize/move events and snaps to grid
- Prevents overlapping displays
- Supports dynamic row/column addition and removal (up to 6x6)
- Can spawn new displays via context menu

#### Putting It Together

Here's how UI elements work together in a real GameMode:

```cpp
void MenuMode::BeginPlay()
{
    UIDispatcher = std::make_shared<EventDispatcher>();

    // Create buttons with fluent API
    StartGame = NewObject<Button>();
    StartGame.TryLoad()
        ->Construct(Width/2 - 100, 200, 200, 50, "Start Game", DARKGRAY)
        .SetEventDispatcher(UIDispatcher)
        .SetEventPayload("SandboxGameMode")
        .CenterText();

    Option = NewObject<Button>();
    Option.TryLoad()
        ->Construct(Width/2 - 100, 270, 200, 50, "Options", DARKGRAY)
        .SetEventDispatcher(UIDispatcher)
        .SetEventPayload("OptionsMode")
        .CenterText();

    Exit = NewObject<Button>();
    Exit.TryLoad()
        ->Construct(Width/2 - 100, 340, 200, 50, "Exit", DARKGRAY)
        .SetEventDispatcher(UIDispatcher)
        .SetEventPayload("ShutdownGame")
        .CenterText();

    // Listen for button clicks
    UIDispatcher->AddListener("MenuButtonHandler", UIEvent::StaticClass(),
        [](std::shared_ptr<IEvent> evt) {
            auto uiEvt = std::dynamic_pointer_cast<UIEvent>(evt);
            // uiEvt->Payload contains the target GameMode name
            GameInstance:: GetInstance()->SwitchGameMode(uiEvt->Payload);
        });
}
```

## SonarGame

The game layer is where the submarine stuff lives. It uses SonarEngine's systems to create the actual gameplay.

### Game Classes

| Class | Inherits From | What It Does |
|-------|--------------|-------------|
| **Display** | BaseUI | Render target with built-in resize and move handles |
| **Waterfall** | Display | Sonar waterfall visualization with double-buffered rendering |
| **Map** | Display | Web Mercator projection map with zoom, pan, and entity tracking |
| **Player** | BaseSubmarine | The player's submarine |
| **PlayerUI** | BaseUI | HUD showing course, speed, depth |
| **GridLayoutManager** | LayoutManager | Handles display arrangement in a grid |
| **World** | Object | Time-of-day, ambient noise, signal propagation |

### GameModes in the Game

- **MenuMode** - Main menu with buttons
- **SandboxGameMode** - Development and testing playground (Debug builds only)
- **OptionsMode** - Settings menu
- **ShutdownGame** - Cleanup and exit

### What's Working Right Now

- Waterfall sonar display with real-time double-buffered rendering
- Interactive map with Web Mercator projection and context menus
- Entity system with naval physics (knots, course, turning rates)
- Grid-based layout system for arranging displays
- Resizable and movable display panels
- Player submarine with controls
- World system with time-of-day and signal propagation
- Full UI system with buttons, text boxes, and context menus

## Getting Started

### Prerequisites

Dependencies download automatically via Premake: 
- [Raylib](https://github.com/raysan5/raylib) (rendering backend - may change in the future)
- [spdlog](https://github.com/gabime/spdlog)
- [nlohmann/json](https://github.com/nlohmann/json)

### Building

```bash
git clone https://github.com/blackcan1122/Sonar.git
cd Sonar

# Visual Studio 2022
build-VisualStudio2022.bat

# MinGW-W64
build-MinGW-W64.bat
```

### Running

```bash
./Sonar           # Starts at the main menu
./Sonar -debug    # Jumps straight to Sandbox GameMode
```

Default window:  1280x720 at 60 FPS

## Project Structure

```
Sonar/
├── src/
│   ├── main.cpp
│   ├── public/                 # Headers
│   │   ├── Base/               # Engine core (GameInstance, Factory, SClass, etc.)
│   │   ├── Entities/           # Game entities (Player, submarines)
│   │   ├── Events/             # Event types (KeyEvent, MouseEvent, AllPurposeEvent)
│   │   ├── GameModes/          # Game states (Menu, Sandbox, Options)
│   │   ├── Rendering/          # Render utilities
│   │   ├── StateMachines/      # State management
│   │   └── UI/                 # UI components (Button, TextBox, Display, GridLayoutManager)
│   ├── private/                # Implementations
│   └── shaders/                # GLSL shaders
├── resources/                  # Assets and JSON configs
└── images/                     # Repo images
```

## What's Next

### Engine Side
- [ ] Audio system
- [ ] Physics system
- [ ] Dynamic GameMode loading via DLLs
- [ ] More UI components
- [ ] Rendering backend may change in the future

### Game Side
- [ ] Actual sonar detection algorithms
- [ ] AI submarine behavior
- [ ] Campaign mode
- [ ] Multiplayer (eventually)

Check the [open issues](https://github.com/blackcan1122/Sonar/issues) for the full list.

## What Works Now vs What's Coming

**Now:** You can run the game, see submarines on a map, watch a working sonar waterfall display, and move around.  The engine architecture is solid and the core systems are in place.  The reflection system, event dispatching, automatic object management, and UI system all work. 

**Soon:** Better rendering, more realistic sonar mechanics, AI opponents. 

**Later:** Campaign, multiplayer, and splitting the engine into its own repo.

This is a learning project that turned into something real. Things will change, APIs will break, but that's part of the process. 

## License

See `LICENSE` for details.

## Contact

**Blackcan1122** - [@blackcan1122](https://github.com/blackcan1122)

Project Link: [https://github.com/blackcan1122/Sonar](https://github.com/blackcan1122/Sonar)

<p align="right">(<a href="#readme-top">back to top</a>)</p>
