/*
=================
cButton.h
- Header file for class definition - SPECIFICATION
- Header file for the Sprite class
=================
*/
#ifndef _CBUTTON_H
#define _CBUTTON_H
#include "cSprite.h"

class cButton : public cSprite
{
private:

public:
	cButton();
	cButton(D3DXVECTOR3 sPosition, LPDIRECT3DDEVICE9 pd3dDevice, LPCSTR theFilename); // Constructor
	void update(float deltaTime);					// update
};
#endif

