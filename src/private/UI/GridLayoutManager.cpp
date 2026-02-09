#include "Base/Core.h"
#include "UI/GridLayoutManager.hpp"
#include "UI/Display.hpp"
#include "Base/GameInstance.h"
#include "Events/AllPurposeEvent.h"
#include <set>
#include <omp.h>
#include "Events/DisplayResizeData.hpp"
#include "Events/DisplayMoveData.hpp"
#include "Base/GameMode.h"
#include "UI/Map.hpp"
#include "UI/WaterfallDisplay.hpp"
#include "UI/Button.h"
#include "Events/UIEvent.h"

GridLayoutManager::GridLayoutManager(int rows, int columns, int windowWidth, int windowHeight)
	: m_Rows(rows)
	, m_Columns(columns)
	, m_WindowWidth(windowWidth)
	, m_WindowHeight(windowHeight)
	, m_CellWidth(0)
	, m_CellHeight(0)
{


	m_Rows = std::max(1, std::min(rows, maxRows));
	m_Columns = std::max(1, std::min(columns, maxCols));

	RecalculateCellDimensions();
}

GridLayoutManager::~GridLayoutManager()
{
	GameInstance::AllPurposeDispatcher.RemoveListener(
		m_WindowResizeListenerId,
		AllPurposeEvent::StaticClass()
	);
}

void GridLayoutManager::RecalculateCellDimensions()
{
	if (m_Rows > 0 && m_Columns > 0)
	{
		m_CellWidth = m_WindowWidth / m_Columns;
		m_CellHeight = m_WindowHeight / m_Rows;
	}
}

bool GridLayoutManager::RegisterDisplay(SoftObjectPath<IObject> inDisplay, std::any LayoutData)
{
	if (!LayoutData.has_value())
	{
		LOG_ERROR("LayoutData has no Data");
		return false;
	}

	if (inDisplay.IsValid() == false)
	{
		LOG_ERROR("Display does not Exist");
		return false;
	}

	if (*(inDisplay.TryLoad()->GetStaticClass()) << Display::StaticClass() == false)
	{
		LOG_ERROR("GridLayoutManager::RegisterDisplay - inDisplay is not of type Display.");
		return false;
	}

	SoftObjectPath<Display> CastedDisplay = inDisplay.Cast<Display>();


	GridCell cell;
	try
	{
		cell = std::any_cast<GridCell>(LayoutData);
	}
	catch (const std::bad_any_cast&)
	{
		LOG_ERROR("GridLayoutManager::RegisterDisplay - Invalid LayoutData provided, expected GridCell.");
		return false;
	}

	if (!CastedDisplay)
	{
		return false;
	}

	if (cell.row < 0 || cell.column < 0 ||
		cell.row + cell.rowSpan > m_Rows ||
		cell.column + cell.colSpan > m_Columns)
	{
		return false;
	}

	if (WouldOverlap(cell, nullpath.Cast<Display>()))
	{
		return false;
	}

	m_DisplayCells.insert({ CastedDisplay, cell });

	ApplyLayoutToDisplay(CastedDisplay, cell);

	return true;
}

bool GridLayoutManager::UnregisterDisplay(SoftObjectPath<IObject> inDisplay)
{
	return m_DisplayCells.erase(inDisplay.Cast<Display>()) == 1;
}

void GridLayoutManager::UpdateLayout()
{
	if (!m_IsEnabled)
	{
		return;
	}

	for (auto& [ItDisplay, ItCell] : m_DisplayCells)
	{
		ApplyLayoutToDisplay(ItDisplay, ItCell);
	}
}

void GridLayoutManager::Initialize()
{
	
	m_SpawnContextMenu = GetOutter()->NewObject<ContextMenu>();
	m_SpawnContextMenu.TryLoad()->AddOnCloseCallback(std::bind(&GridLayoutManager::CloseSpawnMenu, this));

	GridLayoutUIDispatcher = std::make_shared<EventDispatcher>();


	ContextMenuEntry m_SpawnMapEntryWaterfall10;
	m_SpawnMapEntryWaterfall10.SetDisplayName("Waterfall 10 Seconds");
	ContextMenuEntry m_SpawnMapEntryWaterfall30;
	m_SpawnMapEntryWaterfall30.SetDisplayName("Waterfall 30 Seconds");
	ContextMenuEntry m_SpawnMapEntryWaterfall60;
	m_SpawnMapEntryWaterfall60.SetDisplayName("Waterfall 60 Seconds");
	ContextMenuEntry m_SpawnMapEntryWaterfall120;
	m_SpawnMapEntryWaterfall120.SetDisplayName("Waterfall 120 Seconds");
	ContextMenuEntry m_SpawnMapEntryWaterfall300;
	m_SpawnMapEntryWaterfall300.SetDisplayName("Waterfall 300 Seconds");

	ContextMenuEntry m_SpawnMapEntryMap;
	m_SpawnMapEntryMap.SetDisplayName("Map");

	if (auto SpawnMenu = m_SpawnContextMenu.TryLoad())
	{
		SpawnMenu->SetDisplayName("Spawn Menu");


		m_SpawnMapEntryWaterfall10.SetCallback([this](ContextMenuEntry* Self)
			{
				LOG_INFO("TEEESCHD");
				DisplaySpawnInfo info;
				info = DisplaySpawnInfo::CreateWaterfall(10);
				CreateDisplay(m_SpawnMenu.targetCell, info);
			});

		m_SpawnMapEntryWaterfall30.SetCallback([this](ContextMenuEntry* Self)
			{
				LOG_INFO("TEEESCHD");
				DisplaySpawnInfo info;
				info = DisplaySpawnInfo::CreateWaterfall(30);
				CreateDisplay(m_SpawnMenu.targetCell, info);
		});
		m_SpawnMapEntryWaterfall60.SetCallback([this](ContextMenuEntry* Self)
			{
				LOG_INFO("TEEESCHD");
				DisplaySpawnInfo info;
				info = DisplaySpawnInfo::CreateWaterfall(60);
				CreateDisplay(m_SpawnMenu.targetCell, info);
			});
		m_SpawnMapEntryWaterfall120.SetCallback([this](ContextMenuEntry* Self)
			{
				LOG_INFO("TEEESCHD");
				DisplaySpawnInfo info;
				info = DisplaySpawnInfo::CreateWaterfall(120);
				CreateDisplay(m_SpawnMenu.targetCell, info);
			});
		m_SpawnMapEntryWaterfall300.SetCallback([this](ContextMenuEntry* Self)
			{
				LOG_INFO("TEEESCHD");
				DisplaySpawnInfo info;
				info = DisplaySpawnInfo::CreateWaterfall(300);
				CreateDisplay(m_SpawnMenu.targetCell, info);
			});

		m_SpawnMapEntryMap.SetCallback([this](ContextMenuEntry* Self)
			{
				LOG_INFO("MAP SPAWNED");
				DisplaySpawnInfo info;
				info = DisplaySpawnInfo::CreateMap();
				CreateDisplay(m_SpawnMenu.targetCell, info);
			});

		SpawnMenu->AddMenuEntry(m_SpawnMapEntryMap);
		SpawnMenu->AddMenuEntry(m_SpawnMapEntryWaterfall10);
		SpawnMenu->AddMenuEntry(m_SpawnMapEntryWaterfall30);
		SpawnMenu->AddMenuEntry(m_SpawnMapEntryWaterfall60);
		SpawnMenu->AddMenuEntry(m_SpawnMapEntryWaterfall120);
		SpawnMenu->AddMenuEntry(m_SpawnMapEntryWaterfall300);

	}

	// The Rectangle for the Panel Controls
	ResizePanelRect();

	InitializeControlButtons();

	m_WindowResizeListenerId = "GridLayoutManager_" + std::to_string(reinterpret_cast<uintptr_t>(this));
	GameInstance::AllPurposeDispatcher.AddListener(m_WindowResizeListenerId, AllPurposeEvent::StaticClass(), [this](std::shared_ptr<IEvent> event)
		{
			auto allPurposeEvent = std::dynamic_pointer_cast<AllPurposeEvent>(event);
			if (allPurposeEvent && allPurposeEvent->Payload)
			{
				if (allPurposeEvent->Payload->GetStaticClass() == WindowResizeData::StaticClass())
				{
					auto resizeData = std::dynamic_pointer_cast<WindowResizeData>(allPurposeEvent->Payload);
					if (resizeData)
					{
						OnWindowResize(resizeData->width, resizeData->height);
					}
				}
			}
		}
	);
}

