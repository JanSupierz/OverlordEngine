#include "stdafx.h"
#include "Grid.h"

Grid::Grid(int nrCols, int nrRows, float cellSize)
	:m_NrCols{ nrCols }, m_NrRows{ nrRows }, m_CellSize{ cellSize }, 
	m_OffsetX{ nrCols * 0.5f * cellSize}, m_OffsetY{ nrRows * 0.5f * cellSize }
{
	for (int col{}; col < nrCols; ++col)
	{
		for (int row{}; row < nrRows; ++row)
		{
			m_pNodes.push_back(std::move(std::make_unique<Node>(col, row, this)));
		}
	}
}

Grid::~Grid()
{
}

XMFLOAT2 Grid::GetNodeWorldPos(int col, int row) const
{
	return XMFLOAT2(col * m_CellSize + m_CellSize * 0.5f - m_OffsetX, row * m_CellSize + m_CellSize * 0.5f - m_OffsetY);
}

Node* Grid::GetNode(const XMFLOAT2& worldPos) const
{
	int col{ static_cast<int>(worldPos.x + m_OffsetX) / static_cast<int>(m_CellSize) };
	int row{ static_cast<int>(worldPos.y + m_OffsetY) / static_cast<int>(m_CellSize) };

	return GetNode(col, row);
}

Node* Grid::GetNode(int col, int row) const
{
	if (col < m_NrCols && row < m_NrRows && col >= 0 && row >= 0)
	{
		return m_pNodes[static_cast<size_t>(col) * m_NrRows + static_cast<size_t>(row)].get();
	}
	else
	{
		return nullptr;
	}
}

int Grid::GetNrCols() const
{
	return m_NrCols;
}

int Grid::GetNrRows() const
{
	return m_NrRows;
}

float Grid::GetCellSize() const
{
	return m_CellSize;
}

Node::Node(int col, int row, Grid* pGrid)
	:m_Col{ col }, m_Row{ row }, m_pGrid{ pGrid }, m_CellState{ CellState::Empty }
{
}

Node::~Node()
{
}

int Node::GetCol() const
{
	return m_Col;
}

int Node::GetRow() const
{
	return m_Row;
}

XMFLOAT2 Node::GetWorldPos() const
{
	return m_pGrid->GetNodeWorldPos(m_Col, m_Row);
}

Node* Node::GetNeighbor(Direction direction) const
{
	int col{ m_Col }, row{ m_Row };

	switch (direction)
	{
	case Node::Direction::up:
		++row;
		break;
	case Node::Direction::down:
		--row;
		break;
	case Node::Direction::left:
		--col;
		break;
	case Node::Direction::right:
		++col;
		break;
	default:
		return nullptr;
		break;
	}

	return m_pGrid->GetNode(col, row);
}

void Node::SetCellState(CellState state)
{
	m_CellState = state;
}

CellState Node::GetCellState()
{
	return m_CellState;
}


