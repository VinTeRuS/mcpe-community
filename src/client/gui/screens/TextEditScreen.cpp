#include "TextEditScreen.h"
#include "../../../world/level/tile/entity/SignTileEntity.h"
#include "../../../world/level/Level.h"
#include "../../../AppPlatform.h"
#include "../../Minecraft.h"
#include "../../renderer/tileentity/TileEntityRenderDispatcher.h"
#include "../../renderer/Tesselator.h"
#include "../../renderer/Textures.h"
#include "../../renderer/GameRenderer.h"
#include "../components/Button.h"
#include "../../../network/Packet.h"
#include "../../../network/RakNetInstance.h"
 TextEditScreen::TextEditScreen( SignTileEntity* signEntity )
 : sign(signEntity), isShowingKeyboard(false), frame(0), line(0), btnClose(1, "") {
	signX = signEntity ? signEntity->x : 0;
	signY = signEntity ? signEntity->y : 0;
	signZ = signEntity ? signEntity->z : 0;
 }
TextEditScreen::~TextEditScreen() {

}
void TextEditScreen::init() {
	super::init();
	minecraft->platform()->showKeyboard();
	isShowingKeyboard = true;
	ImageDef def;
	def.name = "gui/spritesheet.png";
	def.x = 0;
	def.y = 1;
	def.width = def.height = 18;
	def.setSrc(IntRectangle(60, 0, 18, 18));
	btnClose.setImageDef(def, true);
	btnClose.scaleWhenPressed = false;
	buttons.push_back(&btnClose);
}

void TextEditScreen::setupPositions() {
	btnClose.width = btnClose.height = 19;
	btnClose.x = width - btnClose.width;
	btnClose.y = 0;
}


bool TextEditScreen::handleBackEvent( bool isDown ) {
    sign->setChanged();
	Packet* signUpdatePacket = sign->getUpdatePacket();
	minecraft->raknetInstance->send(signUpdatePacket);
	minecraft->platform()->hideKeyboard();
	minecraft->setScreen(NULL);
	return true;
}

void TextEditScreen::render( int xm, int ym, float a ) {
	sign = (SignTileEntity*) minecraft->level->getTileEntity(signX, signY, signZ);
	if (!sign) {
		LOGW("TextEditScreen::render - sign entity not found, closing screen");
		minecraft->setScreen(NULL);
		return;
	}
	
	glDepthMask(GL_FALSE);
	renderBackground();
	glPushMatrix();
	glDepthMask(GL_TRUE);
	glDisable(GL_CULL_FACE);
	glLoadIdentity();
	Tesselator& t = Tesselator::instance;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrthof(0.0f, (float)minecraft->width, (float)minecraft->height, 0,  -1, 1);
	glMatrixMode(GL_MODELVIEW);
	
	minecraft->textures->loadAndBindTexture("item/sign.png");
    glColor4f2(1, 1, 1, 1);

	static float minUV[] = {0.03126f, 0.06249f};
    static float maxUV[] = {0.39063f, 0.4374f};
	float scale = ((minecraft->height / 2) / 32) * 0.9f;
	
	glTranslatef(minecraft->width / 2.0f, 5.0f, 0.0f);
	glScalef2(scale,scale,1);
	t.begin(GL_QUADS);
	t.vertexUV(-32, 0, 0.0f,minUV[0],minUV[1]);
	t.vertexUV(32,  0, 0.0f, maxUV[0], minUV[1]);
	t.vertexUV(32, 0 + 32, 0.0f, maxUV[0], maxUV[1]);
	t.vertexUV(-32, 0 + 32, 0.0f, minUV[0], maxUV[1]);
	t.draw();
	
	sign->selectedLine = line;
	float textScale = 8.0f / 11.0f;
	
	glTranslatef(0, 2 ,0);
	glScalef2(textScale, textScale, 1);
	for(int i = 0; i < 4; ++i) {
		std::string msg = sign->messages[i];
		if (i == sign->selectedLine && msg.length() < 14) {
			std::string s = "> " + msg + " <";
			font->draw(s, -(float)font->width(s) / 2.0f, 10.0f * i, 0xFF000000, false);
		} else {
			font->draw(msg, -(float)font->width(msg) / 2.0f, 10.0f * i, 0xFF000000, false);
		}
	}
	sign->selectedLine  = -1;
	
	glPopMatrix();
	glEnable(GL_CULL_FACE);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	super::render(xm, ym, a);
}

void TextEditScreen::lostFocus() {

}

void TextEditScreen::tick() {
	frame++;
}

void TextEditScreen::keyPressed( int eventKey ) {
	LOGW("Key pressed! [%d]", eventKey);
	if(eventKey == Keyboard::KEY_BACKSPACE) {
		if(sign->messages[line].length() > 0) {
			sign->messages[line].erase(sign->messages[line].size() - 1, 1);
		} else {
            line--;
            if(line < 0) {
                line = 3;
            }
        }
	} else if(eventKey == Keyboard::KEY_RETURN)  {
		line = (line + 1) % 4;
	} else {
		super::keyPressed(eventKey);
	}
}

void TextEditScreen::keyboardNewChar( char inputChar ) {
	std::string fullstring = sign->messages[line] + inputChar;
	if(fullstring.length() < 16) {
		sign->messages[line] = fullstring;
		//LOGW("Line text updated: %s\n", fullstring.c_str());
	}
}

void TextEditScreen::buttonClicked( Button* button ) {
	if(button == &btnClose)
		handleBackEvent(true);
}