void GridLayoutManager::OnWindowResize(int newWidth, int newHeight)
{
	m_WindowWidth = newWidth;
	m_WindowHeight = newHeight;

	ResolveOverlaps();

	RecalculateCellDimensions();
	UpdateLayout();
	ResizePanelRect();
	UpdateButtonPositions();
}

void GridLayoutManager::OnDisplayResize(std::shared_ptr<IEvent> Event)
{
	std::shared_ptr<AllPurposeEvent> CastedEvent = std::dynamic_pointer_cast<AllPurposeEvent>(Event);
	if (!CastedEvent
		|| !CastedEvent->Payload
		|| (*(CastedEvent->Payload->GetStaticClass()) << DisplayResizeData::StaticClass()) == false)
	{
		return;
	}

	std::shared_ptr<DisplayResizeData> ResizeData = std::dynamic_pointer_cast<DisplayResizeData>(CastedEvent->Payload);
	auto DisplayToUse = ResizeData->DisplayToResize;
	auto PayloadNewHeight = ResizeData->NewHeight;
	auto PayloadNewWidth = ResizeData->NewWidth;

	auto displayObj = DisplayToUse.TryLoad();
	if (!displayObj)
	{
		return;
	}

	// Handle resize complete - snap to grid
	if (ResizeData->bIsResizeComplete)
	{
		SetDisplayResizing(DisplayToUse, false, {});

		int cellWidth = GetCellWidth();
		int cellHeight = GetCellHeight();

		if (cellWidth > 0 && cellHeight > 0)
		{
			int newColSpan = std::max(1, static_cast<int>(std::round(PayloadNewWidth / static_cast<float>(cellWidth))));
			int newRowSpan = std::max(1, static_cast<int>(std::round(PayloadNewHeight / static_cast<float>(cellHeight))));

			ResizeDisplaySpan(DisplayToUse, newRowSpan, newColSpan);
		}
		return;
	}

	// Handle resize in progress - show preview and constrain size
	int gridWidth = GetWindowWidth();
	int gridHeight = GetWindowHeight();

	// Get current display position to calculate maximum allowed size
	Rectangle destRect = displayObj->GetDestinationRect();
	int maxWidth = gridWidth - static_cast<int>(destRect.x);
	int maxHeight = gridHeight - static_cast<int>(destRect.y);

	// Clamp the NEW dimensions to stay within grid bounds
	auto newWidth = std::min(PayloadNewWidth, maxWidth);
	auto newHeight = std::min(PayloadNewHeight, maxHeight);

	// Calculate and show preview of which cells will be occupied
	int cellWidth = GetCellWidth();
	int cellHeight = GetCellHeight();

	if (cellWidth > 0 && cellHeight > 0)
	{
		const GridCell* currentCell = GetDisplayCell(DisplayToUse);
		if (currentCell)
		{
			int previewColSpan = std::max(1, static_cast<int>(std::round(newWidth / static_cast<float>(cellWidth))));
			int previewRowSpan = std::max(1, static_cast<int>(std::round(newHeight / static_cast<float>(cellHeight))));

			previewColSpan = std::min(previewColSpan, GetColumns() - currentCell->column);
			previewRowSpan = std::min(previewRowSpan, GetRows() - currentCell->row);

			GridCell previewCell = *currentCell;
			previewCell.colSpan = previewColSpan;
			previewCell.rowSpan = previewRowSpan;

			SetDisplayResizing(DisplayToUse, true, previewCell);
		}
	}

	if (newWidth != displayObj->GetWidth() || newHeight != displayObj->GetHeight())
	{
		displayObj->ResizeDisplay(newWidth, newHeight);
	}
}

