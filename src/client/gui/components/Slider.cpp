#include "Slider.h"
#include "../../Minecraft.h"
#include "../../renderer/Textures.h"
#include "../Screen.h"
#include "../../../util/Mth.h"
#include <algorithm>
#include <cassert>

Slider::Slider(Minecraft* minecraft, const Options::Option* option, float progressMin, float progressMax)
: sliderType(SliderProgress),
  mouseDownOnElement(false),
  option(option),
  numSteps(0),
  progressMin(progressMin),
  progressMax(progressMax),
  percentage(0.0f),
  curStepValue(0),
  curStep(0),
  clickStartX(0),
  clickStartPercentage(0.0f),
  clickStartStep(0) {
	if (option != NULL) {
		float value = minecraft->options.getProgressValue(option);
		percentage = (value - progressMin) / (progressMax - progressMin);
		percentage = Mth::clamp(percentage, 0.0f, 1.0f);
	}
}

Slider::Slider(Minecraft* minecraft, const Options::Option* option, const std::vector<int>& stepVec)
: sliderType(SliderStep),
  mouseDownOnElement(false),
  option(option),
  sliderSteps(stepVec),
  numSteps((int)stepVec.size()),
  percentage(0.0f),
  progressMin(0.0f),
  progressMax(1.0f),
  curStepValue(0),
  curStep(0),
  clickStartX(0),
  clickStartPercentage(0.0f),
  clickStartStep(0) {
	assert(stepVec.size() > 1);
	if (option != NULL) {
		int currentValue = minecraft->options.getIntValue(option);
		curStep = 0;
		curStepValue = sliderSteps[0];
		for (int i = 0; i < numSteps; i++) {
			if (sliderSteps[i] == currentValue) {
				curStep = i;
				curStepValue = currentValue;
				break;
			}
		}
		percentage = (float)curStep / (float)(numSteps - 1);
	}
}

Slider::Slider(Minecraft* minecraft, const Options::Option* option, const std::vector<int>& stepVec, ValueChangedCallback callback)
: Slider(minecraft, option, stepVec) {
	valueCallback = callback;
}

void Slider::setValueChangedCallback(ValueChangedCallback callback) {
	valueCallback = callback;
}

void Slider::setFloatValueChangedCallback(ValueChangedCallbackFloat callback) {
	floatValueCallback = callback;
}

void Slider::render(Minecraft* minecraft, int xm, int ym) {
	int xSliderStart = x + 5;
	int xSliderEnd = x + width - 4;
	int ySliderStart = y + 6;
	int ySliderEnd = y + 9;
	int handleSizeX = 9;
	int handleSizeY = 15;
	int barWidth = xSliderEnd - xSliderStart;

	fill(xSliderStart, ySliderStart, xSliderEnd, ySliderEnd, 0xff606060);

	if (sliderType == SliderStep) {
		for (int a = 0; a < numSteps; ++a) {
			int tickX = xSliderStart + (barWidth * a) / (numSteps - 1);
			fill(tickX - 1, ySliderStart - 2, tickX + 1, ySliderEnd + 2, 0xff606060);
		}

		minecraft->textures->loadAndBindTexture("gui/touchgui.png");
		int handleX = xSliderStart + (barWidth * curStep) / (numSteps - 1) - handleSizeX / 2;
		handleX = Mth::clamp(handleX, xSliderStart - handleSizeX / 2, xSliderEnd - handleSizeX / 2);
		blit(handleX, y, 226, 126, handleSizeX, handleSizeY, handleSizeX, handleSizeY);
	} else {
		minecraft->textures->loadAndBindTexture("gui/touchgui.png");
		int handleX = xSliderStart + (int)(percentage * barWidth) - handleSizeX / 2;
		handleX = Mth::clamp(handleX, xSliderStart - handleSizeX / 2, xSliderEnd - handleSizeX / 2);
		blit(handleX, y, 226, 126, handleSizeX, handleSizeY, handleSizeX, handleSizeY);
	}
}

void Slider::mouseClicked(Minecraft* minecraft, int x, int y, int buttonNum) {
	if (pointInside(x, y)) {
		mouseDownOnElement = true;
		clickStartX = x;
		clickStartPercentage = percentage;
		clickStartStep = curStep;
	}
}

void Slider::tick(Minecraft* minecraft) {
	if (!mouseDownOnElement) return;

	int xm = Mouse::getX();
	int ym = Mouse::getY();
	minecraft->screen->toGUICoordinate(xm, ym);

	updateProgressFromMouse(xm);
	if (sliderType == SliderStep) {
		updateStepFromMouse(xm);
	}
	applyValue(minecraft);
}

void Slider::updateProgressFromMouse(int guiX) {
	int barLeft = x + 5;
	int barWidth = width - 9;
	percentage = float(guiX - barLeft) / float(barWidth);
	percentage = Mth::clamp(percentage, 0.0f, 1.0f);
}

void Slider::updateStepFromMouse(int guiX) {
	int barLeft = x + 5;
	int barWidth = width - 9;
	int barPos = guiX - barLeft;
	int newStep = (barPos * (numSteps - 1)) / barWidth;
	newStep = Mth::clamp(newStep, 0, numSteps - 1);
	curStep = newStep;
	curStepValue = sliderSteps[curStep];
	percentage = (float)curStep / (float)(numSteps - 1);
}

void Slider::applyValue(Minecraft* minecraft) {
	if (option != NULL) {
		if (sliderType == SliderStep) {
			if (valueCallback) {
				valueCallback(curStepValue);
			} else {
				minecraft->options.set(option, curStepValue);
			}
		} else {
			float value = progressMin + percentage * (progressMax - progressMin);
			if (floatValueCallback) {
				floatValueCallback(value);
			} else {
				minecraft->options.set(option, value);
			}
		}
	}
}

void Slider::mouseReleased(Minecraft* minecraft, int x, int y, int buttonNum) {
	mouseDownOnElement = false;
}
