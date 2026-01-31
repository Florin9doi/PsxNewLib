#include <DigitalIO.h>
#include "PsxControllerHwSpi.h"

const byte PIN_PS2_ATT = 10;
PsxControllerHwSpi<PIN_PS2_ATT> psx;

static const byte BUFFER_SIZE = 10;
byte inputBuffer[BUFFER_SIZE];

/*
  [U]   [Sel] [St]    [RON] 
[L] [R]         [RIICHI] [KAN]
  [D]  [BtnB] [BtnA] [CHII]
                  [PON]
*/
enum WWBtn {
	SELECT = 1 <<  0,
	START  = 1 <<  3,
	UP     = 1 <<  4,
	RIGHT  = 1 <<  5,
	DOWN   = 1 <<  6,
	LEFT   = 1 <<  7,
	KAN    = 1 <<  8, // カン / L2
	PON    = 1 <<  9, // ポン / R2
	CHII   = 1 << 10, // チー / L1
	RIICHI = 1 << 11, // リーチ / R1
	RON    = 1 << 12, // ロン / Triangle
	BTN_A  = 1 << 13, // Circle
	BTN_B  = 1 << 14, // Cross
};
static const char btnNames[][7] = { "Select", "", "", "Start", "Up", "Right", "Down", "Left", "Kan", "Pon", "Chii", "Riichi", "Ron", "Btn_A", "Btn_B" };
void printButtons(u16 buttons) {
	if (buttons != 0) {
		Serial.print(buttons, HEX);
		Serial.print(" (");
	}
	for (int i = 0 ; i < 16; i++) {
		if ((1 << i) & buttons) {
			Serial.print(btnNames[i]);
			Serial.print(", ");
		}
	}
	if (buttons != 0) {
		Serial.print(")");
		Serial.println();
	}
}

