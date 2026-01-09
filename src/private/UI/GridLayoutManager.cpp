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

GridLayoutManager::GridLayoutManager(int rows, int columns, int windowWidth, int windowHeight)
	: m_Rows(rows)
	, m_Columns(columns)
	, m_WindowWidth(windowWidth)
	, m_WindowHeight(windowHeight)
	, m_CellWidth(0)
	, m_CellHeight(0)
{
	int maxRows = windowHeight / MIN_TILE_SIZE;
	int maxCols = windowWidth / MIN_TILE_SIZE;

	m_Rows = std::max(1, std::min(rows, maxRows));
	m_Columns = std::max(1, std::min(columns, maxCols));

	RecalculateCellDimensions();

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

void GridLayoutManager::OnWindowResize(int newWidth, int newHeight)
{
	m_WindowWidth = newWidth;
	m_WindowHeight = newHeight;


	int numDisplays = static_cast<int>(m_DisplayCells.size());
	int minCells = std::max(4, numDisplays);

	int minGridSize = static_cast<int>(std::ceil(std::sqrt(static_cast<double>(minCells))));
	int minRows = std::max(2, minGridSize);
	int minCols = std::max(2, minGridSize);

	int maxRows = newHeight / MIN_TILE_SIZE;
	int maxCols = newWidth / MIN_TILE_SIZE;

	if (m_Rows > maxRows && maxRows >= minRows)
	{
		m_Rows = maxRows;
	}
	else if (m_Rows > maxRows)
	{
		m_Rows = minRows;
	}

	if (m_Columns > maxCols && maxCols >= minCols)
	{
		m_Columns = maxCols;
	}
	else if (m_Columns > maxCols)
	{
		m_Columns = minCols;
	}

	m_Rows = std::max(m_Rows, minRows);
	m_Columns = std::max(m_Columns, minCols);


	const int count = static_cast<int>(m_DisplayCells.size());
	std::vector<SoftObjectPath<Display>> displayKeys;
	displayKeys.reserve(count);
	for (const auto& [display, cell] : m_DisplayCells)
	{
		displayKeys.push_back(display);
	}

	for (int i = 0; i < count; ++i)
	{
		auto& cell = m_DisplayCells[displayKeys[i]];

		if (cell.rowSpan > m_Rows)
		{
			cell.rowSpan = m_Rows;
		}

		if (cell.colSpan > m_Columns)
		{
			cell.colSpan = m_Columns;
		}

		if (cell.row + cell.rowSpan > m_Rows)
		{
			cell.row = m_Rows - cell.rowSpan;
		}

		if (cell.column + cell.colSpan > m_Columns)
		{
			cell.column = m_Columns - cell.colSpan;
		}

		cell.row = std::max(0, cell.row);
		cell.column = std::max(0, cell.column);
	}

	ResolveOverlaps();

	RecalculateCellDimensions();
	UpdateLayout();
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

bool GridLayoutManager::AddRow()
{
	int newRows = m_Rows + 1;
	int newCellHeight = m_WindowHeight / newRows;

	if (newCellHeight < MIN_TILE_SIZE)
	{
		SPDLOG_WARN("GridLayoutManager: Cannot add row - would violate minimum tile size of {}", MIN_TILE_SIZE);
		return false;
	}

	m_Rows = newRows;
	RecalculateCellDimensions();
	UpdateLayout();

	return true;
}

bool GridLayoutManager::AddColumn()
{
	int newColumns = m_Columns + 1;
	int newCellWidth = m_WindowWidth / newColumns;

	if (newCellWidth < MIN_TILE_SIZE)
	{
		SPDLOG_WARN("GridLayoutManager: Cannot add column - would violate minimum tile size of {}", MIN_TILE_SIZE);
		return false;
	}

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
	return m_WindowHeight / MIN_TILE_SIZE;
}

int GridLayoutManager::GetMaxColumns() const
{
	return m_WindowWidth / MIN_TILE_SIZE;
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

void GridLayoutManager::DrawGridControls()
{
	if (!m_ShowControls)
	{
		return;
	}

	Vector2 mousePos = GetMousePosition();

	// Control panel position (bottom-right corner)
	int panelX = m_WindowWidth - (CONTROL_BUTTON_SIZE * 2 + CONTROL_BUTTON_MARGIN * 3);
	int panelY = m_WindowHeight - (CONTROL_BUTTON_SIZE * 2 + CONTROL_BUTTON_MARGIN * 3);

	// Draw semi-transparent background panel
	Rectangle panelRect = {
		static_cast<float>(panelX - CONTROL_BUTTON_MARGIN),
		static_cast<float>(panelY - CONTROL_BUTTON_MARGIN),
		static_cast<float>(CONTROL_BUTTON_SIZE * 2 + CONTROL_BUTTON_MARGIN * 4),
		static_cast<float>(CONTROL_BUTTON_SIZE * 2 + CONTROL_BUTTON_MARGIN * 4)
	};
	DrawRectangleRec(panelRect, ColorAlpha(DARKGRAY, 0.8f));
	DrawRectangleLinesEx(panelRect, 1, LIGHTGRAY);

	// Row controls (top row of panel)
	// Add Row button (+)
	Rectangle addRowRect = {
		static_cast<float>(panelX),
		static_cast<float>(panelY),
		static_cast<float>(CONTROL_BUTTON_SIZE),
		static_cast<float>(CONTROL_BUTTON_SIZE)
	};
	bool canAddRow = (m_WindowHeight / (m_Rows + 1)) >= MIN_TILE_SIZE;
	Color addRowColor = canAddRow ? GREEN : GRAY;
	bool hoverAddRow = CheckCollisionPointRec(mousePos, addRowRect);

	DrawRectangleRec(addRowRect, hoverAddRow && canAddRow ? ColorAlpha(addRowColor, 0.9f) : ColorAlpha(addRowColor, 0.6f));
	DrawRectangleLinesEx(addRowRect, 1, WHITE);
	DrawText("+R", static_cast<int>(addRowRect.x + 4), static_cast<int>(addRowRect.y + 8), 14, WHITE);

	if (hoverAddRow && canAddRow && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		AddRow();
	}

	// Remove Row button (-)
	Rectangle removeRowRect = {
		static_cast<float>(panelX + CONTROL_BUTTON_SIZE + CONTROL_BUTTON_MARGIN),
		static_cast<float>(panelY),
		static_cast<float>(CONTROL_BUTTON_SIZE),
		static_cast<float>(CONTROL_BUTTON_SIZE)
	};
	bool canRemoveRow = m_Rows > 1;
	Color removeRowColor = canRemoveRow ? RED : GRAY;
	bool hoverRemoveRow = CheckCollisionPointRec(mousePos, removeRowRect);

	DrawRectangleRec(removeRowRect, hoverRemoveRow && canRemoveRow ? ColorAlpha(removeRowColor, 0.9f) : ColorAlpha(removeRowColor, 0.6f));
	DrawRectangleLinesEx(removeRowRect, 1, WHITE);
	DrawText("-R", static_cast<int>(removeRowRect.x + 4), static_cast<int>(removeRowRect.y + 8), 14, WHITE);

	if (hoverRemoveRow && canRemoveRow && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		RemoveRow();
	}

	// Column controls (bottom row of panel)
	// Add Column button (+)
	Rectangle addColRect = {
		static_cast<float>(panelX),
		static_cast<float>(panelY + CONTROL_BUTTON_SIZE + CONTROL_BUTTON_MARGIN),
		static_cast<float>(CONTROL_BUTTON_SIZE),
		static_cast<float>(CONTROL_BUTTON_SIZE)
	};
	bool canAddCol = (m_WindowWidth / (m_Columns + 1)) >= MIN_TILE_SIZE;
	Color addColColor = canAddCol ? GREEN : GRAY;
	bool hoverAddCol = CheckCollisionPointRec(mousePos, addColRect);

	DrawRectangleRec(addColRect, hoverAddCol && canAddCol ? ColorAlpha(addColColor, 0.9f) : ColorAlpha(addColColor, 0.6f));
	DrawRectangleLinesEx(addColRect, 1, WHITE);
	DrawText("+C", static_cast<int>(addColRect.x + 4), static_cast<int>(addColRect.y + 8), 14, WHITE);

	if (hoverAddCol && canAddCol && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		AddColumn();
	}

	// Remove Column button (-)
	Rectangle removeColRect = {
		static_cast<float>(panelX + CONTROL_BUTTON_SIZE + CONTROL_BUTTON_MARGIN),
		static_cast<float>(panelY + CONTROL_BUTTON_SIZE + CONTROL_BUTTON_MARGIN),
		static_cast<float>(CONTROL_BUTTON_SIZE),
		static_cast<float>(CONTROL_BUTTON_SIZE)
	};
	bool canRemoveCol = m_Columns > 1;
	Color removeColColor = canRemoveCol ? RED : GRAY;
	bool hoverRemoveCol = CheckCollisionPointRec(mousePos, removeColRect);

	DrawRectangleRec(removeColRect, hoverRemoveCol && canRemoveCol ? ColorAlpha(removeColColor, 0.9f) : ColorAlpha(removeColColor, 0.6f));
	DrawRectangleLinesEx(removeColRect, 1, WHITE);
	DrawText("-C", static_cast<int>(removeColRect.x + 4), static_cast<int>(removeColRect.y + 8), 14, WHITE);

	if (hoverRemoveCol && canRemoveCol && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		RemoveColumn();
	}

	// Draw current grid info
	std::string gridInfo = std::to_string(m_Rows) + "x" + std::to_string(m_Columns);
	int textWidth = MeasureText(gridInfo.c_str(), 12);
	DrawText(gridInfo.c_str(),
		static_cast<int>(panelRect.x + panelRect.width / 2 - textWidth / 2),
		static_cast<int>(panelRect.y - 15),
		12, WHITE);

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

			bool hover = CheckCollisionPointRec(mousePos, deleteButtonRect);
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
			if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !m_SpawnMenu.isOpen)
			{
				DeleteDisplay(display);
				return; // Exit early since we're modifying the collection
			}
		}
	}

	// Draw spawn menu on top of everything
	const_cast<GridLayoutManager*>(this)->DrawSpawnMenu();
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

void GridLayoutManager::OpenSpawnMenu(const GridCell& cell, Vector2 screenPos)
{
	m_SpawnMenu.isOpen = true;
	m_SpawnMenu.targetCell = cell;
	m_SpawnMenu.menuPosition = screenPos;
	m_SpawnMenu.elapsedTime = 0.0f;
}

void GridLayoutManager::CloseSpawnMenu()
{
	m_SpawnMenu.isOpen = false;
}

void GridLayoutManager::DrawSpawnMenu()
{
	if (!m_SpawnMenu.isOpen)
	{
		return;
	}

	// Update elapsed time using raylib's frame time
	m_SpawnMenu.elapsedTime += GetFrameTime();

	// Menu configuration
	const int fontSize = 14;
	const int padding = 8;
	const int itemHeight = fontSize + padding * 2;

	// Menu items
	struct MenuItem
	{
		const char* label;
		DisplaySpawnInfo spawnInfo;
	};

	std::vector<MenuItem> menuItems = {
		{"Map",               DisplaySpawnInfo::CreateMap()},
		{"Waterfall (10s)",   DisplaySpawnInfo::CreateWaterfall(10)},
		{"Waterfall (30s)",   DisplaySpawnInfo::CreateWaterfall(30)},
		{"Waterfall (60s)",   DisplaySpawnInfo::CreateWaterfall(60)},
		{"Waterfall (120s)",  DisplaySpawnInfo::CreateWaterfall(120)}
	};

	// Calculate menu dimensions
	int maxTextWidth = 0;
	for (const auto& item : menuItems)
	{
		int textWidth = MeasureText(item.label, fontSize);
		if (textWidth > maxTextWidth)
		{
			maxTextWidth = textWidth;
		}
	}

	int menuWidth = maxTextWidth + padding * 2;
	int menuHeight = static_cast<int>(menuItems.size()) * itemHeight;

	// Adjust position to stay within screen bounds
	float menuX = m_SpawnMenu.menuPosition.x;
	float menuY = m_SpawnMenu.menuPosition.y;

	if (menuX + menuWidth > m_WindowWidth)
	{
		menuX = m_WindowWidth - menuWidth;
	}
	if (menuY + menuHeight > m_WindowHeight)
	{
		menuY = m_WindowHeight - menuHeight;
	}

	Rectangle menuRect = { menuX, menuY, static_cast<float>(menuWidth), static_cast<float>(menuHeight) };

	// Draw menu background with border
	DrawRectangleRec(menuRect, ColorAlpha(DARKGRAY, 0.95f));
	DrawRectangleLinesEx(menuRect, 2, GREEN);

	// Draw menu items
	Vector2 mousePos = GetMousePosition();
	int clickedItem = -1;

	// Only allow clicks after the close delay (prevents the click that opened the menu from also selecting an item)
	bool canClick = m_SpawnMenu.elapsedTime >= SpawnMenuState::CLOSE_DELAY;

	for (size_t i = 0; i < menuItems.size(); ++i)
	{
		Rectangle itemRect = {
			menuX,
			menuY + static_cast<float>(i * itemHeight),
			static_cast<float>(menuWidth),
			static_cast<float>(itemHeight)
		};

		bool hover = CheckCollisionPointRec(mousePos, itemRect);

		// Draw hover highlight
		if (hover)
		{
			DrawRectangleRec(itemRect, ColorAlpha(GREEN, 0.3f));
		}

		// Draw text
		DrawText(
			menuItems[i].label,
			static_cast<int>(menuX + padding),
			static_cast<int>(itemRect.y + padding),
			fontSize,
			hover ? WHITE : LIGHTGRAY
		);

		// Check for click (only after delay)
		if (canClick && hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			clickedItem = static_cast<int>(i);
		}
	}

	// Handle selection
	if (clickedItem >= 0)
	{
		CreateDisplay(m_SpawnMenu.targetCell, menuItems[clickedItem].spawnInfo);
		CloseSpawnMenu();
		return;
	}

	// Handle clicking outside to close (with delay)
	if (m_SpawnMenu.elapsedTime >= SpawnMenuState::CLOSE_DELAY)
	{
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !CheckCollisionPointRec(mousePos, menuRect))
		{
			CloseSpawnMenu();
		}
		// Also close on right-click or escape
		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || IsKeyPressed(KEY_ESCAPE))
		{
			CloseSpawnMenu();
		}
	}
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
		break;
	}
	}
}
