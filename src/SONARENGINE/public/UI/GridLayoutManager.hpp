#pragma once

#include "Base/Core.h"
#include "Base/Event.hpp"
#include "Base/EventDispatcher.hpp"
#include "Base/LayoutManager.hpp"
#include "Base/ContextMenu.hpp"
#include "Base/ContextMenuEntry.hpp"
#include "Base/SoftObject.hpp"
#include "Events/WindowResizeData.hpp"
#include "UI/Display.hpp"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

class Button;

struct SpawnMenuState
{
	bool isOpen = false;
	GridCell targetCell;
	Vector2 menuPosition;
	float elapsedTime = 0.0f;
	static constexpr float CLOSE_DELAY = 0.1f;
};

class SONARENGINE_API GridLayoutManager : public LayoutManager
{
	AUTOBODY(GridLayoutManager, LayoutManager)

public:
	GridLayoutManager(int rows, int columns, int windowWidth, int windowHeight);
	~GridLayoutManager();

	constexpr static int maxRows = 6;
	constexpr static int maxCols = 6;

	virtual bool RegisterDisplay(SoftObjectPath<IObject> display, std::any LayoutData) override;
	virtual bool UnregisterDisplay(SoftObjectPath<IObject> display) override;
	virtual void Tick(float DeltaTime) override;
	virtual void UpdateLayout() override;
	virtual void Initialize() override;
	virtual void OnWindowResize(int newWidth, int newHeight) override;
	virtual void SetSpawnOptions(std::vector<LayoutSpawnOption> spawnOptions) override;

	void OnDisplayResize(std::shared_ptr<IEvent> Event);
	void OnDisplayMove(std::shared_ptr<IEvent> Event);

	GridCell GetCellAtPosition(Vector2 screenPos) const;
	GridCell SnapToGrid(SoftObjectPath<Display> display, Vector2 screenPos, bool maintainSpan = true);
	bool MoveDisplayToCell(SoftObjectPath<Display> display, const GridCell& targetCell);
	bool ResizeDisplaySpan(SoftObjectPath<Display> display, int newRowSpan, int newColSpan);

	void AddRowEvent(std::shared_ptr<IEvent> Event);
	void AddColumnEvent(std::shared_ptr<IEvent> Event);
	void RemoveRowEvent(std::shared_ptr<IEvent> Event);
	void RemoveColumnEvent(std::shared_ptr<IEvent> Event);

	bool AddRow();
	bool AddColumn();
	bool RemoveRow();
	bool RemoveColumn();

	int GetMaxRows() const;
	int GetMaxColumns() const;

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
	void DrawGridControls();
	void DrawSnapPreviews() const;

	void SetDisplayDragging(SoftObjectPath<Display> display, bool isDragging, const GridCell& previewCell);
	void SetDisplayResizing(SoftObjectPath<Display> display, bool isResizing, const GridCell& previewCell);

	void SetShowAddButtons(bool show) { m_ShowAddButtons = show; }
	bool IsShowingAddButtons() const { return m_ShowAddButtons; }

	void SetShowDeleteButtons(bool show) { m_ShowDeleteButtons = show; }
	bool IsShowingDeleteButtons() const { return m_ShowDeleteButtons; }

	const std::unordered_map<SoftObjectPath<Display>, GridCell>& GetAllDisplays() const { return m_DisplayCells; }

private:
	void RecalculateCellDimensions();
	Rectangle GetGridContentRect() const;
	void ApplyLayoutToDisplay(SoftObjectPath<Display> display, const GridCell& cell);
	bool WouldOverlap(const GridCell& cell, SoftObjectPath<Display> ignoreDisplay) const;
	SoftObjectPath<Display> FindDisplayOccupyingCell(int row, int col) const;
	SoftObjectPath<Display> FindDisplayOccupyingCellForRendering(int row, int col) const;
	void SwapDisplays(SoftObjectPath<Display> displayA, SoftObjectPath<Display> displayB);
	void ResolveOverlaps();
	bool FindEmptyCell(int rowSpan, int colSpan, GridCell& outCell) const;
	void RebuildSpawnMenuEntries();

	int m_Rows;
	int m_Columns;
	int m_WindowWidth;
	int m_WindowHeight;
	int m_CellWidth;
	int m_CellHeight;

	static constexpr int CONTROL_BUTTON_SIZE = 28;
	static constexpr int CONTROL_BUTTON_MARGIN = 5;
	static constexpr int CONTROL_PANEL_WIDTH = CONTROL_BUTTON_SIZE * 2 + CONTROL_BUTTON_MARGIN * 3;
	static constexpr int CONTROL_PANEL_HEIGHT = CONTROL_BUTTON_SIZE * 2 + CONTROL_BUTTON_MARGIN * 3;
	static constexpr int GRID_OUTER_PADDING = 12;
	static constexpr int GRID_RIGHT_PADDING = CONTROL_PANEL_WIDTH + GRID_OUTER_PADDING * 2;
	static constexpr int GRID_BOTTOM_PADDING = CONTROL_PANEL_HEIGHT + GRID_OUTER_PADDING * 2;
	static constexpr int TILE_PADDING = 30;

	std::unordered_map<SoftObjectPath<Display>, GridCell> m_DisplayCells;
	std::unordered_map<SoftObjectPath<Display>, GridCell> m_DraggingDisplays;
	std::unordered_map<SoftObjectPath<Display>, GridCell> m_ResizingDisplays;

	std::string m_WindowResizeListenerId;

	bool m_ShowAddButtons = true;
	bool m_ShowDeleteButtons = true;

	SpawnMenuState m_SpawnMenu;
	std::shared_ptr<EventDispatcher> GridLayoutUIDispatcher;
	SoftObjectPath<ContextMenu> m_SpawnContextMenu;
	SoftObjectPath<Button> AddRowButton;
	SoftObjectPath<Button> RemoveRowButton;
	SoftObjectPath<Button> AddColumnButton;
	SoftObjectPath<Button> RemoveColumnButton;
	std::vector<SoftObjectPath<Button>> m_AllUIButtons;

	void InitializeControlButtons();
	void UpdateButtonPositions();
	void UpdateButtonPosition(SoftObjectPath<Button> inButton, Vector2 NewPos);

	Rectangle PanelRect;
	void ResizePanelRect();

	void OpenSpawnMenu(const GridCell& cell, Vector2 screenPos);
	void CloseSpawnMenu();
	bool HandleSpawnMenuInput();
	void DeleteDisplay(SoftObjectPath<Display> Display);
};