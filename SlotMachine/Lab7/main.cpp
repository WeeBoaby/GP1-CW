/*
=================
main.cpp
Main entry point for the Card application
=================
*/

#pragma region includes
#include "GameConstants.h"
#include "GameResources.h"
#include "cD3DManager.h"
#include "cD3DXSpriteMgr.h"
#include "cD3DXTexture.h"
#include "cFruit.h"
#include "cSprite.h"
#include "cExplosion.h"
#include "cXAudio.h"
#include "cD3DXFont.h"
#include "cButton.h"
#include "cReel.h"
#include "cHoldButton.h"
#include "cMatchMarker.h"
#pragma endregion

using namespace std;

HINSTANCE hInst; // global handle to hold the application instance
HWND wndHandle; // global variable to hold the window handle

// Get a reference to the DirectX Manager
static cD3DManager* d3dMgr = cD3DManager::getInstance();

// Get a reference to the DirectX Sprite renderer Manager 
static cD3DXSpriteMgr* d3dxSRMgr = cD3DXSpriteMgr::getInstance();

//currState is going to control the state of the game, if will change dependant on the current state.
//the int that curr state is at will determine the update and draw functions as well as input.
int currState = 0; // states == 0 = menu, 1 = game, 2 = end game, 3 = quitting
//this bool will control the sounds playing or not
bool isMuted = false;

#pragma region Creation of Objects and Variables for my game

//create an instance of each reel
cReel reel1, reel2, reel3;

//create instance of button
cButton spinButton, bank1Button, bank5Button, insert1Button, insert5Button, unholdButton;

//create the hold buttons
cHoldButton holdReel1, holdReel2, holdReel3;

// create the box that goes around the matching fruit
cMatchMarker matchMarker;

//bools for the game
//bool to check if button pressed
bool buttonPressed = false;
//bool to check if player is allowed to hold this turn
bool allowedToHold = false;
//bools for each reel to tell if theyre held.
bool reel1Held = false;
bool reel2Held = false;
bool reel3Held = false;
//bools to say if there is a match and what fruit has matched
bool appleMatch = false;
bool orangeMatch = false;
bool grapeMatch = false;
bool pearMatch = false;
//bool for if the player has credits or not
bool hasCredits = true;
//your starting credits and the char for the credits text
int credits = 10;
char creditsStr[50];
//your bank and the char for the bank text
int bank = 0;
char bankStr[50];
//position of the reels
D3DXVECTOR3 reel1pos;
D3DXVECTOR3 reel2pos;
D3DXVECTOR3 reel3pos;
//int to store the current value of the fruit on the reel
int reel1Fruit, reel2Fruit, reel3Fruit;
//list of the animations for the slots rotating
list<cExplosion*> gReelAnim;
//create vector to contain the fruit that will be colliding with each other
D3DXVECTOR2 fruitTrans = D3DXVECTOR2(300,300);
vector<cFruit*> colFruit;
vector<cFruit*>::iterator iter;
vector<cFruit*>::iterator index;

#pragma endregion

#pragma region Creation of objects and variables for menu

//create instance of the buttons
cButton playButton, quitButton, helpButton;

#pragma endregion

#pragma region create objects and variables for the end game screen

//create the 2 buttons
cButton continueButton, endButton;

#pragma endregion

#pragma region create objects and variables for the game over screen

//create the menu button
cButton menuButton;
//create the final score string to display at end of game
int finalScore = 0;
char scoreString[50];

#pragma endregion
#pragma region Creation of objects and variables for help screen

//create instance of the buttons
cButton startButton;

#pragma endregion

RECT clientBounds;

TCHAR szTempOutput[30];

//my sounds
cXAudio gSpinSound;
cXAudio gBGSound;
cXAudio gFanfare;
cXAudio gButton;

//array containing every image that is used in my game
cD3DXTexture* GameTextures[22];
char* GameTxtres[] = {"Images\\Apple.png","Images\\Orange.png","Images\\Pear.png","Images\\Grape.png", "Images\\SlotAnim.png",
	"Images\\SpinBTN.png", "Images\\Insert1.png", "Images\\Insert5.png", "Images\\Bank1.png", "Images\\Bank5.png", 
	"Images\\HoldButton.png", "Images\\MatchMarker.png", "Images\\HeldButton.png", "Images\\UnholdButton.png",
	"Images\\MuteButton.png", "Images\\PlayButton.png", "Images\\QuitButton.png", "Images\\ContinueButton.png", "Images\\EndGameButton.png",
	"Images\\MenuButton.png", "Images\\explosion.png", "Images\\3Match.png"};

//forward declerations	
#pragma region ForwardDeclerations

void ChooseFruitForReels();
void CheckIfFruitMatch();
void DrawMyButtons();
void DrawHoldButtons();
void DrawMyReels();
void DecideHold();
void DrawMatchMarkers();
void UnholdReels();
void CheckCredits(int currCredits);
void ChangeStates(int newState);
void CreateFruitForCollision();