void GridLayoutManager::OnDisplayMove(std::shared_ptr<IEvent> Event)
{
	std::shared_ptr<AllPurposeEvent> CastedEvent = std::dynamic_pointer_cast<AllPurposeEvent>(Event);
	if (!CastedEvent
		|| !CastedEvent->Payload
		|| (*(CastedEvent->Payload->GetStaticClass()) << DisplayMoveData::StaticClass()) == false)
	{
		return;
	}

	std::shared_ptr<DisplayMoveData> MoveData = std::dynamic_pointer_cast<DisplayMoveData>(CastedEvent->Payload);
	auto DisplayToUse = MoveData->DisplayToMove;

	auto displayObj = DisplayToUse.TryLoad();
	if (!displayObj)
	{
		return;
	}

	// Handle move complete - snap to grid
	if (MoveData->bIsMoveComplete)
	{
		// Calculate target cell based on mouse position
		Vector2 mousePos = { MoveData->MouseX, MoveData->MouseY };
		GridCell targetCell = SnapToGrid(DisplayToUse, mousePos, true);

		// Clear dragging state
		SetDisplayDragging(DisplayToUse, false, targetCell);

		// Move to the target cell
		MoveDisplayToCell(DisplayToUse, targetCell);
		return;
	}

	// Handle move in progress - constrain position and show preview
	int gridWidth = GetWindowWidth();
	int gridHeight = GetWindowHeight();

	// Clamp to keep display within grid bounds
	float newX = std::max(0.0f, std::min(MoveData->NewX, static_cast<float>(gridWidth - MoveData->DisplayWidth)));
	float newY = std::max(0.0f, std::min(MoveData->NewY, static_cast<float>(gridHeight - MoveData->DisplayHeight)));

	// Calculate preview cell and register for overlay drawing
	Vector2 mousePos = { MoveData->MouseX, MoveData->MouseY };
	GridCell previewCell = SnapToGrid(DisplayToUse, mousePos, true);
	SetDisplayDragging(DisplayToUse, true, previewCell);

	// Update display position
	displayObj->SetPosition({ newX, newY });
}

GridCell GridLayoutManager::GetCellAtPosition(Vector2 screenPos) const
{
	GridCell cell;

	if (m_CellWidth > 0 && m_CellHeight > 0)
	{
		cell.column = static_cast<int>(screenPos.x) / m_CellWidth;
		cell.row = static_cast<int>(screenPos.y) / m_CellHeight;

		// Clamp to grid bounds
		cell.column = std::max(0, std::min(cell.column, m_Columns - 1));
		cell.row = std::max(0, std::min(cell.row, m_Rows - 1));
	}

	return cell;
}

GridCell GridLayoutManager::SnapToGrid(SoftObjectPath<Display> InDisplay, Vector2 screenPos, bool maintainSpan)
{
	GridCell targetCell = GetCellAtPosition(screenPos);

	if (maintainSpan)
	{
		const GridCell* currentCell = GetDisplayCell(InDisplay);
		if (currentCell)
		{
			targetCell.rowSpan = currentCell->rowSpan;
			targetCell.colSpan = currentCell->colSpan;
		}
	}

	if (targetCell.row + targetCell.rowSpan > m_Rows)
	{
		targetCell.row = m_Rows - targetCell.rowSpan;
	}
	if (targetCell.column + targetCell.colSpan > m_Columns)
	{
		targetCell.column = m_Columns - targetCell.colSpan;
	}

	return targetCell;
}

bool GridLayoutManager::MoveDisplayToCell(SoftObjectPath<Display> InDisplay, const GridCell& targetCell)
{
	if (!InDisplay.IsValid())
	{
		return false;
	}

	auto it = m_DisplayCells.find(InDisplay);
	if (it == m_DisplayCells.end())
	{
		return false;
	}

	GridCell& currentCell = it->second;

	// Same cell - still need to snap back to proper grid position
	if (currentCell == targetCell)
	{
		ApplyLayoutToDisplay(InDisplay, currentCell);
		return true;
	}

	// Check bounds
	if (targetCell.row < 0 || targetCell.column < 0 ||
		targetCell.row + targetCell.rowSpan > m_Rows ||
		targetCell.column + targetCell.colSpan > m_Columns)
	{
		return false;
	}

	// Check for overlap and handle swapping
	SoftObjectPath<Display> occupyingDisplay = nullpath.Cast<Display>();

	// Find any display that would conflict
	for (int r = targetCell.row; r < targetCell.row + targetCell.rowSpan && !occupyingDisplay; ++r)
	{
		for (int c = targetCell.column; c < targetCell.column + targetCell.colSpan && !occupyingDisplay; ++c)
		{
			SoftObjectPath<Display> found = FindDisplayOccupyingCell(r, c);
			if (found.IsValid() && found != InDisplay)
			{
				occupyingDisplay = found;
			}
		}
	}

	if (occupyingDisplay.IsValid())
	{
		// Perform swap - the occupying display moves to our current position
		SwapDisplays(InDisplay, occupyingDisplay);
	}
	else
	{
		// No conflict, just move
		currentCell = targetCell;
		ApplyLayoutToDisplay(InDisplay, currentCell);
	}

	return true;
}

bool GridLayoutManager::ResizeDisplaySpan(SoftObjectPath<Display> InDisplay, int newRowSpan, int newColSpan)
{
	if (!InDisplay)
	{
		return false;
	}

	auto it = m_DisplayCells.find(InDisplay);
	if (it == m_DisplayCells.end())
	{
		return false;
	}

	GridCell& currentCell = it->second;

	// Clamp spans to valid range
	newRowSpan = std::max(1, newRowSpan);
	newColSpan = std::max(1, newColSpan);

	// Check if new span would go out of bounds - clamp to grid bounds
	if (currentCell.row + newRowSpan > m_Rows)
	{
		newRowSpan = m_Rows - currentCell.row;
	}
	if (currentCell.column + newColSpan > m_Columns)
	{
		newColSpan = m_Columns - currentCell.column;
	}

	// Check for overlap with new span (ignoring self)
	GridCell testCell = currentCell;
	testCell.rowSpan = newRowSpan;
	testCell.colSpan = newColSpan;

	if (WouldOverlap(testCell, InDisplay))
	{
		// Try to find the largest valid span that doesn't overlap
		// Start from the requested size and shrink until we find a valid one
		for (int tryRowSpan = newRowSpan; tryRowSpan >= 1; --tryRowSpan)
		{
			for (int tryColSpan = newColSpan; tryColSpan >= 1; --tryColSpan)
			{
				testCell.rowSpan = tryRowSpan;
				testCell.colSpan = tryColSpan;

				if (!WouldOverlap(testCell, InDisplay))
				{
					// Found a valid span - use it
					currentCell.rowSpan = tryRowSpan;
					currentCell.colSpan = tryColSpan;
					ApplyLayoutToDisplay(InDisplay, currentCell);
					return true;
				}
			}
		}

		// No valid span found, revert to original cell dimensions (1x1 minimum)
		ApplyLayoutToDisplay(InDisplay, currentCell);
		return false;
	}

	currentCell.rowSpan = newRowSpan;
	currentCell.colSpan = newColSpan;
	ApplyLayoutToDisplay(InDisplay, currentCell);

	return true;
}

