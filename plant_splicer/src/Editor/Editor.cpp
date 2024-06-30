#include "Editor.h"
#include <sstream>
#include <chrono>
#include <string>

#include <memory>

void Editor::Create()
{
	m_window = std::make_unique<sf::RenderWindow>();
	m_window->create(sf::VideoMode(500, 470), "Editor", sf::Style::Titlebar);
	font.loadFromFile("verdana.ttf");
	text.setFont(font);
	text.setString("Editor");

	for (int i = 0; i < 10; i++)
		managers[i].LinkButtons(branchGenomes[i]);

	settingsManager.LinkButtons(settings);
	splicingManager.LinkButtons(splicingSettings);
}

void Editor::DeActivateAll()
{
	for (int i = 0; i < 10; i++)
	{
		managers[i].activeButton = nullptr;
		managers[i].stringifiedNum = "";
	}

	settingsManager.activeButton = nullptr;
	settingsManager.stringifiedNum = "";
	splicingManager.activeButton = nullptr;
	splicingManager.stringifiedNum = "";
}

bool Editor::IsAnythingActive()
{
	for (int i = 0; i < 10; i++)
	{
		if (managers[i].activeButton)
			return true;
	}

	return settingsManager.activeButton || splicingManager.activeButton;
}

void Editor::Process()
{
	if (!m_window->isOpen())
		return;

	sf::Event ev;
	while(m_window->pollEvent(ev))
	{
		if (ev.type == sf::Event::KeyPressed)
		{
			if (!IsAnythingActive())
			{
				switch (ev.key.code)
				{
				case sf::Keyboard::O:
					m_FileManager.SaveGenomes(branchGenomes);
					break;

				case sf::Keyboard::I:
					m_FileManager.LoadGenomes(branchGenomes, splicingSettings.loadPath);
					break;

				case sf::Keyboard::P:
					m_FileManager.CreateSplicedPlant(splicingSettings.splice0Path, splicingSettings.splice1Path,
						std::chrono::system_clock::now().time_since_epoch().count(), branchGenomes);
					break;
				}
			}
		}
		if (mode != Menu)
		{
			if (ev.type == sf::Event::KeyPressed)
			{
				if (ev.key.code == sf::Keyboard::Escape)
				{
					mode = Menu;
					DeActivateAll();
				}
				currPage = 0;
			}
		}

		if(mode == BranchEditor)
		{
			if (ev.type == sf::Event::MouseButtonPressed && m_window->hasFocus())
			{
				managers[currBranch].ActivateButton((sf::Mouse::getPosition() - m_window->getPosition()).y + (30 * 15 * currPage), currPage);
			}
			else if (ev.type == sf::Event::KeyPressed)
			{
				managers[currBranch].ProcessInput(ev.key.code);
				switch (ev.key.code)
				{
				case sf::Keyboard::Left:
					if (currBranch > 0)
						currBranch -= 1;
					currPage = 0;
					DeActivateAll();
					break;

				case sf::Keyboard::Right:
					if (currBranch < 9)
						currBranch += 1;
					currPage = 0;
					DeActivateAll();
					break;

				case sf::Keyboard::Down:
					if (currPage < 2)
						currPage += 1;
					DeActivateAll();
					break;

				case sf::Keyboard::Up:
					if (currPage > 0)
						currPage -= 1;
					DeActivateAll();
					break;
				}
			}
		}
		if (mode == Constants || mode == Splicing)
		{
			if (ev.type == sf::Event::KeyPressed)
			{
				if (ev.key.code == sf::Keyboard::Up)
				{
					if (currPage < 0)
						currPage += 1;

					DeActivateAll();
				}
				if (ev.key.code == sf::Keyboard::Down)
				{
					if (currPage > 0)
						currPage -= 1;

					DeActivateAll();
					
				}
			}
		}
		if (mode == Constants)
		{
			if (ev.type == sf::Event::KeyPressed)
				settingsManager.ProcessInput(ev.key.code);
			else if (ev.type == sf::Event::MouseButtonPressed && m_window->hasFocus())
				settingsManager.ActivateButton((sf::Mouse::getPosition() - m_window->getPosition()).y + (30 * 15 * currPage), currPage);
		}
		if (mode == Splicing)
		{
			switch (ev.type)
			{
			case sf::Event::KeyPressed:
				if (ev.key.code == sf::Keyboard::Enter)
					splicingManager.ProcessInput(ev.key.code);
				break;

			case sf::Event::TextEntered:
				if (ev.text.unicode < 128 && ev.text.unicode != 8)
					splicingManager.ProcessInput(ev.text.unicode);
				break;

			case sf::Event::MouseButtonPressed:
				if (m_window->hasFocus())
					splicingManager.ActivateButton((sf::Mouse::getPosition() - m_window->getPosition()).y + (30 * 15 * currPage), currPage);
				break;
			}
		}
		if (mode == Menu)
		{
			if (ev.type == sf::Event::MouseButtonPressed)
			{
				int mY = (sf::Mouse::getPosition() - m_window->getPosition()).y;

				if (180 < mY && mY < 240)
					mode = BranchEditor;
				else if (280 < mY && mY < 340)
					mode = Constants;
				else if (380 < mY && mY < 440)
					mode = Splicing;
			}
		}
	}

	m_window->clear(settings.editorBG);

	switch (mode)
	{
	case Menu:
		return RenderMenu();

	case BranchEditor:
		return RenderBranchEditor();

	case Constants:
		return RenderConstants();
		
	case Splicing:
		return RenderSplicing();
	}
}

