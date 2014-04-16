/*
=================
cReel.cpp
- Header file for class definition - IMPLEMENTATION
=================
*/
#include "cReel.h"

cReel::cReel(): cSprite() 			// Default constructor
{
	/* initialize random seed: */
	srand ( (unsigned int)time(NULL) );
}
cReel::cReel(D3DXVECTOR3 sPosition, LPDIRECT3DDEVICE9 pd3dDevice, LPCSTR theFilename): cSprite(sPosition,pd3dDevice,theFilename)  // Constructor
{
	/* initialize random seed: */
	srand ( (unsigned int)time(NULL) );
}
/*
=================
- update the position etc of the sprite.
=================
*/
void cReel::update(float deltaTime) 					// update the balloons position based on the translation
{
	cSprite::update();
}

//choose fruit for reels takes a rand of the 4 fruit and returns that rand
int cReel::ChooseFruit()
{
	int fruitNo;
	fruitNo = rand()%4;
	return fruitNo;
}

