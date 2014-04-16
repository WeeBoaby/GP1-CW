/*
=================
cHoldButton.cpp
- Header file for class definition - IMPLEMENTATION
=================
*/
#include "cHoldButton.h"
/*
=================
- Data constructor initializes the cBalloon to the data passed to 
- the constructor from the paramater sPosition.
- Is always called, thus ensures all sprite objects are in a consistent state.
-button for holding the reels has methods to update and do decide if it can hold
=================
*/
cHoldButton::cHoldButton(): cSprite() 			// Default constructor
{
	/* initialize random seed: */
	srand ( (unsigned int)time(NULL) );
}
cHoldButton::cHoldButton(D3DXVECTOR3 sPosition, LPDIRECT3DDEVICE9 pd3dDevice, LPCSTR theFilename): cSprite(sPosition,pd3dDevice,theFilename)  // Constructor
{
	/* initialize random seed: */
	srand ( (unsigned int)time(NULL) );
}
/*
=================
- update the position etc of the sprite.
=================
*/
void cHoldButton::update(float deltaTime) 					// update the balloons position based on the translation
{
	cSprite::update();
}

//check if the payer is allowed to hold, 25% chances
int cHoldButton::CheckHold()
{
	int holdNo;
	holdNo = rand()%100+1;
	return holdNo;
}