#pragma endregion
/*
==================================================================
* LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
* LPARAM lParam)
* The window procedure
==================================================================
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Check any available messages from the queue
	switch (message)
	{

#pragma region mouse button click
	case WM_LBUTTONDOWN:
		{

#pragma region get mouse point

			POINT mouseXY;
			mouseXY.x = LOWORD(lParam);
			mouseXY.y = HIWORD(lParam);

#pragma endregion

#pragma region buttons for state 0

			//menu state buttons
			if (currState == 0)
			{
				if(playButton.insideRect(playButton.getBoundingRect(), mouseXY) || wParam == VK_LEFT)
				{
					//change to the play state
					//choose a fruit but DO NOT test if matched, we dont want to give the player credits
					//makes the reels random every time at start
					gButton.playSound(L"Sounds\\Button.wav",false);
					ChangeStates(1);
					ChooseFruitForReels();
				}
				if(quitButton.insideRect(quitButton.getBoundingRect(), mouseXY))
				{
					//guit the game, sends exit success to the exit function in std(i think)
					gButton.playSound(L"Sounds\\Button.wav",false);
					exit(EXIT_SUCCESS);
				}
				if(helpButton.insideRect(helpButton.getBoundingRect(), mouseXY))
				{
					gButton.playSound(L"Sounds\\Button.wav",false);
					ChangeStates(4);
				}

			}
#pragma endregion

#pragma region buttons for state 1
			if (currState == 1) // playing game
			{
				//set pos for reels so that animation can play
				reel1pos = reel1.getSpritePos();
				reel2pos = reel2.getSpritePos();
				reel3pos = reel3.getSpritePos();

				//check if held before checking if spin is pressed or it wont hold properly
				if(allowedToHold == true)
				{
					if(holdReel1.insideRect(holdReel1.getBoundingRect(), mouseXY))
					{
						//holds the first reel
						reel1Held = true;
					}
					if(holdReel2.insideRect(holdReel2.getBoundingRect(), mouseXY))
					{
						//holds the second reel
						reel2Held = true;
					}
					if(holdReel3.insideRect(holdReel3.getBoundingRect(), mouseXY))
					{
						//hold the third reel
						reel3Held = true;
					}
					if(unholdButton.insideRect(unholdButton.getBoundingRect(), mouseXY))
					{
						//unholds all reels
						UnholdReels();
					}
				}

				if(spinButton.insideRect(spinButton.getBoundingRect(), mouseXY) && credits > 0)
				{
					gButton.playSound(L"Sounds\\Button.wav",false);
					//sets all matches to false, takes a credit from player.
					//creates the animations that play over changing reels
					//plays reel sound
					//chooses new fruit and then checks if there are any matches
					//decide if player will be allowed to hold next turn
					//sets the current turns hold values all back to unheld
					orangeMatch = false;
					grapeMatch = false;
					pearMatch = false;
					appleMatch = false;
					buttonPressed = true;
					credits--;
					if(reel1Held == false)
						gReelAnim.push_back(new cExplosion(reel1pos, GameTextures[4]));
					if(reel2Held == false)
						gReelAnim.push_back(new cExplosion(reel2pos, GameTextures[4]));
					if(reel3Held == false)
						gReelAnim.push_back(new cExplosion(reel3pos, GameTextures[4]));
					gSpinSound.playSound(L"Sounds\\spin.wav",false);

					sprintf_s( creditsStr, 50, "Credits : %d", credits);
					ChooseFruitForReels();
					CheckIfFruitMatch();
					DecideHold();
					reel1Held = false;
					reel2Held = false;
					reel3Held = false;

				}
				//tkaes 1 from credits and puts in bank
				if(bank1Button.insideRect(bank1Button.getBoundingRect(), mouseXY) && credits > 0)
				{
					gButton.playSound(L"Sounds\\Button.wav",false);
					credits --;
					sprintf_s( creditsStr, 50, "Credits : %d", credits);
					bank ++;
					sprintf_s( bankStr, 50, "Bank : %d", bank);
				}
				//takes 5 from credits and puts in bank
				if(bank5Button.insideRect(bank5Button.getBoundingRect(), mouseXY) && credits > 4)
				{
					gButton.playSound(L"Sounds\\Button.wav",false);
					credits -= 5;
					sprintf_s( creditsStr, 50, "Credits : %d", credits);
					bank += 5;
					sprintf_s( bankStr, 50, "Bank : %d", bank);
				}
				//takes 1 from bank and puts in credits
				if(insert1Button.insideRect(insert1Button.getBoundingRect(), mouseXY) && bank > 0)
				{
					gButton.playSound(L"Sounds\\Button.wav",false);
					bank --;
					sprintf_s( bankStr, 50, "Bank : %d", bank);
					credits ++;
					sprintf_s( creditsStr, 50, "Credits : %d", credits);
				}
				//takes 5 from bank and puts in credits
				if(insert5Button.insideRect(insert5Button.getBoundingRect(), mouseXY) && bank > 4)
				{
					gButton.playSound(L"Sounds\\Button.wav",false);
					bank -= 5;
					sprintf_s( bankStr, 50, "Bank : %d", bank);
					credits += 5;
					sprintf_s( creditsStr, 50, "Credits : %d", credits);
				}
			}
#pragma endregion

#pragma region buttons for state 2
			//check if the end game button or the continue button is pressed
			if (currState == 2)
			{
				if(endButton.insideRect(endButton.getBoundingRect(), mouseXY))
				{
					//moves to game over screen
					gButton.playSound(L"Sounds\\Button.wav",false);
					ChangeStates(3);
				}
				if(continueButton.insideRect(continueButton.getBoundingRect(), mouseXY) && bank > 0)
				{
					//inserts a new credit from bank, updates values and sets back to the game state
										gButton.playSound(L"Sounds\\Button.wav",false);
					bank--;
					sprintf_s( bankStr, 50, "Bank : %d", bank);
					credits++;
					sprintf_s( creditsStr, 50, "Credits : %d", credits);
					ChangeStates(1);
				}
			}
#pragma endregion

#pragma region buttons for state 3

			//check if the menu button is pressed in the game over screen
			if (currState == 3)
			{
				if(menuButton.insideRect(menuButton.getBoundingRect(), mouseXY))
				{
					//make sure to reset the credits etc when back to menu
										gButton.playSound(L"Sounds\\Button.wav",false);
					ChangeStates(0);
					bank = 0;
					sprintf_s( bankStr, 50, "Bank : %d", bank);
					credits = 10;
					sprintf_s( creditsStr, 50, "Credits : %d", credits);
				}
			}
#pragma endregion

#pragma region buttons for state 4
			//help state buttons
			if (currState == 4)
			{
				if(startButton.insideRect(startButton.getBoundingRect(), mouseXY))
				{
					//change to the play state
					//choose a fruit but DO NOT test if matched, we dont want to give the player credits
					//makes the reels random every time at start
										gButton.playSound(L"Sounds\\Button.wav",false);
					ChangeStates(1);
					ChooseFruitForReels();
				}

			}
#pragma endregion

			buttonPressed = false;
			return 0;
		}
#pragma endregion
#pragma region keyboard buttons pressed
		case WM_KEYDOWN:
		{
#pragma region buttons for state 0

			//menu state buttons
			if (currState == 0)
			{
				if(wParam == 'P')
				{
					//change to the play state
					//choose a fruit but DO NOT test if matched, we dont want to give the player credits
					//makes the reels random every time at start
					gButton.playSound(L"Sounds\\Button.wav",false);
					ChangeStates(1);
					ChooseFruitForReels();
				}
				if(wParam == 'Q')
				{
					//guit the game, sends exit success to the exit function in std(i think)
					gButton.playSound(L"Sounds\\Button.wav",false);
					exit(EXIT_SUCCESS);
				}
				if(wParam == 'H')
				{
					gButton.playSound(L"Sounds\\Button.wav",false);
					ChangeStates(4);
				}

			}
#pragma endregion

#pragma region buttons for state 1
			if (currState == 1) // playing game
			{
				//set pos for reels so that animation can play
				reel1pos = reel1.getSpritePos();
				reel2pos = reel2.getSpritePos();
				reel3pos = reel3.getSpritePos();

				//check if held before checking if spin is pressed or it wont hold properly
				if(allowedToHold == true)
				{
					if(wParam == '1')
					{
						//holds the first reel
						reel1Held = true;
					}
					if(wParam == '2')
					{
						//holds the second reel
						reel2Held = true;
					}
					if(wParam == '3')
					{
						//hold the third reel
						reel3Held = true;
					}
					if(wParam == '0')
					{
						//unholds all reels
						UnholdReels();
					}
				}

				if(wParam == VK_RETURN)
				{
					gButton.playSound(L"Sounds\\Button.wav",false);
					//sets all matches to false, takes a credit from player.
					//creates the animations that play over changing reels
					//plays reel sound
					//chooses new fruit and then checks if there are any matches
					//decide if player will be allowed to hold next turn
					//sets the current turns hold values all back to unheld
					orangeMatch = false;
					grapeMatch = false;
					pearMatch = false;
					appleMatch = false;
					buttonPressed = true;
					credits--;
					if(reel1Held == false)
						gReelAnim.push_back(new cExplosion(reel1pos, GameTextures[4]));
					if(reel2Held == false)
						gReelAnim.push_back(new cExplosion(reel2pos, GameTextures[4]));
					if(reel3Held == false)
						gReelAnim.push_back(new cExplosion(reel3pos, GameTextures[4]));
					gSpinSound.playSound(L"Sounds\\spin.wav",false);

					sprintf_s( creditsStr, 50, "Credits : %d", credits);
					ChooseFruitForReels();
					CheckIfFruitMatch();
					DecideHold();
					reel1Held = false;
					reel2Held = false;
					reel3Held = false;
				}
			}
#pragma endregion

#pragma region buttons for state 2
			//check if the end game button or the continue button is pressed
			if (currState == 2)
			{
				if(wParam == 'Q')
				{
					//moves to game over screen
					gButton.playSound(L"Sounds\\Button.wav",false);
					ChangeStates(3);
				}
				if(wParam == 'P')
				{
					//inserts a new credit from bank, updates values and sets back to the game state
										gButton.playSound(L"Sounds\\Button.wav",false);
					bank--;
					sprintf_s( bankStr, 50, "Bank : %d", bank);
					credits++;
					sprintf_s( creditsStr, 50, "Credits : %d", credits);
					ChangeStates(1);
				}
			}
#pragma endregion

#pragma region buttons for state 3

			//check if the menu button is pressed in the game over screen
			if (currState == 3)
			{
				if(wParam == 'M')
				{
					//make sure to reset the credits etc when back to menu
										gButton.playSound(L"Sounds\\Button.wav",false);
					ChangeStates(0);
					bank = 0;
					sprintf_s( bankStr, 50, "Bank : %d", bank);
					credits = 10;
					sprintf_s( creditsStr, 50, "Credits : %d", credits);
				}
			}
#pragma endregion

#pragma region buttons for state 4
			//help state buttons
			if (currState == 4)
			{
				if(wParam == 'P')
				{
					//change to the play state
					//choose a fruit but DO NOT test if matched, we dont want to give the player credits
					//makes the reels random every time at start
										gButton.playSound(L"Sounds\\Button.wav",false);
					ChangeStates(1);
					ChooseFruitForReels();
				}

			}
#pragma endregion

			buttonPressed = false;
			return 0;
		}
				case WM_CLOSE:
			{
			// Exit the Game
				PostQuitMessage(0);
				 return 0;
			}

		case WM_DESTROY:
			{
				PostQuitMessage(0);
				return 0;
			}
#pragma endregion
	}
	// Always return the message to the default window
	// procedure for further processing
	return DefWindowProc(hWnd, message, wParam, lParam);
}

/*
==================================================================
* bool initWindow( HINSTANCE hInstance )
* initWindow registers the window class for the application, creates the window
==================================================================
*/
bool initWindow( HINSTANCE hInstance )
{
	WNDCLASSEX wcex;
	// Fill in the WNDCLASSEX structure. This describes how the window
	// will look to the system
	wcex.cbSize = sizeof(WNDCLASSEX); // the size of the structure
	wcex.style = CS_HREDRAW | CS_VREDRAW; // the class style
	wcex.lpfnWndProc = (WNDPROC)WndProc; // the window procedure callback
	wcex.cbClsExtra = 0; // extra bytes to allocate for this class
	wcex.cbWndExtra = 0; // extra bytes to allocate for this instance
	wcex.hInstance = hInstance; // handle to the application instance
	wcex.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_MyWindowIcon)); // icon to associate with the application
	wcex.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_GUNSIGHT));// the default cursor
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); // the background color
	wcex.lpszMenuName = NULL; // the resource name for the menu
	wcex.lpszClassName = "Slots"; // the class name being created
	wcex.hIconSm = LoadIcon(hInstance,"Balloon.ico"); // the handle to the small icon

	RegisterClassEx(&wcex);
	// Create the window
	wndHandle = CreateWindow("Slots",			// the window class to use
		"Slot Machine",			// the title bar text
		WS_OVERLAPPEDWINDOW,	// the window style
		CW_USEDEFAULT, // the starting x coordinate
		CW_USEDEFAULT, // the starting y coordinate
		800, // the pixel width of the window
		600, // the pixel height of the window
		NULL, // the parent window; NULL for desktop
		NULL, // the menu for the application; NULL for none
		hInstance, // the handle to the application instance
		NULL); // no values passed to the window
	// Make sure that the window handle that is created is valid
	if (!wndHandle)
		return false;
	// Display the window on the screen
	ShowWindow(wndHandle, SW_SHOW);
	UpdateWindow(wndHandle);
	return true;
}