void setup () {
	delay (300);
	Serial.begin (115200);

	delay (3000);
	Serial.println (F("Ready!"));

	static const byte cmd42[]   = {0x01, 0x42, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	static const byte cmd43_0[] = {0x01, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	static const byte cmd43_1[] = {0x01, 0x43, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
	static const byte cmd44[]   = {0x01, 0x44, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00};
	static const byte cmd45[]   = {0x01, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	static const byte cmd46_0[] = {0x01, 0x46, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	static const byte cmd46_1[] = {0x01, 0x46, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
	static const byte cmd47[]   = {0x01, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	static const byte cmd4c_0[] = {0x01, 0x4c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	static const byte cmd4c_1[] = {0x01, 0x4c, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
	static const byte cmd4d[]   = {0x01, 0x4d, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff};

	psx.setup();
	psx.shiftInOut (cmd42, inputBuffer, 9);
	psx.shiftInOut (cmd42, inputBuffer, 9);

	psx.shiftInOut (cmd43_1, inputBuffer, 9);
		psx.shiftInOut (cmd45, inputBuffer, 9);
		psx.shiftInOut (cmd46_0, inputBuffer, 9);
		psx.shiftInOut (cmd46_1, inputBuffer, 9);
		psx.shiftInOut (cmd47, inputBuffer, 9);
		psx.shiftInOut (cmd4c_0, inputBuffer, 9);
		psx.shiftInOut (cmd4c_1, inputBuffer, 9);

		psx.shiftInOut (cmd44, inputBuffer, 9); // analog/ext mode
		psx.shiftInOut (cmd4d, inputBuffer, 9); // rumble / enter waiwai mode
	psx.shiftInOut (cmd43_0, inputBuffer, 9);
	psx.shiftInOut (cmd43_0, inputBuffer, 9);
	psx.shiftInOut (cmd42, inputBuffer, 9);

	psx.shiftInOut (cmd43_1, inputBuffer, 9);
		psx.shiftInOut (cmd45, inputBuffer, 9);
	psx.shiftInOut (cmd43_0, inputBuffer, 9);
	psx.shiftInOut (cmd43_0, inputBuffer, 9);
	psx.shiftInOut (cmd42, inputBuffer, 9);


	// test cursor
	// static byte cmd42x[] = {0x01, 0x42, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	// static byte cursorArr[] = {0x50, 0x5e, 0x51, 0x4f, 0x52, 0x5f, 0x53};
	// for (int step = 0; step < sizeof(cursorArr); step++) {
	// 	static byte cmd42data[] = {0x00,0x00, 0xc2,0x50, 0x00,0x00};
	// 	cmd42data[3] = cursorArr[step];
	// 	for (int i = 0; i < sizeof(cmd42data); i++) {
	// 		cmd42x[4] = cmd42data[i];
	// 		psx.shiftInOut (cmd42x, inputBuffer, 9);
	// 	}
	// 	delay (150);
	// }

	// test first char
	// static byte cmd42x[] = {0x01, 0x42, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	// static byte boldArr[] = {0x60, 0x61, 0x62, 0x63, 0x64, 0x60};
	// for (int step = 0; step < sizeof(boldArr); step++) {
	// 	static byte cmd42data[] = {0x00,0x00,0x60,0x00,0x00};
	// 	cmd42data[2] = boldArr[step];
	// 	for (int i = 0; i < sizeof(cmd42data); i++) {
	// 		cmd42x[4] = cmd42data[i];
	// 		psx.shiftInOut (cmd42x, inputBuffer, 9);
	// 	}
	// 	delay (150);
	// }

	// test cards
	// static byte cmd42x[] = {0x01, 0x42, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	// static byte cmd42data[] = {0x00,0x00,0xc0,0x02,0x09,0x09,0x0a,0x0e,0x0f,0x10,0x11,0x16,0x17,0x18,0x1a,0x1c,0x15,0x00,0x00};
	// for (int i = 0; i < sizeof(cmd42data); i++) {
	// 	cmd42x[4] = cmd42data[i];
	// 	psx.shiftInOut (cmd42x, inputBuffer, 9);
	// }
	// delay (150);
}

void loop () {
	static bool triggerUpdate = true;
	static byte cmd42[] = {0x01, 0x42, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	//                            --cursor--- card1 unkn  card2 card3 card4 card5 card6 card7 card8 card9 car10 car11 car12 car13 car14 car15
	//                                  50-5e 60-64
	static byte waiwai[] = {0x00, 0xc2, 0x50, 0x64, 0xc0, 0x01, 0x20, 0x02, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00};

	if (triggerUpdate) {
		for (int i = 0; i < sizeof(waiwai); i++) {
			cmd42[4] = waiwai[i];
			psx.shiftInOut (cmd42, inputBuffer, 9);
		}
		triggerUpdate = false;
	} else {
		cmd42[4] = 0x00;
		psx.shiftInOut (cmd42, inputBuffer, 9);

		u16 buttons = ~(inputBuffer[3] | inputBuffer[4] << 8);
		printButtons(buttons);

		static u8 position = 6;
		static bool lUp = false, lDown = false, lLeft = false, lRight = false;
		bool printLog = false;

		//* flexible
		if (!lLeft && (buttons & WWBtn::LEFT)) {
			if (position > 1) {
				position--;
			} else {
				position = sizeof(waiwai) - 3;
			}
			printLog = true;
		}
		lLeft = buttons & WWBtn::LEFT;

		if (!lRight && (buttons & WWBtn::RIGHT)) {
			if (position < sizeof(waiwai) - 3) {
				position++;
			} else {
				position = 1;
			}
			printLog = true;
		}
		lRight = buttons & WWBtn::RIGHT;

		if (!lUp && (buttons & WWBtn::UP)) {
			waiwai[position]++;
			triggerUpdate = true;
			printLog = true;
		}
		lUp = buttons & WWBtn::UP;

		if (!lDown && (buttons & WWBtn::DOWN)) {
			waiwai[position]--;
			triggerUpdate = true;
			printLog = true;
		}
		lDown = buttons & WWBtn::DOWN;

		/*/ // strict

		if (!lUp && (buttons & WWBtn::UP)) {
			waiwai[6]++;
			triggerUpdate = true;
			printLog = true;
		}
		lUp = buttons & WWBtn::UP;

		if (!lDown && (buttons & WWBtn::DOWN)) {
			waiwai[6]--;
			triggerUpdate = true;
			printLog = true;
		}
		lDown = buttons & WWBtn::DOWN;

		if (triggerUpdate) {
			waiwai[5] = 0x3b;
			waiwai[7] = 0x3b;
			waiwai[8] = 0x2e + (waiwai[6] / 10);
			waiwai[9] = 0x2e + (waiwai[6] % 10);
			waiwai[10] = 0x3b;
			waiwai[11] = 0x2e + (waiwai[6] >> 4);
			waiwai[12] = 0x2e + (waiwai[6] & 0xf);
			waiwai[13] = 0x3b;
		}
		//*/

		if (printLog) {
			Serial.print("position="); Serial.print(position); Serial.print(" - ");
			for (int i = 0; i < sizeof(waiwai); i++) {
				if (position == i) Serial.print("[");
				Serial.print(waiwai[i], HEX);
				if (position == i) Serial.print("]");
				Serial.print(" ");
			}
			Serial.println();
		}

		delay (20);
	}
}
