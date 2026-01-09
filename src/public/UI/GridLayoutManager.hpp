#pragma once
#include "Base/Core.h"
#include "Base/EventDispatcher.hpp"
#include "Events/WindowResizeData.hpp"
#include "Base/Event.hpp"
#include "UI/Display.hpp"
#include "Base/SoftObject.hpp"
#include "Base/LayoutManager.hpp"
#include "Base/GridCell.hpp"

#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <string>



enum class DisplayType
{
	Map,
	Waterfall
};


struct DisplaySpawnInfo
{
	DisplayType type = DisplayType::Waterfall;
	int waterfallTimeframeSecs = 60;  // Only used for Waterfall type
	
	static DisplaySpawnInfo CreateMap() { return {DisplayType::Map, 0}; }
	static DisplaySpawnInfo CreateWaterfall(int timeframeSecs) { return {DisplayType::Waterfall, timeframeSecs}; }
};

struct SpawnMenuState
{
	bool isOpen = false;
	GridCell targetCell;           // Cell where the display will be spawned
	Vector2 menuPosition;          // Screen position of the menu
	float elapsedTime = 0.0f;      // For close delay
	static constexpr float CLOSE_DELAY = 0.1f; // Delay before clicking outside closes menu
};


class GridLayoutManager : public LayoutManager
{
	AUTOBODY(GridLayoutManager, LayoutManager)

public:
	static constexpr int MIN_TILE_SIZE = 250;
	
	GridLayoutManager(int rows, int columns, int windowWidth, int windowHeight);
	~GridLayoutManager();
	

	virtual bool RegisterDisplay(SoftObjectPath<IObject> display, std::any LayoutData) override;
	
	virtual bool UnregisterDisplay(SoftObjectPath<IObject> display) override;

	virtual void Tick(float DeltaTime) override {};
	
	virtual void UpdateLayout() override;
	
	void OnWindowResize(int newWidth, int newHeight);

	void OnDisplayResize(std::shared_ptr<IEvent> Event);
	
	void OnDisplayMove(std::shared_ptr<IEvent> Event);
	
	GridCell GetCellAtPosition(Vector2 screenPos) const;
	
	GridCell SnapToGrid(SoftObjectPath<Display> display, Vector2 screenPos, bool maintainSpan = true);
	
	bool MoveDisplayToCell(SoftObjectPath<Display> display, const GridCell& targetCell);

	bool ResizeDisplaySpan(SoftObjectPath<Display> display, int newRowSpan, int newColSpan);
	

	bool AddRow();
	bool AddColumn();
	bool RemoveRow();
	bool RemoveColumn();
	
	int GetMaxRows() const;
	int GetMaxColumns() const;
	
	// Getters
	int GetRows() const { return m_Rows; }
	int GetColumns() const { return m_Columns; }
	int GetCellWidth() const { return m_CellWidth; }
	int GetCellHeight() const { return m_CellHeight; }
	int GetWindowWidth() const { return m_WindowWidth; }
	int GetWindowHeight() const { return m_WindowHeight; }
	

	const GridCell* GetDisplayCell(SoftObjectPath<Display> display) const;
	SoftObjectPath<Display> GetDisplayAtCell(int row, int col) const;
	bool IsCellOccupied(int row, int col) const;
	Rectangle GetCellRect(const GridCell& cell) const;
	
	void DrawDebugGrid() const;
	void DrawEmptyTiles() const;

	void SetEnabled(bool enabled) { m_IsEnabled = enabled; }
	bool IsEnabled() const { return m_IsEnabled; }
	
	void DrawGridControls();
	void DrawSnapPreviews() const;
	

	void SetDisplayDragging(SoftObjectPath<Display> display, bool isDragging, const GridCell& previewCell);
	void SetDisplayResizing(SoftObjectPath<Display> display, bool isResizing, const GridCell& previewCell);
	

	void SetShowControls(bool show) { m_ShowControls = show; }
	bool IsShowingControls() const { return m_ShowControls; }
	
	// Callback types for display management
	using DeleteDisplayCallback = std::function<void(SoftObjectPath<Display>)>;
	using CreateDisplayCallback = std::function<void(const SoftObjectPath<Display>, const DisplaySpawnInfo)>;
	
	void SetDeleteDisplayCallback(DeleteDisplayCallback callback) { m_OnDeleteDisplay = callback; }
	void SetCreateDisplayCallback(CreateDisplayCallback callback) { m_OnCreateDisplay = callback; }
	

	void SetShowAddButtons(bool show) { m_ShowAddButtons = show; }
	bool IsShowingAddButtons() const { return m_ShowAddButtons; }
	
	void SetShowDeleteButtons(bool show) { m_ShowDeleteButtons = show; }
	bool IsShowingDeleteButtons() const { return m_ShowDeleteButtons; }
	
	const std::unordered_map<SoftObjectPath<Display>, GridCell>& GetAllDisplays() const { return m_DisplayCells; }

private:

	void RecalculateCellDimensions();
	void ApplyLayoutToDisplay(SoftObjectPath<Display> display, const GridCell& cell);
	bool WouldOverlap(const GridCell& cell, SoftObjectPath<Display> ignoreDisplay) const;
	
	SoftObjectPath<Display> FindDisplayOccupyingCell(int row, int col) const;
	void SwapDisplays(SoftObjectPath<Display> displayA, SoftObjectPath<Display> displayB);
	
	void ResolveOverlaps();
	
	bool FindEmptyCell(int rowSpan, int colSpan, GridCell& outCell) const;
	
	int m_Rows;
	int m_Columns;
	int m_WindowWidth;
	int m_WindowHeight;
	int m_CellWidth;
	int m_CellHeight;
	bool m_IsEnabled = true;
	bool m_ShowControls = true;
	
	static constexpr int CONTROL_BUTTON_SIZE = 28;
	static constexpr int CONTROL_BUTTON_MARGIN = 5;
	
	static constexpr int TILE_PADDING = 30;
	
	// Maps SoftObjectPath<Display> to its grid cell assignment
	std::unordered_map<SoftObjectPath<Display>, GridCell> m_DisplayCells;
	
	// Tracks displays currently being dragged and their preview cells
	std::unordered_map<SoftObjectPath<Display>, GridCell> m_DraggingDisplays;
	
	// Tracks displays currently being resized and their preview cells
	std::unordered_map<SoftObjectPath<Display>, GridCell> m_ResizingDisplays;
	
	std::string m_WindowResizeListenerId;
	
	DeleteDisplayCallback m_OnDeleteDisplay;
	CreateDisplayCallback m_OnCreateDisplay;
	
	bool m_ShowAddButtons = true;
	bool m_ShowDeleteButtons = true;

	SpawnMenuState m_SpawnMenu;
	

	void OpenSpawnMenu(const GridCell& cell, Vector2 screenPos);
	void CloseSpawnMenu();
	void DrawSpawnMenu();
	bool HandleSpawnMenuInput();

	void DeleteDisplay(SoftObjectPath<Display> Display);
	void CreateDisplay(const GridCell& cell, const DisplaySpawnInfo& spawnInfo);
};