/*
==================================================================
// This is winmain, the main entry point for Windows applications
==================================================================
*/
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
#pragma region initialise and create window stuff
	// Initialize the window
	if ( !initWindow( hInstance ) )
		return false;
	// called after creating the window
	if ( !d3dMgr->initD3DManager(wndHandle) )
		return false;
	if ( !d3dxSRMgr->initD3DXSpriteMgr(d3dMgr->getTheD3DDevice()))
		return false;

#pragma endregion

#pragma region highdeftimer and framerate stuff
	// Grab the frequency of the high def timer
	__int64 freq = 0;				// measured in counts per second;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	float sPC = 1.0f / (float)freq;			// number of seconds per count

	__int64 currentTime = 0;				// current time measured in counts per second;
	__int64 previousTime = 0;				// previous time measured in counts per second;

	float numFrames   = 0.0f;				// Used to hold the number of frames
	float timeElapsed = 0.0f;				// cumulative elapsed time

	GetClientRect(wndHandle,&clientBounds);

	float fpsRate = 1.0f/25.0f;
#pragma endregion

	//initialise the text and set initial values
	sprintf_s( creditsStr, 50, "Credits : %d", credits);
	sprintf_s( bankStr, 50, "Bank : %d", bank);

	//====================================================================
	// Load all of my game textures in and ready them for use
	//====================================================================
	for (int txture = 0; txture < 22; txture++)
	{
		GameTextures[txture] = new cD3DXTexture(d3dMgr->getTheD3DDevice(), GameTxtres[txture]);
	}