void GridLayoutManager::AddRowEvent(std::shared_ptr<IEvent> Event)
{
	if (*(Event->GetStaticClass()) << UIEvent::StaticClass())
	{
		auto CastedEvent = std::dynamic_pointer_cast<UIEvent>(Event);
		if (CastedEvent->Payload == "AddRow")
		{
			AddRow();
			return;
		}
	}
}

void GridLayoutManager::AddColumnEvent(std::shared_ptr<IEvent> Event)
{
	if (*(Event->GetStaticClass()) << UIEvent::StaticClass())
	{
		auto CastedEvent = std::dynamic_pointer_cast<UIEvent>(Event);
		if (CastedEvent->Payload == "AddCol")
		{
			AddColumn();
			return;
		}
	}
}

void GridLayoutManager::RemoveRowEvent(std::shared_ptr<IEvent> Event)
{
	if (*(Event->GetStaticClass()) << UIEvent::StaticClass())
	{
		auto CastedEvent = std::dynamic_pointer_cast<UIEvent>(Event);
		if (CastedEvent->Payload == "RemoveRow")
		{
			RemoveRow();
			return;
		}
	}
}

void GridLayoutManager::RemoveColumnEvent(std::shared_ptr<IEvent> Event)
{
	if (*(Event->GetStaticClass()) << UIEvent::StaticClass())
	{
		auto CastedEvent = std::dynamic_pointer_cast<UIEvent>(Event);
		if (CastedEvent->Payload == "RemoveCol")
		{
			RemoveColumn();
			return;
		}
	}
}

bool GridLayoutManager::AddRow()
{
	int newRows = m_Rows + 1;
	int newCellHeight = m_WindowHeight / newRows;

	m_Rows = newRows;
	RecalculateCellDimensions();
	UpdateLayout();

	return true;
}

bool GridLayoutManager::AddColumn()
{
	int newColumns = m_Columns + 1;
	int newCellWidth = m_WindowWidth / newColumns;

	m_Columns = newColumns;
	RecalculateCellDimensions();
	UpdateLayout();

	return true;
}

bool GridLayoutManager::RemoveRow()
{
	// Enforce minimum 2x2 grid
	if (m_Rows <= 2)
	{
		return false;
	}

	// Also ensure we have enough cells for all displays
	int numDisplays = static_cast<int>(m_DisplayCells.size());
	if ((m_Rows - 1) * m_Columns < numDisplays)
	{
		SPDLOG_WARN("GridLayoutManager: Cannot remove row - not enough cells for all displays");
		return false;
	}

	// Check if any display is in the last row
	for (const auto& [display, cell] : m_DisplayCells)
	{
		if (cell.row + cell.rowSpan > m_Rows - 1)
		{
			SPDLOG_WARN("GridLayoutManager: Cannot remove row - display occupies cells in last row");
			return false;
		}
	}

	m_Rows--;
	RecalculateCellDimensions();
	UpdateLayout();

	return true;
}

bool GridLayoutManager::RemoveColumn()
{
	// Enforce minimum 2x2 grid
	if (m_Columns <= 2)
	{
		return false;
	}

	// Also ensure we have enough cells for all displays
	int numDisplays = static_cast<int>(m_DisplayCells.size());
	if (m_Rows * (m_Columns - 1) < numDisplays)
	{
		SPDLOG_WARN("GridLayoutManager: Cannot remove column - not enough cells for all displays");
		return false;
	}

	// Check if any display is in the last column
	for (const auto& [display, cell] : m_DisplayCells)
	{
		if (cell.column + cell.colSpan > m_Columns - 1)
		{
			SPDLOG_WARN("GridLayoutManager: Cannot remove column - display occupies cells in last column");
			return false;
		}
	}

	m_Columns--;
	RecalculateCellDimensions();
	UpdateLayout();

	return true;
}

int GridLayoutManager::GetMaxRows() const
{
	return maxRows;
}

int GridLayoutManager::GetMaxColumns() const
{
	return maxCols;
}

const GridCell* GridLayoutManager::GetDisplayCell(SoftObjectPath<Display> InDisplay) const
{
	auto it = m_DisplayCells.find(InDisplay);
	if (it != m_DisplayCells.end())
	{
		return &it->second;
	}
	return nullptr;
}

SoftObjectPath<Display> GridLayoutManager::GetDisplayAtCell(int row, int col) const
{
	return FindDisplayOccupyingCell(row, col);
}

bool GridLayoutManager::IsCellOccupied(int row, int col) const
{
	return FindDisplayOccupyingCell(row, col).IsValid();
}

Rectangle GridLayoutManager::GetCellRect(const GridCell& cell) const
{
	// Calculate base position and size
	float x = static_cast<float>(cell.column * m_CellWidth);
	float y = static_cast<float>(cell.row * m_CellHeight);
	float width = static_cast<float>(cell.colSpan * m_CellWidth);
	float height = static_cast<float>(cell.rowSpan * m_CellHeight);

	// Apply padding (half on each side)
	float padding = static_cast<float>(TILE_PADDING);
	x += padding / 2.0f;
	y += padding / 2.0f;
	width -= padding;
	height -= padding;

	return { x, y, width, height };
}

