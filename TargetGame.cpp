
//Std CPP Header files
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <memory>		//for shared_ptr
#include "resource.h"

//Kanji Engine Header files
#include "KGame.h"
#include "KSound.h"
#include "KGraphic.h"
#include "GameConstants.h"
#include "Target.h"
#include "TargetDisappear.h"
#include "GameManager.h"

// Target Class Static variables
int Target::iDia1 = 0;
int Target::iDia2 = 0;
int Target::iDia3 = 0;
std::shared_ptr<KGraphic*> Target::lpTargetImg = NULL;
std::shared_ptr<KGraphic*> Target::lpHitImg = NULL;

// GameManager Class Static variables
bool GameManager::bInstanceFlag;
GameManager *GameManager::lpGameManager;

//Game Entry Point
#ifdef K_WIN32
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, INT iCmdShow) {
#else
int kanjiMain(int argc, char **argv) {
#endif
		
	GameManager *lpGameManager = NULL;
	KResource::addArchive(KMiscTools::makeFilePath("res.pak"), K_RESARCHIVE_REQUIRED, "");

	/********************* GAME LOGIC AND RENDERING AREA *************************************************/
	lpGameManager = lpGameManager->getInstance();
	
	lpGameManager->run();

	delete lpGameManager;

	return 0;
}