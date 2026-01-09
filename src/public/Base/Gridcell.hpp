#pragma once

struct GridCell
{
	int row = 0;         // Starting row (0-indexed)
	int column = 0;      // Starting column (0-indexed)
	int rowSpan = 1;     // Number of rows this display occupies
	int colSpan = 1;     // Number of columns this display occupies

	bool operator==(const GridCell& other) const
	{
		return row == other.row && column == other.column &&
			rowSpan == other.rowSpan && colSpan == other.colSpan;
	}
};