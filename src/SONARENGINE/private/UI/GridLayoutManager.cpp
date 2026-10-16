#include "UI/GridLayoutManager.hpp"

#include "Base/EngineRuntimeContext.hpp"
#include "Base/GameMode.h"
#include "Events/AllPurposeEvent.h"
#include "Events/DisplayMoveData.hpp"
#include "Events/DisplayResizeData.hpp"
#include "Events/UIEvent.h"
#include "UI/Button.h"
#include <set>

GridLayoutManager::GridLayoutManager(int rows, int columns, int windowWidth, int windowHeight)
	: m_Rows(rows)
	, m_Columns(columns)
	, m_WindowWidth(windowWidth)
	, m_WindowHeight(windowHeight)
	, m_CellWidth(0)
	, m_CellHeight(0)
{
	SetTickGroup(ETickGroup::MAX);

	m_Rows = std::max(1, std::min(rows, maxRows));
	m_Columns = std::max(1, std::min(columns, maxCols));

	RecalculateCellDimensions();
}

GridLayoutManager::~GridLayoutManager()
{
	if (auto* Outter = GetOutter())
	{
		Outter->GetRuntimeContext().GetAllPurposeDispatcher().RemoveListener(
		m_WindowResizeListenerId,
		AllPurposeEvent::StaticClass());
	}
}

void GridLayoutManager::RecalculateCellDimensions()
{
	if (m_Rows > 0 && m_Columns > 0)
	{
		const Rectangle GridContentRect = GetGridContentRect();
		m_CellWidth = std::max(1, static_cast<int>(GridContentRect.width) / m_Columns);
		m_CellHeight = std::max(1, static_cast<int>(GridContentRect.height) / m_Rows);
	}
}

Rectangle GridLayoutManager::GetGridContentRect() const
{
	const float left = static_cast<float>(GRID_OUTER_PADDING);
	const float top = static_cast<float>(GRID_OUTER_PADDING);
	const float width = std::max(1.0f, static_cast<float>(m_WindowWidth - GRID_OUTER_PADDING - GRID_RIGHT_PADDING));
	const float height = std::max(1.0f, static_cast<float>(m_WindowHeight - GRID_OUTER_PADDING - GRID_BOTTOM_PADDING));

	return { left, top, width, height };
}

bool GridLayoutManager::RegisterDisplay(SoftObjectPath<IObject> inDisplay, std::any LayoutData)
{
	if (!LayoutData.has_value())
	{
		LOG_ERROR("LayoutData has no Data");
		return false;
	}

	if (!inDisplay.IsValid())
	{
		LOG_ERROR("Display does not Exist");
		return false;
	}

	auto DisplayObject = inDisplay.TryLoad();
	if (!DisplayObject || (*(DisplayObject->GetStaticClass()) << Display::StaticClass()) == false)
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

	auto CastedDisplayObject = CastedDisplay.TryLoad();
	if (!CastedDisplayObject)
	{
		return false;
	}

	CastedDisplayObject->OnResize.AddListener("GridLayoutManager Display Resize Listener", AllPurposeEvent::StaticClass(), this, &GridLayoutManager::OnDisplayResize);
	CastedDisplayObject->OnMove.AddListener("GridLayoutManager Display Move Listener", AllPurposeEvent::StaticClass(), this, &GridLayoutManager::OnDisplayMove);

	m_DisplayCells.insert({ CastedDisplay, cell });
	ApplyLayoutToDisplay(CastedDisplay, cell);

	return true;
}

bool GridLayoutManager::UnregisterDisplay(SoftObjectPath<IObject> inDisplay)
{
	return m_DisplayCells.erase(inDisplay.Cast<Display>()) == 1;
}