void GridLayoutManager::DrawDebugGrid() const
{
	// Draw vertical lines
	for (int c = 0; c <= m_Columns; ++c)
	{
		int x = c * m_CellWidth;
		DrawLine(x, 0, x, m_WindowHeight, DARKGREEN);
	}

	// Draw horizontal lines
	for (int r = 0; r <= m_Rows; ++r)
	{
		int y = r * m_CellHeight;
		DrawLine(0, y, m_WindowWidth, y, DARKGREEN);
	}

	// Draw cell labels
	for (int r = 0; r < m_Rows; ++r)
	{
		for (int c = 0; c < m_Columns; ++c)
		{
			int x = c * m_CellWidth + 5;
			int y = r * m_CellHeight + 5;

			std::string label = std::to_string(r) + "," + std::to_string(c);

			Color textColor = IsCellOccupied(r, c) ? GREEN : DARKGRAY;
			DrawText(label.c_str(), x, y, 12, textColor);
		}
	}
}

void GridLayoutManager::ApplyLayoutToDisplay(SoftObjectPath<Display> InDisplay, const GridCell& cell)
{
	auto display = InDisplay.TryLoad();
	if (!display || !m_IsEnabled)
	{
		return;
	}

	Rectangle rect = GetCellRect(cell);


	display->SetPosition({ rect.x, rect.y });
	display->ResizeDisplay(static_cast<int>(rect.width), static_cast<int>(rect.height));
}

bool GridLayoutManager::WouldOverlap(const GridCell& cell, SoftObjectPath<Display> DisplayToIgnore) const
{
	for (int r = cell.row; r < cell.row + cell.rowSpan; ++r)
	{
		for (int c = cell.column; c < cell.column + cell.colSpan; ++c)
		{
			SoftObjectPath<Display> occupying = FindDisplayOccupyingCell(r, c);
			if (occupying && occupying != DisplayToIgnore)
			{
				return true;
			}
		}
	}
	return false;
}

SoftObjectPath<Display> GridLayoutManager::FindDisplayOccupyingCell(int row, int col) const
{
	for (const auto& [display, cell] : m_DisplayCells)
	{
		if (row >= cell.row && row < cell.row + cell.rowSpan &&
			col >= cell.column && col < cell.column + cell.colSpan)
		{
			return display;
		}
	}
	SoftObjectPath<Display> nullDisplay;
	return nullDisplay;
}

void GridLayoutManager::SwapDisplays(SoftObjectPath<Display> displayA, SoftObjectPath<Display> displayB)
{
	if (!displayA || !displayB)
	{
		return;
	}

	auto itA = m_DisplayCells.find(displayA);
	auto itB = m_DisplayCells.find(displayB);

	if (itA == m_DisplayCells.end() || itB == m_DisplayCells.end())
	{
		return;
	}

	// Swap the cell assignments
	std::swap(itA->second, itB->second);

	// Apply new layouts
	ApplyLayoutToDisplay(displayA, itA->second);
	ApplyLayoutToDisplay(displayB, itB->second);
}

// TODO: Refactor to use button class
void GridLayoutManager::DrawGridControls()
{
	if (!m_ShowControls)
	{
		return;
	}

	// Draw delete buttons on each display if enabled
	if (m_ShowDeleteButtons)
	{
		for (const auto& [display, cell] : m_DisplayCells)
		{
			Rectangle cellRect = GetCellRect(cell);

			// Delete button in top-right corner of display
			float buttonSize = 24.0f;
			float margin = 8.0f;
			Rectangle deleteButtonRect = {
				cellRect.x + cellRect.width - buttonSize - margin,
				cellRect.y + margin,
				buttonSize,
				buttonSize
			};

			bool hover = CheckCollisionPointRec(GetMousePosition(), deleteButtonRect);
			if (hover)
			{
				SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
			}
			Color buttonColor = hover ? ColorAlpha(RED, 0.9f) : ColorAlpha(RED, 0.6f);
			Color iconColor = WHITE;

			DrawRectangleRounded(deleteButtonRect, 0.3f, 8, buttonColor);
			DrawRectangleRoundedLinesEx(deleteButtonRect, 0.3f, 8, 1, iconColor);

			// Draw X icon
			float iconPadding = buttonSize * 0.25f;
			float x1 = deleteButtonRect.x + iconPadding;
			float y1 = deleteButtonRect.y + iconPadding;
			float x2 = deleteButtonRect.x + buttonSize - iconPadding;
			float y2 = deleteButtonRect.y + buttonSize - iconPadding;

			DrawLineEx({ x1, y1 }, { x2, y2 }, 2, iconColor);
			DrawLineEx({ x2, y1 }, { x1, y2 }, 2, iconColor);

			// Handle click
			if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				std::cout << "Delete display at cell (" << cell.row << ", " << cell.column << ")\n";
				DeleteDisplay(display);
				return;
			}
		}
	}
}

void GridLayoutManager::DrawSnapPreviews() const
{
	// Draw preview overlays for all displays being dragged
	for (const auto& [display, previewCell] : m_DraggingDisplays)
	{
		Rectangle previewRect = GetCellRect(previewCell);

		// Draw filled rectangle with transparency
		DrawRectangleRec(previewRect, ColorAlpha(GREEN, 0.25f));
		// Draw border
		DrawRectangleLinesEx(previewRect, 3, ColorAlpha(GREEN, 0.9f));
	}

	// Draw preview overlays for all displays being resized
	for (const auto& [display, previewCell] : m_ResizingDisplays)
	{
		Rectangle previewRect = GetCellRect(previewCell);

		// Draw filled rectangle with transparency (use cyan to differentiate from move)
		DrawRectangleRec(previewRect, ColorAlpha(SKYBLUE, 0.25f));
		// Draw border
		DrawRectangleLinesEx(previewRect, 3, ColorAlpha(SKYBLUE, 0.9f));

		// Draw text showing the span dimensions
		const char* spanText = TextFormat("%dx%d", previewCell.colSpan, previewCell.rowSpan);
		int textWidth = MeasureText(spanText, 20);
		DrawText(spanText,
			static_cast<int>(previewRect.x + previewRect.width / 2 - textWidth / 2),
			static_cast<int>(previewRect.y + previewRect.height / 2 - 10),
			20, WHITE);
	}
}

