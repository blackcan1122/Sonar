#pragma once
#include "Base/Core.h"
#include "Base/EventDispatcher.hpp"
#include "Events/WindowResizeData.hpp"
#include "Base/Event.hpp"
#include "UI/Display.hpp"

#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <string>

/**
 * @enum DisplayType
 * @brief Types of displays that can be spawned via the grid layout manager
 */
enum class DisplayType
{
	Map,
	Waterfall
};

/**
 * @struct DisplaySpawnInfo
 * @brief Contains information about what type of display to spawn and its configuration
 */
struct DisplaySpawnInfo
{
	DisplayType type = DisplayType::Waterfall;
	int waterfallTimeframeSecs = 60;  // Only used for Waterfall type
	
	// Helper constructors
	static DisplaySpawnInfo CreateMap() { return {DisplayType::Map, 0}; }
	static DisplaySpawnInfo CreateWaterfall(int timeframeSecs) { return {DisplayType::Waterfall, timeframeSecs}; }
};

/**
 * @class GridLayoutManager
 * @brief Manages a grid-based layout system for Display objects.
 * 
 * Features:
 * - Dynamic rows/columns with minimum tile size of 250x250
 * - Snap-to-grid behavior during move/resize operations
 * - Automatic tile swapping when displays overlap
 * - Multi-tile spanning support (e.g., 2x2 displays)
 * - Responsive to window resize events
 * 
 * Usage:
 * 1. Create GridLayoutManager with initial rows/columns
 * 2. Register displays with RegisterDisplay()
 * 3. Call UpdateLayout() after window resize or display changes
 * 4. Use SnapToGrid() during drag operations
 */
class GridLayoutManager
{
public:
	static constexpr int MIN_TILE_SIZE = 250;
	
	/**
	 * @brief Constructs a GridLayoutManager
	 * @param rows Initial number of rows
	 * @param columns Initial number of columns
	 * @param windowWidth Current window width
	 * @param windowHeight Current window height
	 */
	GridLayoutManager(int rows, int columns, int windowWidth, int windowHeight);
	~GridLayoutManager();
	
	/**
	 * @brief Registers a Display to be managed by this grid
	 * @param display The Display to manage
	 * @param cell The initial grid cell assignment
	 * @return true if registration successful
	 */
	bool RegisterDisplay(Display* display, const GridCell& cell);
	
	/**
	 * @brief Unregisters a Display from the grid
	 * @param display The Display to remove
	 */
	void UnregisterDisplay(Display* display);
	
	/**
	 * @brief Updates all display positions and sizes based on current grid state
	 */
	void UpdateLayout();
	
	/**
	 * @brief Called when window is resized - recalculates grid and updates displays
	 * @param newWidth New window width
	 * @param newHeight New window height
	 */
	void OnWindowResize(int newWidth, int newHeight);
	
	/**
	 * @brief Calculates which grid cell a screen position falls into
	 * @param screenPos Position in screen coordinates
	 * @return GridCell with row and column set (span = 1)
	 */
	GridCell GetCellAtPosition(Vector2 screenPos) const;
	
	/**
	 * @brief Snaps a display to the nearest grid cell(s)
	 * @param display The display being dragged
	 * @param screenPos Current mouse/drag position
	 * @param maintainSpan If true, maintains the display's current span
	 * @return The target GridCell for snapping
	 */
	GridCell SnapToGrid(Display* display, Vector2 screenPos, bool maintainSpan = true);
	
	/**
	 * @brief Moves a display to a new cell, handling swaps if necessary
	 * @param display The display to move
	 * @param targetCell The target cell
	 * @return true if move was successful
	 */
	bool MoveDisplayToCell(Display* display, const GridCell& targetCell);
	
	/**
	 * @brief Attempts to resize a display to span more/fewer cells
	 * @param display The display to resize
	 * @param newRowSpan New row span
	 * @param newColSpan New column span
	 * @return true if resize was successful
	 */
	bool ResizeDisplaySpan(Display* display, int newRowSpan, int newColSpan);
	
	/**
	 * @brief Adds a row to the grid if minimum tile size allows
	 * @return true if row was added
	 */
	bool AddRow();
	
	/**
	 * @brief Adds a column to the grid if minimum tile size allows
	 * @return true if column was added
	 */
	bool AddColumn();
	