void Editor::RenderMenu()
{
	text.setCharacterSize(20);

	text.setString("Editor Menu");
	text.setPosition(250 - (text.getLocalBounds().width / 2), 20);
	m_window->draw(text);

	text.setString("Genetics Editor");
	text.setPosition(250 - (text.getLocalBounds().width / 2), 180);
	m_window->draw(text);

	text.setString("Settings");
	text.setPosition(250 - (text.getLocalBounds().width / 2), 280);
	m_window->draw(text);

	text.setString("Save/Load/Splice");
	text.setPosition(250 - (text.getLocalBounds().width / 2), 380);
	m_window->draw(text);

	m_window->display();
}
void Editor::RenderBranchEditor()
{
	text.setCharacterSize(11);
	text.setString(sf::String("(Left/Right arrow keys) Editing Branch num ") + char('0' + currBranch));
	text.setPosition(250 - (text.getLocalBounds().width / 2), 0);
	m_window->draw(text);
	text.setString(sf::String("(Up/Down arrow keys) Page ") + char('0' + currPage + 1) + sf::String("/3"));
	text.setPosition(250 - (text.getLocalBounds().width / 2), 15);
	m_window->draw(text);

	text.setCharacterSize(15);
	for (int i = 0; i < NUM_GENES_IN_BRANCH; i++)
	{
		if (managers[currBranch].buttons[i].page != currPage)
			continue;

		text.setString(managers[currBranch].buttons[i].label);
		text.setPosition(0, (managers[currBranch].buttons[i].yPos + 1) * 30);
		m_window->draw(text);

		if (managers[currBranch].activeButton == &managers[currBranch].buttons[i])
			text.setString(managers[currBranch].stringifiedNum);
		else if (managers[currBranch].buttons[i].floorToInt)
			text.setString(std::to_string(*managers[currBranch].buttons[i].intVal));
		else
			text.setString(std::to_string(*managers[currBranch].buttons[i].floatVal));

		text.setPosition(250, (managers[currBranch].buttons[i].yPos + 1) * 30);
		m_window->draw(text);
	}

	m_window->display();
}
void Editor::RenderConstants()
{
	text.setCharacterSize(11);
	text.setString(sf::String("(Up/Down arrow keys) Page ") + char('0' + currPage + 1) + sf::String("/1"));
	text.setPosition(250 - (text.getLocalBounds().width / 2), 7);
	m_window->draw(text);

	text.setCharacterSize(15);
	for (int i = 0; i < 6; i++)
	{
		if (settingsManager.buttons[i].page != currPage)
			continue;

		text.setString(settingsManager.buttons[i].label);
		text.setPosition(0, (settingsManager.buttons[i].yPos + 1) * 30);
		m_window->draw(text);

		if (settingsManager.activeButton == &settingsManager.buttons[i])
			text.setString(settingsManager.stringifiedNum);
		else if (settingsManager.buttons[i].floorToInt)
			text.setString(std::to_string(*settingsManager.buttons[i].intVal));
		else
			text.setString(std::to_string(*settingsManager.buttons[i].floatVal));

		text.setPosition(250, (settingsManager.buttons[i].yPos + 1) * 30);
		m_window->draw(text);
	}

	m_window->display();
}
void Editor::RenderSplicing()
{
	text.setCharacterSize(11);
	text.setString(sf::String("(Up/Down arrow keys) Page ") + char('0' + currPage + 1) + sf::String("/1"));
	text.setPosition(250 - (text.getLocalBounds().width / 2), 7);
	m_window->draw(text);

	text.setCharacterSize(15);

	for (int i = 0; i < 3; i++)
	{
		text.setString(splicingManager.buttons[i].label);
		text.setPosition(0, (splicingManager.buttons[i].yPos + 1) * 30);
		m_window->draw(text);

		if (splicingManager.activeButton == &splicingManager.buttons[i])
			text.setString(splicingManager.stringifiedNum);
		else
			text.setString(*splicingManager.buttons[i].value);

		text.setPosition(250, (splicingManager.buttons[i].yPos + 1) * 30);
		m_window->draw(text);
	}

	text.setString("KeyBinds");
	text.setPosition(250 - (text.getLocalBounds().width / 2), 300);
	m_window->draw(text);

	text.setString("I-Load/O-Save/P-Splice/Esc-Menu\nInteract with the render window to update it");
	text.setPosition(250 - (text.getLocalBounds().width / 2), 350);
	m_window->draw(text);

	m_window->display();
}