#pragma region  create vector3s for play scene
	//positions of reels
	D3DXVECTOR3 reel1Pos;
	D3DXVECTOR3 reel2Pos;
	D3DXVECTOR3 reel3Pos;
	//spin button v3
	D3DXVECTOR3 spinBtnPos;
	//insert/bank button v3s
	D3DXVECTOR3 insert1ButtonPos;
	D3DXVECTOR3 insert5ButtonPos;
	D3DXVECTOR3 bank1ButtonPos;
	D3DXVECTOR3 bank5ButtonPos;
	//hold buttons v3s
	D3DXVECTOR3 holdReel1Pos;
	D3DXVECTOR3 holdReel2Pos;
	D3DXVECTOR3 holdReel3Pos;
	D3DXVECTOR3 unholdButtonPos;
	//v3 for match marker
	D3DXVECTOR3 matchMarkerPos;
	//v3 that stores current pos of moving colliding sprites
	D3DXVECTOR3 afruitPos;
#pragma endregion

#pragma region create vector3s for menu
	//buttons positions for the menu
	D3DXVECTOR3 playPos;
	D3DXVECTOR3 quitPos;
	D3DXVECTOR3 helpPos;
#pragma endregion

#pragma region create vector3s for continue screen
	//vector 3's for the continue and end game buttons
	D3DXVECTOR3 continuePos;
	D3DXVECTOR3 endPos;
#pragma endregion

#pragma region create vector3s for game over screen
	//vector 3 for the menu button
	D3DXVECTOR3 menuPos;
#pragma endregion

	#pragma region create vector3s for help screen
	//buttons positions for the help screen
	D3DXVECTOR3 startPos;
#pragma endregion

	/* initialize random seed: */
	srand ( (unsigned int)time(NULL) );

#pragma region initialise objects for menu

	//initialise play button
	playPos = D3DXVECTOR3(50,250,0);
	playButton.setSpritePos(playPos);
	playButton.setTexture(GameTextures[15]);

	//initialise quit button
	quitPos = D3DXVECTOR3(50,350,0);
	quitButton.setSpritePos(quitPos);
	quitButton.setTexture(GameTextures[16]);

	//initialise mute button
	helpPos = D3DXVECTOR3(50,450,0);
	helpButton.setSpritePos(helpPos);
	helpButton.setTexture(GameTextures[14]);

#pragma endregion

