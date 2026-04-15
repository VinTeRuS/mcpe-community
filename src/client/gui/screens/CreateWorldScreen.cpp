#include "CreateWorldScreen.h"
#include "ProgressScreen.h"
#include "ScreenChooser.h"
#include "../components/Button.h"
#include "../../Minecraft.h"
#include "../../../world/level/LevelSettings.h"
#include "../../../platform/time.h"
#include "../../../util/StringUtils.h"
#include <cstdio>

CreateWorldScreen::CreateWorldScreen()
: bGameMode(0), bCreateWorld(0), bCancel(0),
  tbWorldName(0), tbSeed(0),
  isCreative(false), hasStarted(false)
{
}

CreateWorldScreen::~CreateWorldScreen()
{
	delete bGameMode;
	delete bCreateWorld;
	delete bCancel;
	delete tbWorldName;
	delete tbSeed;
}

void CreateWorldScreen::init()
{
	Screen::init();
	
	tbWorldName = new TextBox(1, 0, 0, width - 40, 20, "New World");
	tbSeed = new TextBox(2, 0, 0, width - 40, 20, "");
	
	if (minecraft->useTouchscreen()) {
		bGameMode = new Touch::TButton(1, "Survival");
		bCreateWorld = new Touch::TButton(2, "Create World");
		bCancel = new Touch::TButton(3, "Cancel");
	} else {
		bGameMode = new Button(1, "Survival");
		bCreateWorld = new Button(2, "Create World");
		bCancel = new Button(3, "Cancel");
	}
	
	buttons.push_back(bGameMode);
	buttons.push_back(bCreateWorld);
	buttons.push_back(bCancel);
	
	textBoxes.push_back(tbWorldName);
	textBoxes.push_back(tbSeed);
	
	tabButtons.push_back(bGameMode);
	tabButtons.push_back(bCreateWorld);
	tabButtons.push_back(bCancel);
}

void CreateWorldScreen::setupPositions()
{
	int centerX = width / 2;
	
	tbWorldName->x = centerX - tbWorldName->w / 2;
	tbWorldName->y = height / 3 - 20;
	
	tbSeed->x = centerX - tbSeed->w / 2;
	tbSeed->y = height / 3 + 15;
	
	bGameMode->width = 120;
	bGameMode->x = centerX - bGameMode->width / 2;
	bGameMode->y = height / 3 + 50;
	
	bCreateWorld->width = bCancel->width = 120;
	bCreateWorld->x = centerX - bCreateWorld->width - 10;
	bCreateWorld->y = height - 60;
	
	bCancel->x = centerX + 10;
	bCancel->y = height - 60;
}

void CreateWorldScreen::render(int xm, int ym, float a)
{
	renderBackground();
	glEnable2(GL_BLEND);
	
	int centerX = width / 2;
	
	drawCenteredString(font, "World Name:", centerX, tbWorldName->y - 15, 0xffffffff);
	drawCenteredString(font, "Seed:", centerX, tbSeed->y - 15, 0xffffffff);
	
	Screen::render(xm, ym, a);
	
	tbWorldName->render(minecraft, xm, ym);
	tbSeed->render(minecraft, xm, ym);
	
	glDisable2(GL_BLEND);
}

void CreateWorldScreen::buttonClicked(Button* button)
{
	if (hasStarted)
		return;
	
	if (button == bCancel) {
		minecraft->screenChooser.setScreen(SCREEN_SELECTWORLD);
		return;
	}
	
	if (button == bGameMode) {
		isCreative = !isCreative;
		if (isCreative) {
			bGameMode->msg = "Creative";
		} else {
			bGameMode->msg = "Survival";
		}
		return;
	}
	
	if (button == bCreateWorld) {
		hasStarted = true;
		
		std::string levelName = tbWorldName->text;
		if (levelName.empty()) {
			levelName = "New World";
		}
		
		char idBuffer[32];
		snprintf(idBuffer, sizeof(idBuffer), "%ld", (long)getEpochTimeS());
		std::string levelId = getUniqueLevelName(idBuffer);
		
		int seed = (int)getEpochTimeS();
		std::string seedStr = tbSeed->text;
		if (!seedStr.empty()) {
			if (sscanf(seedStr.c_str(), "%d", &seed) <= 0) {
				seed = Util::hashCode(seedStr);
			}
		}
		
		int gType = isCreative ? GameType::Creative : GameType::Survival;
		LevelSettings settings(seed, gType);
		
		LOGI("Creating world: name='%s', id='%s', seed=%d, mode=%s\n",
			 levelName.c_str(), levelId.c_str(), seed,
			 isCreative ? "Creative" : "Survival");
		
		minecraft->selectLevel(levelId, levelName, settings);
		minecraft->hostMultiplayer();
		minecraft->setScreen(new ProgressScreen());
	}
}

bool CreateWorldScreen::handleBackEvent(bool isDown)
{
	if (!isDown) {
		minecraft->screenChooser.setScreen(SCREEN_SELECTWORLD);
	}
	return true;
}

void CreateWorldScreen::mouseClicked(int x, int y, int buttonNum)
{
	bool clickedOnTextbox = false;
	
	if (tbWorldName->isMouseOver(minecraft, x, y)) {
		tbSeed->loseFocus(minecraft);
		tbWorldName->setFocus(minecraft);
		clickedOnTextbox = true;
	} else if (tbSeed->isMouseOver(minecraft, x, y)) {
		tbWorldName->loseFocus(minecraft);
		tbSeed->setFocus(minecraft);
		clickedOnTextbox = true;
	} else {
		tbWorldName->loseFocus(minecraft);
		tbSeed->loseFocus(minecraft);
	}
	
	Screen::mouseClicked(x, y, buttonNum);
}

void CreateWorldScreen::keyPressed(int eventKey)
{
	if (eventKey == 250) { // Tab (250) - switch focus
		if (!tbWorldName->focused && !tbSeed->focused) {
			tbWorldName->setFocus(minecraft);
		} else if (tbWorldName->focused) {
			tbWorldName->loseFocus(minecraft);
			tbSeed->setFocus(minecraft);
		} else if (tbSeed->focused) {
			tbSeed->loseFocus(minecraft);
			tbWorldName->setFocus(minecraft);
		}
		return;
	}
	
	if (eventKey == 13 && !tbWorldName->focused && !tbSeed->focused) { // Enter - create world if no textbox focused
		buttonClicked(bCreateWorld);
		return;
	}
	
	if (eventKey == 8) { // Backspace
		if (tbWorldName->focused && !tbWorldName->text.empty()) {
			tbWorldName->text.pop_back();
		} else if (tbSeed->focused && !tbSeed->text.empty()) {
			tbSeed->text.pop_back();
		}
		return;
	}
	
	Screen::keyPressed(eventKey);
}

void CreateWorldScreen::keyboardNewChar(char inputChar)
{
	if (inputChar < 32 || inputChar == 127) { // Ignore control characters including backspace
		return;
	}
	if (tbWorldName->focused) {
		if (tbWorldName->text.length() < 30) {
			tbWorldName->text += inputChar;
		}
	} else if (tbSeed->focused) {
		if (tbSeed->text.length() < 20) {
			tbSeed->text += inputChar;
		}
	}
}