void GridLayoutManager::Tick(float DeltaTime)
{
	DrawEmptyTiles();
	DrawSnapPreviews();
	DrawGridControls();
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
	if (auto SpawnMenu = m_SpawnContextMenu.TryLoad())
	{
		SpawnMenu->SetDisplayName("Spawn Menu");
		SpawnMenu->AddOnCloseCallback(std::bind(&GridLayoutManager::CloseSpawnMenu, this));
	}

	GridLayoutUIDispatcher = std::make_shared<EventDispatcher>();

	ResizePanelRect();
	InitializeControlButtons();
	RebuildSpawnMenuEntries();

	m_WindowResizeListenerId = "GridLayoutManager_" + std::to_string(reinterpret_cast<uintptr_t>(this));
	GetOutter()->GetRuntimeContext().GetAllPurposeDispatcher().AddListener(m_WindowResizeListenerId, AllPurposeEvent::StaticClass(), [this](std::shared_ptr<IEvent> event)
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
		});
}

void GridLayoutManager::SetSpawnOptions(std::vector<LayoutSpawnOption> spawnOptions)
{
	LayoutManager::SetSpawnOptions(std::move(spawnOptions));
	RebuildSpawnMenuEntries();
}

void GridLayoutManager::RebuildSpawnMenuEntries()
{
	auto SpawnMenu = m_SpawnContextMenu.TryLoad();
	if (!SpawnMenu)
	{
		return;
	}

	SpawnMenu->ClearMenuEntries();

	for (const auto& SpawnOption : GetSpawnOptions())
	{
		ContextMenuEntry MenuEntry;
		MenuEntry.SetDisplayName(SpawnOption.DisplayName);
		MenuEntry.SetCallback([this, SpawnOption](ContextMenuEntry* Self)
			{
				RequestCreateDisplay(m_SpawnMenu.targetCell, SpawnOption.ToRequest());
				if (auto CurrentSpawnMenu = m_SpawnContextMenu.TryLoad())
				{
					m_SpawnMenu.isOpen = false;
					CurrentSpawnMenu->OnDelete();
				}
			});
		SpawnMenu->AddMenuEntry(MenuEntry);
	}
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
	if (!CastedEvent || !CastedEvent->Payload || (*(CastedEvent->Payload->GetStaticClass()) << DisplayResizeData::StaticClass()) == false)
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

	int gridWidth = GetWindowWidth();
	int gridHeight = GetWindowHeight();
	const Rectangle GridContentRect = GetGridContentRect();
	Rectangle destRect = displayObj->GetDestinationRect();
	int maxWidth = static_cast<int>(GridContentRect.x + GridContentRect.width - destRect.x);
	int maxHeight = static_cast<int>(GridContentRect.y + GridContentRect.height - destRect.y);
	auto newWidth = std::min(PayloadNewWidth, maxWidth);
	auto newHeight = std::min(PayloadNewHeight, maxHeight);

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
		else
		{
			m_ResizingDisplays.erase(DisplayToUse);
		}
	}
	else
	{
		m_ResizingDisplays.erase(DisplayToUse);
	}

	displayObj->SetDisplayPreviewRect({
		destRect.x,
		destRect.y,
		static_cast<float>(newWidth),
		static_cast<float>(newHeight)
	});
}

void GridLayoutManager::OnDisplayMove(std::shared_ptr<IEvent> Event)
{
	std::shared_ptr<AllPurposeEvent> CastedEvent = std::dynamic_pointer_cast<AllPurposeEvent>(Event);
	if (!CastedEvent || !CastedEvent->Payload || (*(CastedEvent->Payload->GetStaticClass()) << DisplayMoveData::StaticClass()) == false)
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

	if (MoveData->bIsMoveComplete)
	{
		Vector2 mousePos = { MoveData->MouseX, MoveData->MouseY };
		GridCell targetCell = SnapToGrid(DisplayToUse, mousePos, true);
		SetDisplayDragging(DisplayToUse, false, targetCell);
		MoveDisplayToCell(DisplayToUse, targetCell);
		return;
	}

	const Rectangle GridContentRect = GetGridContentRect();
	float minX = GridContentRect.x;
	float minY = GridContentRect.y;
	float maxX = GridContentRect.x + GridContentRect.width - MoveData->DisplayWidth;
	float maxY = GridContentRect.y + GridContentRect.height - MoveData->DisplayHeight;
	float newX = std::max(minX, std::min(MoveData->NewX, maxX));
	float newY = std::max(minY, std::min(MoveData->NewY, maxY));
	Vector2 mousePos = { MoveData->MouseX, MoveData->MouseY };
	GridCell previewCell = SnapToGrid(DisplayToUse, mousePos, true);
	SetDisplayDragging(DisplayToUse, true, previewCell);
	displayObj->SetPosition({ newX, newY });
}

