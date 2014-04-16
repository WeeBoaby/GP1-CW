/*
=================
cMatchMarker.h
- Header file for class definition - SPECIFICATION
- Header file for the Sprite class
=================
*/
#ifndef _CMATCHMARKER_H
#define _CMATCHMARKER_H
#include "cSprite.h"

class cMatchMarker : public cSprite
{
private:

public:
	cMatchMarker();
	cMatchMarker(D3DXVECTOR3 sPosition, LPDIRECT3DDEVICE9 pd3dDevice, LPCSTR theFilename); // Constructor
	void update(float deltaTime);					// update the markers position based on the current match
};
#endif