void GridLayoutManager::SetDisplayDragging(SoftObjectPath<Display> display, bool isDragging, const GridCell& previewCell)
{
	if (isDragging)
	{
		m_DraggingDisplays[display] = previewCell;
	}
	else
	{
		m_DraggingDisplays.erase(display);
	}
}

void GridLayoutManager::SetDisplayResizing(SoftObjectPath<Display> display, bool isResizing, const GridCell& previewCell)
{
	if (isResizing)
	{
		m_ResizingDisplays[display] = previewCell;
	}
	else
	{
		m_ResizingDisplays.erase(display);
	}
}

void GridLayoutManager::ResolveOverlaps()
{
	std::set<std::pair<int, int>> assignedCells;
	std::vector<SoftObjectPath<Display>> displaysToDelete;

	auto wouldOverlapAssigned = [&assignedCells](const GridCell& cell) -> bool {
		for (int r = cell.row; r < cell.row + cell.rowSpan; ++r)
		{
			for (int c = cell.column; c < cell.column + cell.colSpan; ++c)
			{
				if (assignedCells.count({ r, c }))
				{
					return true;
				}
			}
		}
		return false;
		};

	auto findEmptySpot = [this, &assignedCells](int rowSpan, int colSpan, GridCell& outCell) -> bool {
		for (int r = 0; r <= m_Rows - rowSpan; ++r)
		{
			for (int c = 0; c <= m_Columns - colSpan; ++c)
			{
				bool isEmpty = true;
				for (int dr = 0; dr < rowSpan && isEmpty; ++dr)
				{
					for (int dc = 0; dc < colSpan && isEmpty; ++dc)
					{
						if (assignedCells.count({ r + dr, c + dc }))
						{
							isEmpty = false;
						}
					}
				}

				if (isEmpty)
				{
					outCell.row = r;
					outCell.column = c;
					outCell.rowSpan = rowSpan;
					outCell.colSpan = colSpan;
					return true;
				}
			}
		}
		return false;
		};

	auto markAssigned = [&assignedCells](const GridCell& cell) {
		for (int r = cell.row; r < cell.row + cell.rowSpan; ++r)
		{
			for (int c = cell.column; c < cell.column + cell.colSpan; ++c)
			{
				assignedCells.insert({ r, c });
			}
		}
		};

	for (auto& [display, cell] : m_DisplayCells)
	{
		if (!wouldOverlapAssigned(cell))
		{
			markAssigned(cell);
			continue;
		}

		GridCell newCell;
		if (findEmptySpot(cell.rowSpan, cell.colSpan, newCell))
		{
			cell = newCell;
			markAssigned(cell);
			continue;
		}

		bool found = false;
		for (int tryRowSpan = cell.rowSpan; tryRowSpan >= 1 && !found; --tryRowSpan)
		{
			for (int tryColSpan = cell.colSpan; tryColSpan >= 1 && !found; --tryColSpan)
			{
				if (findEmptySpot(tryRowSpan, tryColSpan, newCell))
				{
					cell = newCell;
					markAssigned(cell);
					found = true;
				}
			}
		}

		if (!found)
		{
			for (int r = 0; r < m_Rows && !found; ++r)
			{
				for (int c = 0; c < m_Columns && !found; ++c)
				{
					if (!assignedCells.count({ r, c }))
					{
						cell.row = r;
						cell.column = c;
						cell.rowSpan = 1;
						cell.colSpan = 1;
						markAssigned(cell);
						found = true;
					}
				}
			}
		}

		if (!found)
		{
			LOG_ERROR("GridLayoutManager: Unable to resolve overlap for display - no empty cells available");
			displaysToDelete.push_back(display);
		}
	}

	for (const auto& display : displaysToDelete)
	{

		DeleteDisplay(display);
	}
}

bool GridLayoutManager::FindEmptyCell(int rowSpan, int colSpan, GridCell& outCell) const
{
	// Build set of occupied cells
	std::set<std::pair<int, int>> occupiedCells;
	for (const auto& [display, cell] : m_DisplayCells)
	{
		for (int r = cell.row; r < cell.row + cell.rowSpan; ++r)
		{
			for (int c = cell.column; c < cell.column + cell.colSpan; ++c)
			{
				occupiedCells.insert({ r, c });
			}
		}
	}

	// Search for empty spot
	for (int r = 0; r <= m_Rows - rowSpan; ++r)
	{
		for (int c = 0; c <= m_Columns - colSpan; ++c)
		{
			bool isEmpty = true;
			for (int dr = 0; dr < rowSpan && isEmpty; ++dr)
			{
				for (int dc = 0; dc < colSpan && isEmpty; ++dc)
				{
					if (occupiedCells.count({ r + dr, c + dc }))
					{
						isEmpty = false;
					}
				}
			}

			if (isEmpty)
			{
				outCell.row = r;
				outCell.column = c;
				outCell.rowSpan = rowSpan;
				outCell.colSpan = colSpan;
				return true;
			}
		}
	}

	return false;
}

