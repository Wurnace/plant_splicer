#include "Plant.h"

#define MAX_RECUSION_DEPTH 5

Fruit::Fruit(FruitGenome& genomeData, Branch* parentBranch)
{
	Create(genomeData, parentBranch);
}

void Fruit::Create(FruitGenome& genomeData, Branch* parentBranch)
{
	data.length = genomeData.length;

	data.colourChanges[0] = genomeData.colourChanges[0];
	data.colourChanges[1] = genomeData.colourChanges[1];
	data.colourChanges[2] = genomeData.colourChanges[2];

	data.dirChanges[0] = genomeData.directionChanges[0];
	data.dirChanges[1] = genomeData.directionChanges[1];
	data.dirChanges[2] = genomeData.directionChanges[2];

	data.randomTurn = genomeData.randTurn;

	data.colour = genomeData.initColour;

	data.colourAdoption = genomeData.colourAdoption;
	data.widthAdoption = genomeData.widthAdoption;

	data.dir = LERP(genomeData.initDir, parentBranch->data.dir, genomeData.dirAdoption);
	data.width = LERP(genomeData.initWidth, parentBranch->data.width, genomeData.widthAdoption);
}

Branch::Branch(BranchGenome& genomeData, Branch* parentBranch)
{
	Create(genomeData, parentBranch);
}

void Branch::Create(BranchGenome& genomeData, Branch* parentBranch)
{
	data.length = genomeData.length + (int)(genomeData.lengthVariation * ((rand() % 201) - 100) / 100.0f);

	data.branchIndexes[0] = genomeData.branch0;
	data.branchIndexes[1] = genomeData.branch1;
	data.branchIndexes[2] = genomeData.branch2;

	data.branchingPoints[0] = (int)floor(genomeData.branch0Position * data.length) - 1;
	data.branchingPoints[1] = (int)floor(genomeData.branch1Position * data.length) - 1;
	data.branchingPoints[2] = (int)floor(genomeData.branch2Position * data.length) - 1;

	data.randomTurn = genomeData.randTurn;
	data.isDirPositive = rand() % 2;

	data.dirChange = genomeData.dirChange;
	data.widthChange = genomeData.widthChange;
	data.colourChange = genomeData.colourChange;

	data.colour = genomeData.initColour;
	data.colourAdoption = genomeData.colourAdoption;

	data.widthAdoption = genomeData.widthAdoption;
	data.dirAdoption = genomeData.dirAdoption;

	if (parentBranch)
	{
		data.dir = LERP(genomeData.initDir, parentBranch->data.dir, genomeData.dirAdoption);
		data.width = LERP(genomeData.initWidth, parentBranch->data.width, genomeData.widthAdoption);
	}
	else
	{
		data.dir = genomeData.initDir;
		data.width = genomeData.initWidth;
	}
}

static void RenderBranchSegment(
	const std::unique_ptr<sf::CircleShape>& circle,
	Vector2 position, float width, const FloatColour& colour,
	sf::RenderWindow* window
) {
	circle->setRadius(width);

	// Light
	circle->setFillColor(colour + FloatColour{ 25, 25, 25, 100 });
	circle->setPosition({ position.x - 1, position.y - 1 });
	window->draw(*circle);

	// Shadow
	circle->setFillColor(FloatColour(
		abs(colour.r - 25) + (colour.r - 25),
		abs(colour.g - 25) + (colour.g - 25),
		abs(colour.b - 25) + (colour.b - 25),
		100
	));
	circle->setPosition({ position.x + 1, position.y + 1 });
	window->draw(*circle);

	// Actual
	circle->setFillColor(colour);
	circle->setPosition({ position.x, position.y });
	window->draw(*circle);
}

