/*
=================
cReel.h
- Header file for class definition - SPECIFICATION
- Header file for the Sprite class
=================
*/
#ifndef _CREEL_H
#define _CREEL_H
#include "cSprite.h"

class cReel : public cSprite
{
private:

public:
	cReel();
	cReel(D3DXVECTOR3 sPosition, LPDIRECT3DDEVICE9 pd3dDevice, LPCSTR theFilename); // Constructor
	void update(float deltaTime);					// update the balloons position based on the translation
	int ChooseFruit();								//choose the fruit for thereel
};
#endif