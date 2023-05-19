#pragma once
#include <vector>
#include <memory>

class Grid;

class Node final
{
public:
	enum class Direction { up, down, left, right };

	Node(int col, int row, Grid* pGrid);
	~Node();

	Node(const Node& other) = delete;
	Node(Node&& other) noexcept = delete;
	Node& operator=(const Node& other) = delete;
	Node& operator=(Node&& other) noexcept = delete;
	
	int GetCol() const;
	int GetRow() const;

	XMFLOAT2 GetWorldPos() const;
	Node* GetNeighbor(Direction direction) const;
	void SetBlocked(bool isBlocked);
	bool IsBlocked() const;
private:
	int m_Col, m_Row;
	Grid* m_pGrid;

	bool m_IsBlocked;
};

class Grid final
{
public:
	Grid(int nrCols, int nrRows, float cellSize);
	~Grid();

	Grid(const Grid& other) = delete;
	Grid(Grid&& other) noexcept = delete;
	Grid& operator=(const Grid& other) = delete;
	Grid& operator=(Grid&& other) noexcept = delete;

	XMFLOAT2 GetNodeWorldPos(int col, int row) const;
	Node* GetNode(const XMFLOAT2& worldPos) const;
	Node* GetNode(int col, int row) const;

	int GetNrCols() const;
	int GetNrRows() const;
	float GetCellSize() const;

private:
	const int m_NrCols, m_NrRows;
	const float m_CellSize;
	std::vector<std::unique_ptr<Node>> m_pNodes{};
	const float m_OffsetX, m_OffsetY;
};