GridCell GridLayoutManager::GetCellAtPosition(Vector2 screenPos) const
{
	GridCell cell;
	const Rectangle GridContentRect = GetGridContentRect();

	if (m_CellWidth > 0 && m_CellHeight > 0)
	{
		const float clampedX = std::clamp(screenPos.x, GridContentRect.x, GridContentRect.x + GridContentRect.width - 1.0f);
		const float clampedY = std::clamp(screenPos.y, GridContentRect.y, GridContentRect.y + GridContentRect.height - 1.0f);
		cell.column = static_cast<int>((clampedX - GridContentRect.x) / m_CellWidth);
		cell.row = static_cast<int>((clampedY - GridContentRect.y) / m_CellHeight);
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
	if (currentCell == targetCell)
	{
		ApplyLayoutToDisplay(InDisplay, currentCell);
		return true;
	}

	if (targetCell.row < 0 || targetCell.column < 0 ||
		targetCell.row + targetCell.rowSpan > m_Rows ||
		targetCell.column + targetCell.colSpan > m_Columns)
	{
		return false;
	}

	SoftObjectPath<Display> occupyingDisplay = nullpath.Cast<Display>();
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
		SwapDisplays(InDisplay, occupyingDisplay);
	}
	else
	{
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
	newRowSpan = std::max(1, newRowSpan);
	newColSpan = std::max(1, newColSpan);

	if (currentCell.row + newRowSpan > m_Rows)
	{
		newRowSpan = m_Rows - currentCell.row;
	}
	if (currentCell.column + newColSpan > m_Columns)
	{
		newColSpan = m_Columns - currentCell.column;
	}

	GridCell testCell = currentCell;
	testCell.rowSpan = newRowSpan;
	testCell.colSpan = newColSpan;

	if (WouldOverlap(testCell, InDisplay))
	{
		for (int tryRowSpan = newRowSpan; tryRowSpan >= 1; --tryRowSpan)
		{
			for (int tryColSpan = newColSpan; tryColSpan >= 1; --tryColSpan)
			{
				testCell.rowSpan = tryRowSpan;
				testCell.colSpan = tryColSpan;

				if (!WouldOverlap(testCell, InDisplay))
				{
					currentCell.rowSpan = tryRowSpan;
					currentCell.colSpan = tryColSpan;
					ApplyLayoutToDisplay(InDisplay, currentCell);
					return true;
				}
			}
		}

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
	m_Rows = m_Rows + 1;
	RecalculateCellDimensions();
	UpdateLayout();
	return true;
}

bool GridLayoutManager::AddColumn()
{
	m_Columns = m_Columns + 1;
	RecalculateCellDimensions();
	UpdateLayout();
	return true;
}

bool GridLayoutManager::RemoveRow()
{
	if (m_Rows <= 2)
	{
		return false;
	}

	int numDisplays = static_cast<int>(m_DisplayCells.size());
	if ((m_Rows - 1) * m_Columns < numDisplays)
	{
		SPDLOG_WARN("GridLayoutManager: Cannot remove row - not enough cells for all displays");
		return false;
	}

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
	if (m_Columns <= 2)
	{
		return false;
	}

	int numDisplays = static_cast<int>(m_DisplayCells.size());
	if (m_Rows * (m_Columns - 1) < numDisplays)
	{
		SPDLOG_WARN("GridLayoutManager: Cannot remove column - not enough cells for all displays");
		return false;
	}

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
	const Rectangle GridContentRect = GetGridContentRect();
	float x = GridContentRect.x + static_cast<float>(cell.column * m_CellWidth);
	float y = GridContentRect.y + static_cast<float>(cell.row * m_CellHeight);
	float width = static_cast<float>(cell.colSpan * m_CellWidth);
	float height = static_cast<float>(cell.rowSpan * m_CellHeight);
	float padding = static_cast<float>(TILE_PADDING);
	x += padding / 2.0f;
	y += padding / 2.0f;
	width -= padding;
	height -= padding;
	return { x, y, width, height };
}

void GridLayoutManager::DrawDebugGrid() const
{
	const Rectangle GridContentRect = GetGridContentRect();
	const int gridLeft = static_cast<int>(GridContentRect.x);
	const int gridTop = static_cast<int>(GridContentRect.y);
	const int gridRight = static_cast<int>(GridContentRect.x + m_CellWidth * m_Columns);
	const int gridBottom = static_cast<int>(GridContentRect.y + m_CellHeight * m_Rows);

	for (int c = 0; c <= m_Columns; ++c)
	{
		int x = gridLeft + c * m_CellWidth;
		DrawLine(x, gridTop, x, gridBottom, DARKGREEN);
	}

	for (int r = 0; r <= m_Rows; ++r)
	{
		int y = gridTop + r * m_CellHeight;
		DrawLine(gridLeft, y, gridRight, y, DARKGREEN);
	}

	for (int r = 0; r < m_Rows; ++r)
	{
		for (int c = 0; c < m_Columns; ++c)
		{
			int x = gridLeft + c * m_CellWidth + 5;
			int y = gridTop + r * m_CellHeight + 5;
			std::string label = std::to_string(r) + "," + std::to_string(c);
			Color textColor = FindDisplayOccupyingCellForRendering(r, c).IsValid() ? GREEN : DARKGRAY;
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
		if (row >= cell.row && row < cell.row + cell.rowSpan && col >= cell.column && col < cell.column + cell.colSpan)
		{
			return display;
		}
	}
	SoftObjectPath<Display> nullDisplay;
	return nullDisplay;
}

SoftObjectPath<Display> GridLayoutManager::FindDisplayOccupyingCellForRendering(int row, int col) const
{
	for (const auto& [display, cell] : m_DisplayCells)
	{
		const auto previewIt = m_ResizingDisplays.find(display);
		const GridCell& effectiveCell = previewIt != m_ResizingDisplays.end() ? previewIt->second : cell;

		if (row >= effectiveCell.row && row < effectiveCell.row + effectiveCell.rowSpan && col >= effectiveCell.column && col < effectiveCell.column + effectiveCell.colSpan)
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

	std::swap(itA->second, itB->second);
	ApplyLayoutToDisplay(displayA, itA->second);
	ApplyLayoutToDisplay(displayB, itB->second);
}

void GridLayoutManager::DrawGridControls()
{
	if (!m_ShowControls)
	{
		return;
	}

	if (m_ShowDeleteButtons)
	{
		for (const auto& [display, cell] : m_DisplayCells)
		{
			Rectangle cellRect = GetCellRect(cell);
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

			float iconPadding = buttonSize * 0.25f;
			float x1 = deleteButtonRect.x + iconPadding;
			float y1 = deleteButtonRect.y + iconPadding;
			float x2 = deleteButtonRect.x + buttonSize - iconPadding;
			float y2 = deleteButtonRect.y + buttonSize - iconPadding;

			DrawLineEx({ x1, y1 }, { x2, y2 }, 2, iconColor);
			DrawLineEx({ x2, y1 }, { x1, y2 }, 2, iconColor);

			if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				DeleteDisplay(display);
				return;
			}
		}
	}
}

void GridLayoutManager::DrawSnapPreviews() const
{
	for (const auto& [display, previewCell] : m_DraggingDisplays)
	{
		Rectangle previewRect = GetCellRect(previewCell);
		DrawRectangleRec(previewRect, ColorAlpha(GREEN, 0.25f));
		DrawRectangleLinesEx(previewRect, 3, ColorAlpha(GREEN, 0.9f));
	}

	for (const auto& [display, previewCell] : m_ResizingDisplays)
	{
		Rectangle previewRect = GetCellRect(previewCell);
		DrawRectangleRec(previewRect, ColorAlpha(SKYBLUE, 0.18f));
		DrawRectangleLinesEx(previewRect, 3, ColorAlpha(SKYBLUE, 0.85f));
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
		if (auto displayObj = display.TryLoad())
		{
			displayObj->ClearDisplayPreviewRect();
		}
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

	for (int r = 0; r < m_Rows; ++r)
	{
		for (int c = 0; c < m_Columns; ++c)
		{
			if (!FindDisplayOccupyingCellForRendering(r, c).IsValid())
			{
				GridCell emptyCell{ r, c, 1, 1 };
				Rectangle rect = GetCellRect(emptyCell);

				DrawRectangleRec(rect, ColorAlpha(DARKGRAY, 0.3f));
				DrawRectangleLinesEx(rect, 1, ColorAlpha(GRAY, 0.5f));

				float centerX = rect.x + rect.width / 2;
				float centerY = rect.y + rect.height / 2;

				if (m_ShowAddButtons)
				{
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

					float iconSize = buttonSize * 0.3f;
					DrawLineEx({ centerX - iconSize, centerY }, { centerX + iconSize, centerY }, 3, iconColor);
					DrawLineEx({ centerX, centerY - iconSize }, { centerX, centerY + iconSize }, 3, iconColor);

					if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !m_SpawnMenu.isOpen)
					{
						const_cast<GridLayoutManager*>(this)->OpenSpawnMenu(emptyCell, mousePos);
					}
				}
				else
				{
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
		if (auto ButtonPtr = Button.TryLoad())
		{
			ButtonPtr->CalculateRelativePosition();
		}
	}
}

void GridLayoutManager::UpdateButtonPosition(SoftObjectPath<Button> inButton, Vector2 NewPos)
{
	if (auto ButtonPtr = inButton.TryLoad())
	{
		ButtonPtr->UpdateButtonPosition(NewPos);
	}
}

void GridLayoutManager::ResizePanelRect()
{
	int panelX = m_WindowWidth - GRID_OUTER_PADDING - CONTROL_PANEL_WIDTH;
	int panelY = m_WindowHeight - GRID_OUTER_PADDING - CONTROL_PANEL_HEIGHT;

	PanelRect = {
		static_cast<float>(panelX),
		static_cast<float>(panelY),
		static_cast<float>(CONTROL_PANEL_WIDTH),
		static_cast<float>(CONTROL_PANEL_HEIGHT)
	};
}

void GridLayoutManager::OpenSpawnMenu(const GridCell& cell, Vector2 screenPos)
{
	if (m_SpawnMenu.isOpen || GetSpawnOptions().empty())
	{
		return;
	}

	m_SpawnMenu.isOpen = true;
	m_SpawnMenu.targetCell = cell;
	m_SpawnMenu.menuPosition = screenPos;
	m_SpawnMenu.elapsedTime = 0.0f;

	float menuX = m_SpawnMenu.menuPosition.x;
	float menuY = m_SpawnMenu.menuPosition.y;

	if (auto SpawnMenu = m_SpawnContextMenu.TryLoad())
	{
		SpawnMenu->OnConstruct({ menuX, menuY });
	}
	m_SpawnMenu.isOpen = true;
}

void GridLayoutManager::CloseSpawnMenu()
{
	m_SpawnMenu.isOpen = false;
}

bool GridLayoutManager::HandleSpawnMenuInput()
{
	return m_SpawnMenu.isOpen;
}

void GridLayoutManager::DeleteDisplay(SoftObjectPath<Display> Display)
{
	if (!Display)
	{
		return;
	}

	UnregisterDisplay(Display);
	RequestDeleteDisplay(Display);
}