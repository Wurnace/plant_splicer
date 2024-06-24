#pragma once

#include "Genome.h"

#include "Buffer.h"
#include "Vector2.h"

#include <memory>

#define LERP(a, b, k) a * k + b * (1 - k)

struct BranchData
{
	int length = 2; // Length of the Branch
	int branchingPoints[6] { 1, 1, 1, 1, 1, 1 }; // Where Along Each Branch do Child Branches begin
	int branchIndexes[6] {-1, -1, -1, -1, -1, -1}; // Genome Indices of Child Branches
	int numBranches = 0; // Number of Child Branches
	bool isDirPositive = 0; // Direction is Left or Right
	
	float widthChange = 0; // Amount Width changes each step
	FloatColour colourChange {0, 0, 0}; // Amount Colour changes each step
	float dirChange = 0; // Amount branch rotates each step
	float widthAdoption = 0; // How much colour is inherited from the point it splits up from
	float colourAdoption = 0; // How much colour is inherited from the point it splits up from
	float dirAdoption = 0; // How much dir is inherited
	
	float randomTurn = 0; // Variable amount branch rotates each step

	float dir = 0; // Direction of Branch
	float width = 25; // Width of Branch
	FloatColour colour { 0, 0, 0 }; // Colour of Branch
};

class Branch
{
public:
	struct Orientation
	{
		Vector2 pos{ 0, 0 }; // Position of Branch
		float dir = 0; // Direction of Branch
		FloatColour colour{ 0, 0, 0 }; // Colour of Branch
		float width = 0; // Width of Branch

		inline Orientation operator* (uint8_t sf) {
			return Orientation{ 
				pos * sf, 
				dir * sf, 
				FloatColour { colour.r * sf, colour.r * sf, colour.r * sf },
				width* sf
			};
		}
	};

public:
	Branch() {}
	Branch(BranchGenome& genomeData, Branch* parentBranch = nullptr);
	void Create(BranchGenome& genomeData, Branch* parentBranch = nullptr);
	void RenderBranch(const std::unique_ptr<sf::CircleShape>& circle, sf::RenderWindow* window, const Buffer<Branch>& allBranches, const Branch::Orientation& offset, uint32_t recursionDepth = 0) const;

public:
	BranchData data;
	uint32_t childIndices[3] { 0, 0, 0 }; // Child Index of 0 would mean no child for that index
	bool isChildBranch[3]{ true, true, true }; // Is child a branch or Fruit?
};

class Plant
{
public:
	Plant() {}
	Plant(Vector2 pos, sf::RenderWindow* window);

	void Render();
	
public:
	Vector2 pos;
	sf::RenderWindow* window = nullptr;

	static const uint32_t BRANCH_COUNT = 10;
	static const uint32_t FRUIT_COUNT = 2;
	Buffer<BranchGenome> branchGenes { BRANCH_COUNT };
	uint32_t InitBranches(uint32_t genomeIdx = 0, uint8_t recursionDepth = 0, Branch* parent = nullptr);
	void ResetIntermediate();

private:
	uint32_t GetBranchCount(uint32_t genomeIdx = 0, uint8_t recursionDepth = 0);

private:
	std::unique_ptr<sf::CircleShape> m_BranchRenderShape = std::make_unique<sf::CircleShape>();
	uint32_t m_IntermediateBranchCount = 0;
	Buffer<Branch> m_Branches{};
};