#pragma region initialise my objects for the game scene
	//initialise spin button
	spinBtnPos = D3DXVECTOR3(695,10,0);
	spinButton.setSpritePos(spinBtnPos);
	spinButton.setTexture(GameTextures[5]);

	//initialise bank 1 button
	bank1ButtonPos = D3DXVECTOR3(695,110,0);
	bank1Button.setSpritePos(bank1ButtonPos);
	bank1Button.setTexture(GameTextures[8]);

	//initialise bank 5 button
	bank5ButtonPos = D3DXVECTOR3(695,210,0);
	bank5Button.setSpritePos(bank5ButtonPos);
	bank5Button.setTexture(GameTextures[9]);

	//initialise insert 1 button
	insert1ButtonPos = D3DXVECTOR3(695,310,0);
	insert1Button.setSpritePos(insert1ButtonPos);
	insert1Button.setTexture(GameTextures[6]);

	//initialise insert 5 button
	insert5ButtonPos = D3DXVECTOR3(695,410,0);
	insert5Button.setSpritePos(insert5ButtonPos);
	insert5Button.setTexture(GameTextures[7]);

	//initialise reel1
	reel1Pos = D3DXVECTOR3(35,115,0);
	reel1.setSpritePos(reel1Pos);
	reel1.setTexture(GameTextures[1]);

	//initialise reel2
	reel2Pos = D3DXVECTOR3(262,115,0);
	reel2.setSpritePos(reel2Pos);
	reel2.setTexture(GameTextures[2]);

	//initialise reel3
	reel3Pos = D3DXVECTOR3(490,115,0);
	reel3.setSpritePos(reel3Pos);
	reel3.setTexture(GameTextures[3]);

	//initialise reel1hold
	holdReel1Pos = D3DXVECTOR3(35,310,0);
	holdReel1.setSpritePos(holdReel1Pos);
	holdReel1.setTexture(GameTextures[10]);

	//initialise reel2hold
	holdReel2Pos = D3DXVECTOR3(262,310,0);
	holdReel2.setSpritePos(holdReel2Pos);
	holdReel2.setTexture(GameTextures[10]);

	//initialise reel3hold
	holdReel3Pos = D3DXVECTOR3(490,310,0);
	holdReel3.setSpritePos(holdReel3Pos);
	holdReel3.setTexture(GameTextures[10]);

	//initialise unhold button
	unholdButtonPos = D3DXVECTOR3(700,500,0);
	unholdButton.setSpritePos(unholdButtonPos);
	unholdButton.setTexture(GameTextures[13]);

	//initialise match Marker
	matchMarkerPos = D3DXVECTOR3(45,385,0);
	matchMarker.setSpritePos(matchMarkerPos);
	matchMarker.setTexture(GameTextures[11]);

	//call method that created the bouncy sprites
	CreateFruitForCollision();

#pragma endregion

#pragma region initialise objects for continuescreen

	//initialise play button
	continuePos = D3DXVECTOR3(0,0,0);
	continueButton.setSpritePos(continuePos);
	continueButton.setTexture(GameTextures[17]);

	//initialise quit button
	endPos = D3DXVECTOR3(0,300,0);
	endButton.setSpritePos(endPos);
	endButton.setTexture(GameTextures[18]);

#pragma endregion

#pragma region initialise objects for game over screen

	//initialise play button
	menuPos = D3DXVECTOR3(600,500,0);
	menuButton.setSpritePos(menuPos);
	menuButton.setTexture(GameTextures[19]);

#pragma endregion

#pragma region initialise objects for help screen

	//initialise play button
	startPos = D3DXVECTOR3(50,250,0);
	startButton.setSpritePos(startPos);
	startButton.setTexture(GameTextures[15]);
#pragma endregion

	//play background music here must be outside game loop or play will be called every update
	if(!isMuted)
		gBGSound.playSound(L"Sounds\\Pamgaea.wav",true);

#pragma region aSurface and BackBuffer
	LPDIRECT3DSURFACE9 menuSurface;				// the main menu surface
	LPDIRECT3DSURFACE9 gameSurface;				// the game surface
	LPDIRECT3DSURFACE9 quitSurface;				// the quit surface
	LPDIRECT3DSURFACE9 helpSurface;				// the main menu surface
	LPDIRECT3DSURFACE9 theBackbuffer = NULL;  // This will hold the back buffer
#pragma endregion

	MSG msg;
	ZeroMemory( &msg, sizeof( msg ) );

#pragma region set background
	// Create the background surface for game
	gameSurface = d3dMgr->getD3DSurfaceFromFile("Images\\Background.png");
	// Create the background surface for menu
	menuSurface = d3dMgr->getD3DSurfaceFromFile("Images\\MenuBG.png");
	// Create the background surface for game over
	quitSurface = d3dMgr->getD3DSurfaceFromFile("Images\\GameOver.png");
		// Create the background surface for help screen
	helpSurface = d3dMgr->getD3DSurfaceFromFile("Images\\HelpScreen.png");

#pragma endregion

#pragma region font and text pos
	// load custom font
	cD3DXFont* gameFont = new cD3DXFont(d3dMgr->getTheD3DDevice(),hInstance, "Times New Roman");
	RECT creditTextPos;
	SetRect(&creditTextPos, 500, 10, 700, 50);

	//text rect for the bank stuff
	RECT bankTextPos;
	SetRect(&bankTextPos, 500, 50, 700, 90);
	//text rect for the final game over screen stuff
	RECT scoreTextPos;
	SetRect(&scoreTextPos, 150, 0, 700, 90);


