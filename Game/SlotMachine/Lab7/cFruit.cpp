/*
=================
cBalloon.cpp
- Header file for class definition - IMPLEMENTATION
=================
*/
#include "cFruit.h"
/*
=================
- Data constructor initializes the cBalloon to the data passed to 
- the constructor from the paramater sPosition.
- Is always called, thus ensures all sprite objects are in a consistent state.
=================
*/
cFruit::cFruit(): cSprite() 			// Default constructor
{
}
cFruit::cFruit(D3DXVECTOR3 sPosition, LPDIRECT3DDEVICE9 pd3dDevice, LPCSTR theFilename): cSprite(sPosition,pd3dDevice,theFilename)  // Constructor
{
}
/*
=================
- update the position etc of the sprite.
=================
*/
void cFruit::update(float deltaTime) 					// update the balloons position based on the translation
{
	D3DXVECTOR2 tempv2;
	D3DXVECTOR3 tempv3;
	tempv2 = cFruit::getSpritePos2D() + (cFruit::getTranslation())*deltaTime;
	tempv3 = D3DXVECTOR3(tempv2.x, tempv2.y, 0.0f);
	cFruit::setSpritePos(tempv3);
	cSprite::update();
}
/*
=================
- Returns the current direction of the sprite.
=================
*/
eDirection cFruit::getDirection()       // Rturns the current direction of the sprite
{
	return cFruit::mDirection;
}
/*
=================
- Sets the current direction for the sprite.
=================
*/
void cFruit::setDirection(eDirection theDirection)  // Sets the current direction for the sprite.
{
	cFruit::mDirection = theDirection;
}

