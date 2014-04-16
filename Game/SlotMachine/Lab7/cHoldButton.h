/*
=================
cHoldButton.h
- Header file for class definition - SPECIFICATION
- Header file for the Sprite class
=================
*/
#ifndef _CHOLDBUTTON_H
#define _CHOLDBUTTON_H
#include "cSprite.h"

class cHoldButton : public cSprite
{
private:

public:
	cHoldButton();
	cHoldButton(D3DXVECTOR3 sPosition, LPDIRECT3DDEVICE9 pd3dDevice, LPCSTR theFilename); // Constructor
	void update(float deltaTime);					// update the balloons position based on the translation
	int CheckHold();								//check if allowed to hold
};
#endif