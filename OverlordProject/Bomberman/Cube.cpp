#include "stdafx.h"
#include "Scenes/BombermanScene.h"
#include "Cube.h"
#include "Grid.h"
#include "PickUp.h"

Cube::Cube(Node* pNode, Grid* pGrid)
	:m_pNode{ pNode }, m_pGrid{ pGrid }
{
	m_pNode->SetCellState(CellState::Destructible);
	SetTag(L"Cube");
}

Cube::~Cube()
{
}

void Cube::DestroyCube()
{
	m_pNode->SetCellState(CellState::Empty);

	//Spawn a random pickUp
	int randomValue{ std::rand() % (static_cast<int>(PickUpType::Highest) * 1) };

	if (randomValue <= static_cast<int>(PickUpType::Highest))
	{
		BombermanScene::AddGameObject(new PickUp(m_pNode, m_pGrid, static_cast<PickUpType>(randomValue)));
	}

	BombermanScene::RemoveGameObject(this);
}
