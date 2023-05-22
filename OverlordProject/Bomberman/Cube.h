#pragma once

class Grid;
class Node;
class BaseMaterial;

class Cube final : public GameObject
{
public:
	Cube(Node* pNode, Grid* pGrid);
	virtual ~Cube();

	Cube(const Cube& other) = delete;
	Cube(Cube&& other) noexcept = delete;
	Cube& operator=(const Cube& other) = delete;
	Cube& operator=(Cube&& other) noexcept = delete;

	void DestroyCube();

private:
	Node* m_pNode;
	Grid* m_pGrid;
};
