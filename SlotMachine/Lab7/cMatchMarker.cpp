/*
=================
cMatchMarker.cpp
- Header file for class definition - IMPLEMENTATION
=================
*/
#include "cMatchMarker.h"
/*
=================
- Data constructor initializes the SpinBtn to the data passed to 
- the constructor from the paramater sPosition.
- Is always called, thus ensures all sprite objects are in a consistent state.
=================
*/
cMatchMarker::cMatchMarker(): cSprite() 			// Default constructor
{
}
cMatchMarker::cMatchMarker(D3DXVECTOR3 sPosition, LPDIRECT3DDEVICE9 pd3dDevice, LPCSTR theFilename): cSprite(sPosition,pd3dDevice,theFilename)  // Constructor
{
}
/*
=================
- update the position etc of the sprite.
=================
*/
void cMatchMarker::update(float deltaTime) 					// update the sprites position based on the match
{
	cSprite::update();
}