void GridLayoutManager::DrawEmptyTiles() const
{
	Vector2 mousePos = GetMousePosition();

	// Find all empty cells and draw a placeholder
	for (int r = 0; r < m_Rows; ++r)
	{
		for (int c = 0; c < m_Columns; ++c)
		{
			if (!IsCellOccupied(r, c))
			{
				GridCell emptyCell{ r, c, 1, 1 };
				Rectangle rect = GetCellRect(emptyCell);

				// Draw a subtle background rectangle for empty tiles
				DrawRectangleRec(rect, ColorAlpha(DARKGRAY, 0.3f));

				// Draw a dashed border to indicate it's an empty slot
				DrawRectangleLinesEx(rect, 1, ColorAlpha(GRAY, 0.5f));

				// Draw a plus icon/button in the center
				float centerX = rect.x + rect.width / 2;
				float centerY = rect.y + rect.height / 2;

				if (m_ShowAddButtons)
				{
					// Draw clickable add button
					float buttonSize = std::min(std::min(rect.width, rect.height) * 0.3f, 60.0f);
					Rectangle addButtonRect = {
						centerX - buttonSize / 2,
						centerY - buttonSize / 2,
						buttonSize,
						buttonSize
					};

					bool hover = CheckCollisionPointRec(mousePos, addButtonRect);
					Color buttonColor = hover ? ColorAlpha(GREEN, 0.7f) : ColorAlpha(GRAY, 0.5f);
					Color iconColor = hover ? WHITE : ColorAlpha(WHITE, 0.7f);

					DrawRectangleRounded(addButtonRect, 0.2f, 8, buttonColor);
					DrawRectangleRoundedLinesEx(addButtonRect, 0.2f, 8, 2, iconColor);

					// Draw plus icon
					float iconSize = buttonSize * 0.3f;
					DrawLineEx({ centerX - iconSize, centerY }, { centerX + iconSize, centerY }, 3, iconColor);
					DrawLineEx({ centerX, centerY - iconSize }, { centerX, centerY + iconSize }, 3, iconColor);

					// Handle click - open spawn menu instead of directly creating
					if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !m_SpawnMenu.isOpen)
					{
						const_cast<GridLayoutManager*>(this)->OpenSpawnMenu(emptyCell, mousePos);
					}
				}
				else
				{
					// Just draw a subtle plus icon
					float iconSize = std::min(rect.width, rect.height) * 0.15f;
					iconSize = std::min(iconSize, 20.0f);

					Color iconColor = ColorAlpha(GRAY, 0.4f);
					DrawLineEx({ centerX - iconSize, centerY }, { centerX + iconSize, centerY }, 2, iconColor);
					DrawLineEx({ centerX, centerY - iconSize }, { centerX, centerY + iconSize }, 2, iconColor);
				}
			}
		}
	}
}

void GridLayoutManager::InitializeControlButtons()
{
	Rectangle AddRowButtonRect = {
	static_cast<float>(PanelRect.x),
	static_cast<float>(PanelRect.y),
	static_cast<float>(CONTROL_BUTTON_SIZE),
	static_cast<float>(CONTROL_BUTTON_SIZE)
	};

	AddRowButton = this->GetOutter()->NewObject<Button>();
	RemoveRowButton = this->GetOutter()->NewObject<Button>();
	AddColumnButton = this->GetOutter()->NewObject<Button>();
	RemoveColumnButton = this->GetOutter()->NewObject<Button>();

	m_AllUIButtons.push_back(AddRowButton);
	m_AllUIButtons.push_back(RemoveRowButton);
	m_AllUIButtons.push_back(AddColumnButton);
	m_AllUIButtons.push_back(RemoveColumnButton);


	if (auto ButtonPtr = AddRowButton.TryLoad())
	{
		ButtonPtr->Construct(AddRowButtonRect, "+R", GREEN)
			.CenterText()
			.UpdateFontSize(14)
			.UpdateColor(ColorAlpha(GREEN, 0.6f))
			.OnHover([this](Button* ButtonClass)
				{
					ButtonClass->UpdateColor(ColorAlpha(GREEN, 0.9f));
				})
			.OnHoverLeave([this](Button* ButtonClass)
				{
					ButtonClass->UpdateColor(ColorAlpha(GREEN, 0.6f));
				})
			.SetEventDispatcher(GridLayoutUIDispatcher)
			.SetEventPayload("AddRow")
			.SetStickyPosition({ PanelRect.x, PanelRect.y });
	}

	if (auto ButtonPtr = RemoveRowButton.TryLoad())
	{
		ButtonPtr->Construct({
			static_cast<float>(PanelRect.x + CONTROL_BUTTON_SIZE + CONTROL_BUTTON_MARGIN),
			static_cast<float>(PanelRect.y),
			static_cast<float>(CONTROL_BUTTON_SIZE),
			static_cast<float>(CONTROL_BUTTON_SIZE)
			}, "-R", RED)
			.CenterText()
			.UpdateFontSize(14)
			.UpdateColor(ColorAlpha(RED, 0.6f))
			.OnHover([this](Button* ButtonClass)
				{
					ButtonClass->UpdateColor(ColorAlpha(RED, 0.9f));
				})
			.OnHoverLeave([this](Button* ButtonClass)
				{
					ButtonClass->UpdateColor(ColorAlpha(RED, 0.6f));
				})
			.SetEventDispatcher(GridLayoutUIDispatcher)
			.SetEventPayload("RemoveRow")
			.SetStickyPosition({ PanelRect.x + CONTROL_BUTTON_SIZE + CONTROL_BUTTON_MARGIN, PanelRect.y });
	}

	if (auto ButtonPtr = AddColumnButton.TryLoad())
	{
		ButtonPtr->Construct({
			static_cast<float>(PanelRect.x),
			static_cast<float>(PanelRect.y + CONTROL_BUTTON_SIZE + CONTROL_BUTTON_MARGIN),
			static_cast<float>(CONTROL_BUTTON_SIZE),
			static_cast<float>(CONTROL_BUTTON_SIZE)
			}, "+C", GREEN)
			.CenterText()
			.UpdateFontSize(14)
			.UpdateColor(ColorAlpha(GREEN, 0.6f))
			.OnHover([this](Button* ButtonClass)
				{
					ButtonClass->UpdateColor(ColorAlpha(GREEN, 0.9f));
				})
			.OnHoverLeave([this](Button* ButtonClass)
				{
					ButtonClass->UpdateColor(ColorAlpha(GREEN, 0.6f));
				})
			.SetEventDispatcher(GridLayoutUIDispatcher)
			.SetEventPayload("AddCol")
			.SetStickyPosition({ PanelRect.x, PanelRect.y + CONTROL_BUTTON_SIZE + CONTROL_BUTTON_MARGIN });
	}

	if (auto ButtonPtr = RemoveColumnButton.TryLoad())
	{
		ButtonPtr->Construct({
			static_cast<float>(PanelRect.x + CONTROL_BUTTON_SIZE + CONTROL_BUTTON_MARGIN),
			static_cast<float>(PanelRect.y + CONTROL_BUTTON_SIZE + CONTROL_BUTTON_MARGIN),
			static_cast<float>(CONTROL_BUTTON_SIZE),
			static_cast<float>(CONTROL_BUTTON_SIZE)
			}, "-C", RED)
			.CenterText()
			.UpdateFontSize(14)
			.UpdateColor(ColorAlpha(RED, 0.6f))
			.OnHover([this](Button* ButtonClass)
				{
					ButtonClass->UpdateColor(ColorAlpha(RED, 0.9f));
				})
			.OnHoverLeave([this](Button* ButtonClass)
				{
					ButtonClass->UpdateColor(ColorAlpha(RED, 0.6f));
				})
			.SetEventDispatcher(GridLayoutUIDispatcher)
			.SetEventPayload("RemoveCol")
			.SetStickyPosition({ PanelRect.x + CONTROL_BUTTON_SIZE + CONTROL_BUTTON_MARGIN, PanelRect.y + CONTROL_BUTTON_SIZE + CONTROL_BUTTON_MARGIN });
	}


	GridLayoutUIDispatcher->AddListener("AddRow", UIEvent::StaticClass(), this, &GridLayoutManager::AddRowEvent);
	GridLayoutUIDispatcher->AddListener("RemoveRow", UIEvent::StaticClass(), this, &GridLayoutManager::RemoveRowEvent);
	GridLayoutUIDispatcher->AddListener("AddCol", UIEvent::StaticClass(), this, &GridLayoutManager::AddColumnEvent);
	GridLayoutUIDispatcher->AddListener("RemoveCol", UIEvent::StaticClass(), this, &GridLayoutManager::RemoveColumnEvent);
}

