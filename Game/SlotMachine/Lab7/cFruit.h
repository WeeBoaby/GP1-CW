/*
=================
cFruit.h
- Header file for class definition - SPECIFICATION
- Header file for the Sprite class
=================
*/
#ifndef _CFRUIT_H
#define _CFRUIT_H
#include "cSprite.h"

class cFruit : public cSprite
{
private:
	eDirection mDirection;

public:
	
	cFruit();
	cFruit(D3DXVECTOR3 sPosition, LPDIRECT3DDEVICE9 pd3dDevice, LPCSTR theFilename); // Constructor
	void update(float deltaTime);					// update the sprites position based on the translation
	eDirection getDirection();      // Rturns the current direction of the sprite
	void setDirection(eDirection theDirection); // Sets the current direction for the sprite.
};
#endif