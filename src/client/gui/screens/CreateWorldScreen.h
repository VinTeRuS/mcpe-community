#ifndef NET_MINECRAFT_CLIENT_GUI_SCREENS__CreateWorldScreen_H__
#define NET_MINECRAFT_CLIENT_GUI_SCREENS__CreateWorldScreen_H__

#include "ChooseLevelScreen.h"
#include "../components/TextBox.h"
#include "../components/Button.h"

class CreateWorldScreen : public ChooseLevelScreen
{
public:
	CreateWorldScreen();
	virtual ~CreateWorldScreen();

	void init();
	void setupPositions();
	void render(int xm, int ym, float a);
	void buttonClicked(Button* button);
	bool handleBackEvent(bool isDown);
	void mouseClicked(int x, int y, int buttonNum);
	void keyPressed(int eventKey);
	void keyboardNewChar(char inputChar);

private:
	Button* bGameMode;
	Button* bCreateWorld;
	Button* bCancel;
	TextBox* tbWorldName;
	TextBox* tbSeed;

	bool isCreative;
	bool hasStarted;
};

#endif /*NET_MINECRAFT_CLIENT_GUI_SCREENS__CreateWorldScreen_H__*/