void Branch::RenderBranch(
	const std::unique_ptr<sf::CircleShape>& circle,
	sf::RenderWindow* window,
	const Buffer<Branch>& allBranches, 
	const Branch::Orientation& offset = Branch::Orientation(),
	uint32_t recursionDepth
) const {
	FloatColour colour;

	Vector2 pos = offset.pos;
	float dir = LERP(data.dir, offset.dir, data.dirAdoption);
	float width = LERP((recursionDepth == 0 ? data.width : offset.width), data.width, data.widthAdoption);
	colour.r = floor(LERP((recursionDepth == 0 ? data.colour.r : offset.colour.r), data.colour.r, data.colourAdoption));
	colour.g = floor(LERP((recursionDepth == 0 ? data.colour.g : offset.colour.g), data.colour.g, data.colourAdoption));
	colour.b = floor(LERP((recursionDepth == 0 ? data.colour.b : offset.colour.b), data.colour.b, data.colourAdoption));

	for (int i = 0; i < data.length; i++)
	{
		dir += (data.dirChange + ((((rand() % 201) - 100) / 100.0f) * data.randomTurn)) * ((int)data.isDirPositive * 2 - 1);
		pos += Vector2(0, -1).rotateNew(dir);
		width += data.widthChange;
		colour += data.colourChange;

		RenderBranchSegment(circle, pos, width, colour, window);

		if (recursionDepth < MAX_RECUSION_DEPTH)
		{
			for (int j = 0; j < 3; j++)
			{
				if (i == data.branchingPoints[j] && data.branchIndexes[j] >= 0)
					allBranches[childIndices[j]].RenderBranch(circle, window, allBranches, { pos, dir, colour, width }, recursionDepth + 1);
			}
		}
	}
}

void Plant::ResetIntermediate()
{
	m_IntermediateBranchCount = 0;
}

Plant::Plant(Vector2 pos, sf::RenderWindow* window)
	:pos(pos), window(window)
{
	m_Branches.Create(GetBranchCount());
	m_Fruits.Create(GetFruitCount());
	InitBranches();
}

void Plant::Render()
{
	m_Branches[0].RenderBranch(
		m_BranchRenderShape, window, 
		m_Branches,
		Branch::Orientation { pos, 0, FloatColour{ 0, 0, 0 } }
	);
}

uint32_t Plant::GetBranchCount(uint32_t genomeIdx, uint8_t recursionDepth)
{
	if (genomeIdx > 13) return 0;
	if (recursionDepth >= MAX_RECUSION_DEPTH) return 1;

	return (
		GetBranchCount(branchGenes[genomeIdx].branch0, recursionDepth + 1) +
		GetBranchCount(branchGenes[genomeIdx].branch1, recursionDepth + 1) +
		GetBranchCount(branchGenes[genomeIdx].branch2, recursionDepth + 1) +
		1
	);
}

uint32_t Plant::GetFruitCount(uint32_t genomeIdx, uint8_t recursionDepth)
{
	if (genomeIdx >= 10) return 1;
	if (recursionDepth >= MAX_RECUSION_DEPTH) return 0;

	return (
		GetBranchCount(branchGenes[genomeIdx].branch0, recursionDepth + 1) +
		GetBranchCount(branchGenes[genomeIdx].branch1, recursionDepth + 1) +
		GetBranchCount(branchGenes[genomeIdx].branch2, recursionDepth + 1) +
		1
	);
}

uint32_t Plant::InitBranches(uint32_t genomeIdx, uint8_t recursionDepth, Branch* parent)
{
	uint32_t currentIndex = m_IntermediateBranchCount;
	m_Branches[currentIndex].Create(branchGenes[genomeIdx], parent);
	m_IntermediateBranchCount++;

	if (recursionDepth >= MAX_RECUSION_DEPTH)
		return currentIndex;

	for (int i = 0; i < 3; i++)
	{
		int index = m_Branches[currentIndex].data.branchIndexes[i];
		if (index >= 0)
		{
			if (index >= 10)
				m_Branches[currentIndex].childIndices[i] = InitFruit(index, &m_Branches[currentIndex]);
			else
				m_Branches[currentIndex].childIndices[i] = InitBranches(index, recursionDepth + 1, &m_Branches[currentIndex]);
		}
	}

	return currentIndex;
}

uint32_t Plant::InitFruit(uint32_t genomeIdx, Branch* parent)
{
	uint32_t currentIndex = m_IntermediateFruitCount;
	m_Fruits[currentIndex].Create(branchGenes[genomeIdx], parent);
	m_IntermediateFruitCount++;

	return currentIndex;
}