	/**
	 * @brief Removes a row if it doesn't contain any displays
	 * @return true if row was removed
	 */
	bool RemoveRow();
	
	/**
	 * @brief Removes a column if it doesn't contain any displays
	 * @return true if column was removed
	 */
	bool RemoveColumn();
	
	/**
	 * @brief Gets the maximum number of rows possible given current window size
	 */
	int GetMaxRows() const;
	
	/**
	 * @brief Gets the maximum number of columns possible given current window size
	 */
	int GetMaxColumns() const;
	
	// Getters
	int GetRows() const { return m_Rows; }
	int GetColumns() const { return m_Columns; }
	int GetCellWidth() const { return m_CellWidth; }
	int GetCellHeight() const { return m_CellHeight; }
	int GetWindowWidth() const { return m_WindowWidth; }
	int GetWindowHeight() const { return m_WindowHeight; }
	
	/**
	 * @brief Gets the cell assignment for a display
	 * @param display The display to query
	 * @return Pointer to GridCell, or nullptr if not registered
	 */
	const GridCell* GetDisplayCell(Display* display) const;
	
	/**
	 * @brief Gets the display at a specific cell, if any
	 * @param row Row index
	 * @param col Column index
	 * @return Pointer to Display, or nullptr if cell is empty
	 */
	Display* GetDisplayAtCell(int row, int col) const;
	
	/**
	 * @brief Checks if a cell is occupied (directly or by a spanning display)
	 * @param row Row index
	 * @param col Column index
	 * @return true if cell is occupied
	 */
	bool IsCellOccupied(int row, int col) const;
	
	/**
	 * @brief Calculates the screen rectangle for a given grid cell configuration
	 * @param cell The cell configuration
	 * @return Rectangle in screen coordinates
	 */
	Rectangle GetCellRect(const GridCell& cell) const;
	
	/**
	 * @brief Draws debug overlay showing grid lines (call in debug mode)
	 */
	void DrawDebugGrid() const;
	
	/**
	 * @brief Draws placeholder graphics on empty (unoccupied) tiles
	 * Call this BEFORE drawing displays so they render underneath
	 */
	void DrawEmptyTiles() const;
	
	/**
	 * @brief Sets whether the grid is enabled (displays use grid positioning)
	 */
	void SetEnabled(bool enabled) { m_IsEnabled = enabled; }
	bool IsEnabled() const { return m_IsEnabled; }
	
	/**
	 * @brief Draws interactive control buttons for adding/removing rows and columns
	 * Call this in Update() after drawing displays
	 */
	void DrawGridControls();
	
	/**
	 * @brief Draws snap preview overlays for all displays that are currently being moved/resized
	 * Call this AFTER all displays have been rendered to ensure it appears on top
	 */
	void DrawSnapPreviews() const;
	
	/**
	 * @brief Registers a display as currently being dragged (for preview rendering)
	 */
	void SetDisplayDragging(Display* display, bool isDragging, const GridCell& previewCell);
	
	/**
	 * @brief Registers a display as currently being resized (for preview rendering)
	 * @param display The display being resized
	 * @param isResizing Whether resizing is active
	 * @param previewCell The cell configuration showing the preview span
	 */
	void SetDisplayResizing(Display* display, bool isResizing, const GridCell& previewCell);
	
	/**
	 * @brief Show or hide the grid control buttons
	 */
	void SetShowControls(bool show) { m_ShowControls = show; }
	bool IsShowingControls() const { return m_ShowControls; }
	
	// Callback types for display management
	using DeleteDisplayCallback = std::function<void(Display*)>;
	using CreateDisplayCallback = std::function<void(const GridCell&, const DisplaySpawnInfo&)>;
	
	/**
	 * @brief Sets callback for when a display delete is requested
	 * @param callback Function to call with the display to delete
	 */
	void SetDeleteDisplayCallback(DeleteDisplayCallback callback) { m_OnDeleteDisplay = callback; }
	
	/**
	 * @brief Sets callback for when a new display should be created
	 * @param callback Function to call with the target cell and spawn info for the new display
	 */
	void SetCreateDisplayCallback(CreateDisplayCallback callback) { m_OnCreateDisplay = callback; }
	
	/**
	 * @brief Enable/disable the add display buttons on empty tiles
	 */
	void SetShowAddButtons(bool show) { m_ShowAddButtons = show; }
	bool IsShowingAddButtons() const { return m_ShowAddButtons; }
	