void GridLayoutManager::UpdateButtonPositions()
{
	for (const auto& Button : m_AllUIButtons)
	{
		Button.TryLoad()->CalculateRelativePosition();
	}
}

void GridLayoutManager::UpdateButtonPosition(SoftObjectPath<Button> inButton, Vector2 NewPos)
{
	inButton.TryLoad()->UpdateButtonPosition(NewPos);
}

void GridLayoutManager::ResizePanelRect()
{
	int panelX = m_WindowWidth - (CONTROL_BUTTON_SIZE * 2 + CONTROL_BUTTON_MARGIN * 3);
	int panelY = m_WindowHeight - (CONTROL_BUTTON_SIZE * 2 + CONTROL_BUTTON_MARGIN * 3);

	PanelRect = {
		static_cast<float>(panelX),
		static_cast<float>(panelY),
		static_cast<float>(CONTROL_BUTTON_SIZE * 2 + CONTROL_BUTTON_MARGIN * 3),
		static_cast<float>(CONTROL_BUTTON_SIZE * 2 + CONTROL_BUTTON_MARGIN * 3)
	};
}

void GridLayoutManager::OpenSpawnMenu(const GridCell& cell, Vector2 screenPos)
{

	if (m_SpawnMenu.isOpen)
	{
		return;
	}

	m_SpawnMenu.isOpen = true;
	m_SpawnMenu.targetCell = cell;
	m_SpawnMenu.menuPosition = screenPos;
	m_SpawnMenu.elapsedTime = 0.0f;

	// Adjust position to stay within screen bounds
	float menuX = m_SpawnMenu.menuPosition.x;
	float menuY = m_SpawnMenu.menuPosition.y;


	m_SpawnContextMenu.TryLoad()->OnConstruct({ menuX, menuY });
	m_SpawnMenu.isOpen = true;
}

void GridLayoutManager::CloseSpawnMenu()
{
	m_SpawnMenu.isOpen = false;
}

bool GridLayoutManager::HandleSpawnMenuInput()
{
	// This method can be used if input handling needs to be separated from drawing
	// Currently, input is handled in DrawSpawnMenu for simplicity
	return m_SpawnMenu.isOpen;
}

void GridLayoutManager::DeleteDisplay(SoftObjectPath<Display> Display)
{
	if (!Display)
	{
		return;
	}

	UnregisterDisplay(Display);
	if (m_OnDeleteDisplay)
	{
		m_OnDeleteDisplay(Display);
	}
}

void GridLayoutManager::CreateDisplay(const GridCell& Cell, const DisplaySpawnInfo& SpawnInfo)
{
	switch (SpawnInfo.type)
	{
	case DisplayType::Map:
	{

		SoftObjectPath<Map> MapDisplay = GetOutter()->NewObject<Map>(400, 400);
		if (auto map = MapDisplay.TryLoad())
		{
			RegisterDisplay(MapDisplay, Cell);

			map->OnResize.AddListener("SandboxGameMode Map Resize Listener", AllPurposeEvent::StaticClass(), this, &GridLayoutManager::OnDisplayResize);
			map->OnMove.AddListener("SandboxGameMode Map Move Listener", AllPurposeEvent::StaticClass(), this, &GridLayoutManager::OnDisplayMove);
			if (m_OnCreateDisplay)
			{
				m_OnCreateDisplay(MapDisplay, SpawnInfo);
			}
		}
		if (auto CurrentSpawnMenu = m_SpawnContextMenu.TryLoad())
		{
			m_SpawnMenu.isOpen = false;
			CurrentSpawnMenu->OnDelete();
		}
		break;
	}

	case DisplayType::Waterfall:
	{
		SoftObjectPath<Waterfall> newWaterfall = GetOutter()->NewObject<Waterfall>(360, 300, SpawnInfo.waterfallTimeframeSecs);
		if (auto waterfall = newWaterfall.TryLoad())
		{
			RegisterDisplay(newWaterfall, Cell);
			waterfall->OnResize.AddListener("SandboxGameMode Waterfall Resize Listener", AllPurposeEvent::StaticClass(), this, &GridLayoutManager::OnDisplayResize);
			waterfall->OnMove.AddListener("SandboxGameMode Waterfall Move Listener", AllPurposeEvent::StaticClass(), this, &GridLayoutManager::OnDisplayMove);
			if (m_OnCreateDisplay)
			{
				m_OnCreateDisplay(newWaterfall, SpawnInfo);
			}
		}
		if (auto CurrentSpawnMenu = m_SpawnContextMenu.TryLoad())
		{
			m_SpawnMenu.isOpen = false;
			CurrentSpawnMenu->OnDelete();
		}
		break;
	}
	}
}

