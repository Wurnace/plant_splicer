#pragma once
#include "../Vector2.h"
#include "../Buffer.h"
#include "../Genome.h"
#include <SFML/System.hpp>

#define NUM_GENES_IN_BRANCH 22

class ValueEditButton
{
public:
	union { float* floatVal; int* intVal; };
	float max = 100;
	float min = -100;
	int yPos = 0;
	int page = 0;
	bool floorToInt = false;
	sf::String label;

	void InitButton(sf::String name, int buttonIndex = 0, int buttonsPerPage = 38, bool floorToIntN = false, float maxN = 1000, float minN = -1000);
	void PointButton(float& value);
	void PointButton(int& value);

};

class SoloBranchGenomeButtonManager
{
public:
	Buffer<ValueEditButton> buttons{NUM_GENES_IN_BRANCH};
	ValueEditButton* activeButton = nullptr;
	sf::String stringifiedNum = "";

	void LinkButtons(BranchGenome& linkedGenome);
	void ActivateButton(int mouseX, int currPage);
	void ProcessInput(sf::Keyboard::Key key);
};