	/**
	 * @brief Enable/disable the delete buttons on displays
	 */
	void SetShowDeleteButtons(bool show) { m_ShowDeleteButtons = show; }
	bool IsShowingDeleteButtons() const { return m_ShowDeleteButtons; }
	
	/**
	 * @brief Gets all registered displays
	 */
	const std::unordered_map<Display*, GridCell>& GetAllDisplays() const { return m_DisplayCells; }

private:
	/**
	 * @brief Recalculates cell dimensions based on current window size and row/column count
	 */
	void RecalculateCellDimensions();
	
	/**
	 * @brief Applies the calculated position and size to a display
	 * @param display The display to update
	 * @param cell The cell configuration
	 */
	void ApplyLayoutToDisplay(Display* display, const GridCell& cell);
	
	/**
	 * @brief Checks if a cell configuration would cause overlap (ignoring a specific display)
	 * @param cell The cell to check
	 * @param ignoreDisplay Display to ignore in overlap check (usually the one being moved)
	 * @return true if there would be an overlap
	 */
	bool WouldOverlap(const GridCell& cell, Display* ignoreDisplay) const;
	
	/**
	 * @brief Finds the display that occupies a given cell (accounts for spanning)
	 * @param row Row index
	 * @param col Column index
	 * @return The display occupying this cell, or nullptr
	 */
	Display* FindDisplayOccupyingCell(int row, int col) const;
	
	/**
	 * @brief Swaps the positions of two displays
	 */
	void SwapDisplays(Display* displayA, Display* displayB);
	
	/**
	 * @brief Resolves any overlapping displays after window resize
	 * Tries to find empty cells for overlapping displays
	 */
	void ResolveOverlaps();
	
	/**
	 * @brief Finds the first empty cell that can fit a display with given span
	 * @param rowSpan Required row span
	 * @param colSpan Required column span
	 * @param outCell Output cell if found
	 * @return true if an empty cell was found
	 */
	bool FindEmptyCell(int rowSpan, int colSpan, GridCell& outCell) const;
	
	int m_Rows;
	int m_Columns;
	int m_WindowWidth;
	int m_WindowHeight;
	int m_CellWidth;
	int m_CellHeight;
	bool m_IsEnabled = true;
	bool m_ShowControls = true;
	
	// Control button dimensions
	static constexpr int CONTROL_BUTTON_SIZE = 30;
	static constexpr int CONTROL_BUTTON_MARGIN = 5;
	
	// Tile padding (spacing between tiles)
	static constexpr int TILE_PADDING = 16;
	
	// Maps Display* to its grid cell assignment
	std::unordered_map<Display*, GridCell> m_DisplayCells;
	
	// Tracks displays currently being dragged and their preview cells
	std::unordered_map<Display*, GridCell> m_DraggingDisplays;
	
	// Tracks displays currently being resized and their preview cells
	std::unordered_map<Display*, GridCell> m_ResizingDisplays;
	
	// Listener identifier for cleanup
	std::string m_WindowResizeListenerId;
	
	// Display management callbacks
	DeleteDisplayCallback m_OnDeleteDisplay;
	CreateDisplayCallback m_OnCreateDisplay;
	
	// Button visibility
	bool m_ShowAddButtons = true;
	bool m_ShowDeleteButtons = true;
	
	// Spawn menu state
	struct SpawnMenuState
	{
		bool isOpen = false;
		GridCell targetCell;           // Cell where the display will be spawned
		Vector2 menuPosition;          // Screen position of the menu
		float elapsedTime = 0.0f;      // For close delay
		static constexpr float CLOSE_DELAY = 0.1f; // Delay before clicking outside closes menu
	};
	SpawnMenuState m_SpawnMenu;
	
	/**
	 * @brief Opens the spawn menu at the specified position
	 * @param cell The target cell for spawning
	 * @param screenPos The screen position to show the menu
	 */
	void OpenSpawnMenu(const GridCell& cell, Vector2 screenPos);
	
	/**
	 * @brief Closes the spawn menu
	 */
	void CloseSpawnMenu();
	
	/**
	 * @brief Draws the spawn menu if open
	 */
	void DrawSpawnMenu();
	
	/**
	 * @brief Handles input for the spawn menu
	 * @return true if input was consumed by the menu
	 */
	bool HandleSpawnMenuInput();
};

