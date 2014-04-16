/*
=================
cButton.cpp
- Header file for class definition - IMPLEMENTATION
=================
*/
#include "cButton.h"
/*
=================
- Data constructor initializes the button to the data passed to 
- the constructor from the paramater sPosition.
- Is always called, thus ensures all sprite objects are in a consistent state.
- all methods are handles inside main as i wasnt able to figure out pointers and passing values correctly within the time limit
=================
*/
cButton::cButton(): cSprite() 			// Default constructor
{
}
cButton::cButton(D3DXVECTOR3 sPosition, LPDIRECT3DDEVICE9 pd3dDevice, LPCSTR theFilename): cSprite(sPosition,pd3dDevice,theFilename)  // Constructor
{
}
/*
=================
- update the position etc of the sprite.
=================
*/
void cButton::update(float deltaTime) 					// update the balloons position based on the translation
{
	cSprite::update();
}

