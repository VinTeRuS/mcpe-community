#ifndef NET_MINECRAFT_CLIENT_GUI_COMPONENTS__Slider_H__
#define NET_MINECRAFT_CLIENT_GUI_COMPONENTS__Slider_H__

#include "GuiElement.h"
#include "../../../client/Options.h"
#include <functional>
#include <vector>

enum SliderType {
	SliderProgress,
	SliderStep
};

class Slider : public GuiElement {
	typedef GuiElement super;
public:
	using ValueChangedCallback = std::function<void(int)>;
	using ValueChangedCallbackFloat = std::function<void(float)>;

	Slider(Minecraft* minecraft, const Options::Option* option, float progressMin, float progressMax);
	Slider(Minecraft* minecraft, const Options::Option* option, const std::vector<int>& stepVec);
	Slider(Minecraft* minecraft, const Options::Option* option, const std::vector<int>& stepVec, ValueChangedCallback callback);
	
	virtual void render(Minecraft* minecraft, int xm, int ym) override;
	virtual void mouseClicked(Minecraft* minecraft, int x, int y, int buttonNum) override;
	virtual void mouseReleased(Minecraft* minecraft, int x, int y, int buttonNum) override;
	virtual void tick(Minecraft* minecraft) override;

	void setValueChangedCallback(ValueChangedCallback callback);
	void setFloatValueChangedCallback(ValueChangedCallbackFloat callback);

private:
	void updateProgressFromMouse(int guiX);
	void updateStepFromMouse(int guiX);
	void applyValue(Minecraft* minecraft);

private:
	SliderType sliderType;
	std::vector<int> sliderSteps;
	bool mouseDownOnElement;
	float percentage;
	int curStepValue;
	int curStep;
	int numSteps;
	float progressMin;
	float progressMax;
	const Options::Option* option;

	ValueChangedCallback valueCallback;
	ValueChangedCallbackFloat floatValueCallback;

	int clickStartX;
	float clickStartPercentage;
	int clickStartStep;
};

#endif