#pragma endregion

	QueryPerformanceCounter((LARGE_INTEGER*)&previousTime);

	while( msg.message!=WM_QUIT )
	{
		// Check the message queue
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
			float dt = (currentTime - previousTime)*sPC;

			// Accumulate how much time has passed.
			timeElapsed += dt;

			//Update method is in here
			if(timeElapsed > fpsRate)
			{
#pragma region update methods

				//only update stuff if in the menu
				if (currState == 0)
				{
					playButton.update(timeElapsed);
					quitButton.update(timeElapsed);
					helpButton.update(timeElapsed);
				}

				//only update the stuff for the game screen no need to update when the stuff cant be interacted with
				if (currState == 1)
				{
					reel1.update(timeElapsed);
					reel2.update(timeElapsed);
					reel3.update(timeElapsed);
					spinButton.update(timeElapsed);
					bank1Button.update(timeElapsed);
					bank5Button.update(timeElapsed);
					insert1Button.update(timeElapsed);
					insert5Button.update(timeElapsed);
					holdReel1.update(timeElapsed);
					holdReel2.update(timeElapsed);
					holdReel3.update(timeElapsed);
					unholdButton.update(timeElapsed);
					matchMarker.update(timeElapsed);

					
				//always update the moving and colliding sprites, but only draw them when needed
				for(iter = colFruit.begin(); iter != colFruit.end(); ++iter)
				{
					if(currState == 1 && (reel1Fruit == reel2Fruit && reel1Fruit == reel3Fruit))
					{
						(*iter)->update(timeElapsed);			// update balloon
						afruitPos = (*iter)->getSpritePos();  // get the position of the current balloon

						//Check for boundry collision and change the sprite direction
						if (afruitPos.x > (clientBounds.right + 100) || (afruitPos.x < clientBounds.left - 100))
						{
							OutputDebugString("Collision!!");
							(*iter)->setTranslation((*iter)->getTranslation()*(-1));
						}

						//Check each against each other for collisions		
						for(index = colFruit.begin(); index != colFruit.end(); ++index)
						{
							if ((*iter)->collidedWith((*iter)->getBoundingRect(),(*index)->getBoundingRect()))
							{
								// if a collision occurs change the direction of the sprite that has collided
								OutputDebugString("Collision!!");
								(*iter)->setTranslation((*iter)->getTranslation()*(-1));
								(*index)->setTranslation((*index)->getTranslation()*(-1));
							}
						}
					}
				}

				}
				//only update the continue screen stuff, prevents player pressing spin button when not able
				if (currState == 2)
				{
					continueButton.update(timeElapsed);

					endButton.update(timeElapsed);
				}

				//only update the game over stuff
				if (currState == 3)
				{
					menuButton.update(timeElapsed);
					finalScore = bank;
					sprintf_s( scoreString, 50, "Your final score was : %d", bank);
				}
				//only update stuff if in the help screen
				if (currState == 4)
				{
					startButton.update(timeElapsed);
				}

#pragma endregion

				//sets the position of the marker that shows the player if there is a match currently
#pragma region change match marker position
				if(appleMatch == true)
				{
					matchMarkerPos = D3DXVECTOR3(45,385,0);
					matchMarker.setSpritePos(matchMarkerPos);
				}

				if(grapeMatch == true)
				{
					matchMarkerPos = D3DXVECTOR3(325,460,0);
					matchMarker.setSpritePos(matchMarkerPos);
				}

				if(orangeMatch == true)
				{
					matchMarkerPos = D3DXVECTOR3(45,450,0);
					matchMarker.setSpritePos(matchMarkerPos);
				}

				if(pearMatch == true)
				{
					matchMarkerPos = D3DXVECTOR3(325,385,0);
					matchMarker.setSpritePos(matchMarkerPos);
				}
#pragma endregion

				//sets the textures for the hold buttons if they are pressed, it informs the player
#pragma region held reel changes sprite
				if (reel1Held == true)
				{
					holdReel1.setTexture(GameTextures[12]);
				}
				if (reel2Held == true)
				{
					holdReel2.setTexture(GameTextures[12]);
				}
				if (reel3Held == true)
				{
					holdReel3.setTexture(GameTextures[12]);
				}

				if (reel1Held == false)
				{
					holdReel1.setTexture(GameTextures[10]);
				}
				if (reel2Held == false)
				{
					holdReel2.setTexture(GameTextures[10]);
				}
				if (reel3Held == false)
				{
					holdReel3.setTexture(GameTextures[10]);
				}
#pragma endregion

				//change the states if the credits have run out
				if (credits == 0 && currState == 1)
					ChangeStates(2);

				//render starts here
				d3dMgr->beginRender();
				theBackbuffer = d3dMgr->getTheBackBuffer();

				if (currState == 0) // draw the menu surface
					d3dMgr->updateTheSurface(menuSurface, theBackbuffer);
				if (currState == 1 || currState == 2) // draw the game surface
					d3dMgr->updateTheSurface(gameSurface, theBackbuffer);
				if (currState == 3) // draw the quit surface
					d3dMgr->updateTheSurface(quitSurface, theBackbuffer);
				if (currState == 4) // draw the help surface
					d3dMgr->updateTheSurface(helpSurface, theBackbuffer);

				d3dMgr->releaseTheBackbuffer(theBackbuffer);

				d3dxSRMgr->beginDraw();

				//custom method to draw reels (only used to tidy code)
				DrawMyReels();

				//draw the animations ove rthe slots to hide changing fruit
				list<cExplosion*>::iterator iter = gReelAnim.begin();
				while(iter != gReelAnim.end())
				{
					if((*iter)->isActive() == false)
					{
						iter = gReelAnim.erase(iter);
					}
					else
					{
						(*iter)->update(timeElapsed);
						d3dxSRMgr->setTheTransform((*iter)->getSpriteTransformMatrix());  
						d3dxSRMgr->drawSprite((*iter)->getTexture(),&((*iter)->getSourceRect()),NULL,NULL,0xFFFFFFFF);
						++iter;
					}
				}

				//custom method to draw buttons (only used to tidy code)
				DrawMyButtons();

				//draw the hold buttons if allowed to hold reel
				if (allowedToHold == true)
					DrawHoldButtons();

				//only display marker if there is a match
				if(appleMatch == true || orangeMatch == true || pearMatch == true || grapeMatch == true)
					DrawMatchMarkers();

				//only draw the bouncy thingys if there is a 3 match currently
				if(currState == 1 && (reel1Fruit == reel2Fruit && reel1Fruit == reel3Fruit))
				{
				vector<cFruit*>::iterator iterB = colFruit.begin();
				for(iterB = colFruit.begin(); iterB != colFruit.end(); ++iterB)
				{
						d3dxSRMgr->setTheTransform((*iterB)->getSpriteTransformMatrix());  
						d3dxSRMgr->drawSprite((*iterB)->getTexture(),NULL,NULL,NULL,0xFFFFFFFF);
				}
				}

				d3dxSRMgr->endDraw();

				//make sure t only draw the credits and bank during the contine and the game screen
				if (currState == 1 || currState == 2)
				{
					gameFont->printText(creditsStr,creditTextPos);
					gameFont->printText(bankStr,bankTextPos);
				}

				//we only want to draw the text for the players final score in the game over screen
				if (currState == 3)
				{
					gameFont->printText(scoreString,scoreTextPos);
				}

				d3dMgr->endRender();
				timeElapsed = 0.0f;
			}

			previousTime = currentTime;
		}
	}
	d3dxSRMgr->cleanUp();
	d3dMgr->clean();
	return (int) msg.wParam;
}

//method for making the reels choose their new fruit, sets image nothing else
//chooses the fruit for the reel and sets it
void ChooseFruitForReels()
{
	if (reel1Held == false)
	{
		int newFruit1 = reel1.ChooseFruit();
		reel1.setTexture(GameTextures[newFruit1]);
		reel1Fruit = newFruit1;
	}

	if (reel2Held == false)
	{
		int newFruit2 = reel2.ChooseFruit();
		reel2.setTexture(GameTextures[newFruit2]);
		reel2Fruit = newFruit2;
	}

	if (reel3Held == false)
	{
		int newFruit3 = reel3.ChooseFruit();
		reel3.setTexture(GameTextures[newFruit3]);
		reel3Fruit = newFruit3;
	}
}

//method that checks if there are any matches in the reels
//first check if the first 2 match, then checks if the first matches with third
void CheckIfFruitMatch()
{
	//if only the first 2 reels match
#pragma region 2 reels match
	if (reel1Fruit == reel2Fruit && reel1Fruit != reel3Fruit)
	{
							gFanfare.playSound(L"Sounds\\Fanfare.wav",false);
		//if apples
		if (reel1Fruit ==0)
		{
			appleMatch = true;
			bank++;
			sprintf_s( bankStr, 50, "Bank : %d", bank);
		}
		//if oranges
		if (reel1Fruit ==1)
		{
			orangeMatch = true;
			bank+=2;
			sprintf_s( bankStr, 50, "Bank : %d", bank);
		}
		//if pears
		if (reel1Fruit ==2)
		{
			pearMatch = true;
			bank+=3;
			sprintf_s( bankStr, 50, "Bank : %d", bank);
		}
		//if grapes
		if (reel1Fruit ==3)
		{
			grapeMatch = true;
			bank+=4;
			sprintf_s( bankStr, 50, "Bank : %d", bank);
		}
	}
#pragma endregion
	//if all 3 of the reels match
#pragma region 3 reels match
	if (reel1Fruit == reel2Fruit && reel1Fruit == reel3Fruit)
	{
		gFanfare.playSound(L"Sounds\\Fanfare.wav",false);
		//if apples
		if (reel1Fruit ==0)
		{
			appleMatch = true;
			bank+=2;
			sprintf_s( bankStr, 50, "Bank : %d", bank);

		}
		//if oranges
		if (reel1Fruit ==1)
		{
			orangeMatch = true;
			bank+=4;
			sprintf_s( bankStr, 50, "Bank : %d", bank);
		}
		//if pears
		if (reel1Fruit ==2)
		{
			pearMatch = true;
			bank+=6;
			sprintf_s( bankStr, 50, "Bank : %d", bank);
		}
		//if grapes
		if (reel1Fruit ==3)
		{
			grapeMatch = true;
			bank+=8;
			sprintf_s( bankStr, 50, "Bank : %d", bank);

		}
	}
#pragma endregion
}

//method to tidy draw code, draws buttons
//draws all buttons excluding the hold buttons
void DrawMyButtons()
{
	if (currState == 0)
	{
		//draw the play button
		d3dxSRMgr->setTheTransform(playButton.getSpriteTransformMatrix());
		d3dxSRMgr->drawSprite(playButton.getTexture(),NULL,NULL,NULL,0xFFFFFFFF);
		//draw quit button
		d3dxSRMgr->setTheTransform(quitButton.getSpriteTransformMatrix());
		d3dxSRMgr->drawSprite(quitButton.getTexture(),NULL,NULL,NULL,0xFFFFFFFF);
		//draw mute button
		d3dxSRMgr->setTheTransform(helpButton.getSpriteTransformMatrix());
		d3dxSRMgr->drawSprite(helpButton.getTexture(),NULL,NULL,NULL,0xFFFFFFFF);
	}
	if (currState == 1 || currState == 2)
	{
		//draw the spin button
		d3dxSRMgr->setTheTransform(spinButton.getSpriteTransformMatrix());
		d3dxSRMgr->drawSprite(spinButton.getTexture(),NULL,NULL,NULL,0xFFFFFFFF);
		//draw bank 5 button
		d3dxSRMgr->setTheTransform(bank5Button.getSpriteTransformMatrix());
		d3dxSRMgr->drawSprite(bank5Button.getTexture(),NULL,NULL,NULL,0xFFFFFFFF);
		//draw bank 1 button
		d3dxSRMgr->setTheTransform(bank1Button.getSpriteTransformMatrix());
		d3dxSRMgr->drawSprite(bank1Button.getTexture(),NULL,NULL,NULL,0xFFFFFFFF);
		// draw insert 5 button
		d3dxSRMgr->setTheTransform(insert5Button.getSpriteTransformMatrix());
		d3dxSRMgr->drawSprite(insert5Button.getTexture(),NULL,NULL,NULL,0xFFFFFFFF);
		//draw insert 1 button
		d3dxSRMgr->setTheTransform(insert1Button.getSpriteTransformMatrix());
		d3dxSRMgr->drawSprite(insert1Button.getTexture(),NULL,NULL,NULL,0xFFFFFFFF);
	}

	if (currState == 2)
	{
		if (bank > 0)
		{
			//draw the continue button
			d3dxSRMgr->setTheTransform(continueButton.getSpriteTransformMatrix());
			d3dxSRMgr->drawSprite(continueButton.getTexture(),NULL,NULL,NULL,0xFFFFFFFF);
		}
		//draw end game button
		d3dxSRMgr->setTheTransform(endButton.getSpriteTransformMatrix());
		d3dxSRMgr->drawSprite(endButton.getTexture(),NULL,NULL,NULL,0xFFFFFFFF);

	}
	if (currState == 3)
	{
		//draw the menu button
		d3dxSRMgr->setTheTransform(menuButton.getSpriteTransformMatrix());
		d3dxSRMgr->drawSprite(menuButton.getTexture(),NULL,NULL,NULL,0xFFFFFFFF);
	}

		if (currState == 4)
	{
		//draw the play game button
		d3dxSRMgr->setTheTransform(playButton.getSpriteTransformMatrix());
		d3dxSRMgr->drawSprite(playButton.getTexture(),NULL,NULL,NULL,0xFFFFFFFF);
	}
}

//method to tidy draw code, draws reels
//draws the reels for the game, makes sure only drawing in the correct game state
void DrawMyReels()
{
	if (currState == 1 || currState == 2)
	{
		//draw the first reel
		d3dxSRMgr->setTheTransform(reel1.getSpriteTransformMatrix());
		d3dxSRMgr->drawSprite(reel1.getTexture(),NULL,NULL,NULL,0xFFFFFFFF);
		//draw the second reel
		d3dxSRMgr->setTheTransform(reel2.getSpriteTransformMatrix());
		d3dxSRMgr->drawSprite(reel2.getTexture(),NULL,NULL,NULL,0xFFFFFFFF);
		//draw the third reel
		d3dxSRMgr->setTheTransform(reel3.getSpriteTransformMatrix());
		d3dxSRMgr->drawSprite(reel3.getTexture(),NULL,NULL,NULL,0xFFFFFFFF);
	}
}

//method called to decide if the player will be allowed to hold
//hold is only allowed 25% of the time
void DecideHold()
{
	int allowedToHoldNo = holdReel1.CheckHold();
	if (allowedToHoldNo < 76)
	{
		allowedToHold = false;
	}
	if (allowedToHoldNo > 75)
	{
		allowedToHold = true;
	}

}

//method only called if holds should be active
void DrawHoldButtons()
{
	if (currState == 1 || currState == 2)
	{
		//draw the first hold button
		d3dxSRMgr->setTheTransform(holdReel1.getSpriteTransformMatrix());
		d3dxSRMgr->drawSprite(holdReel1.getTexture(),NULL,NULL,NULL,0xFFFFFFFF);
		//draw the second hold button
		d3dxSRMgr->setTheTransform(holdReel2.getSpriteTransformMatrix());
		d3dxSRMgr->drawSprite(holdReel2.getTexture(),NULL,NULL,NULL,0xFFFFFFFF);
		//draw the third hold button
		d3dxSRMgr->setTheTransform(holdReel3.getSpriteTransformMatrix());
		d3dxSRMgr->drawSprite(holdReel3.getTexture(),NULL,NULL,NULL,0xFFFFFFFF);

		//draw the un hold button
		d3dxSRMgr->setTheTransform(unholdButton.getSpriteTransformMatrix());
		d3dxSRMgr->drawSprite(unholdButton.getTexture(),NULL,NULL,NULL,0xFFFFFFFF);
	}
}

//method to draw the markers that spawn around fruits
void DrawMatchMarkers()
{
	if (currState == 1 || currState == 2)
	{
		//draw the first hold button
		d3dxSRMgr->setTheTransform(matchMarker.getSpriteTransformMatrix());
		d3dxSRMgr->drawSprite(matchMarker.getTexture(),NULL,NULL,NULL,0xFFFFFFFF);
	}
}

//method to un hold the reels
void UnholdReels()
{
	reel1Held = false;
	reel2Held = false;
	reel3Held = false;
}

//method to change the state of the game all it does is change an Int
void ChangeStates(int newState)
{
	currState = newState;
}

//creates the "matched 3" markers that bounce on the screen when there is a 3 match
void CreateFruitForCollision()
{
	D3DXVECTOR3 fruitPos;
	fruitPos = D3DXVECTOR3(0,10,0);
	colFruit.push_back(new cFruit());
	colFruit[0]->setSpritePos(fruitPos);
	colFruit[0]->setTranslation(D3DXVECTOR2(100.0f, 0.0f));
	colFruit[0]->setTexture(GameTextures[21]);

	fruitPos = D3DXVECTOR3(0,300,0);
	colFruit.push_back(new cFruit());
	colFruit[1]->setSpritePos(fruitPos);
	colFruit[1]->setTranslation(D3DXVECTOR2(100.0f, 0.0f));
	colFruit[1]->setTexture(GameTextures[21]);

	fruitPos = D3DXVECTOR3(0,500,0);
	colFruit.push_back(new cFruit());
	colFruit[2]->setSpritePos(fruitPos);
	colFruit[2]->setTranslation(D3DXVECTOR2(100.0f, 0.0f));
	colFruit[2]->setTexture(GameTextures[21]);

	fruitPos = D3DXVECTOR3(800,10,0);
	colFruit.push_back(new cFruit());
	colFruit[3]->setSpritePos(fruitPos);
	colFruit[3]->setTranslation(D3DXVECTOR2(-100.0f, 0.0f));
	colFruit[3]->setTexture(GameTextures[21]);

	fruitPos = D3DXVECTOR3(800,300,0);
	colFruit.push_back(new cFruit());
	colFruit[4]->setSpritePos(fruitPos);
	colFruit[4]->setTranslation(D3DXVECTOR2(-100.0f, 0.0f));
	colFruit[4]->setTexture(GameTextures[21]);

	fruitPos = D3DXVECTOR3(800,500,0);
	colFruit.push_back(new cFruit());
	colFruit[5]->setSpritePos(fruitPos);
	colFruit[5]->setTranslation(D3DXVECTOR2(-100.0f, 0.0f));
	colFruit[5]->setTexture(GameTextures[21]);
}
