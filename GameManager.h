//Event Handler
bool myEventHandler(KEvent *lpKEvent) 
{

	switch (lpKEvent->type) 
	{
	case K_EVENT_ACTIVATED:
		KSound::resume();
		break;
	case K_EVENT_DEACTIVATED:
		KSound::suspend();
		break;
	}

	return true;
}

	 
class GameManager {
private:

	/************************** FIELDS **************************/

	//Game State variables
	int iHitCounter;
	int iGameScore;
	int iLevel;
	int iHitX;
	int iHitY;
	int iCurrFPS;
	unsigned long now;
	long lCreatedTime;
	unsigned long lLevelInit;
	int iLevelInitDelay;
	unsigned long lLevelTimer;
	long lLevelTimeLimit;
	int iTimeLeft;
	int iTimerClosingCounter;
	int iScoreDigits;
	bool bTimeOver;
	bool bTimeLimitClosing;
	bool bLevelInitWait;
	bool bRunning;
	bool bIntro;
	bool bSuccess;
	bool bScoreChanged;

	//Game Pointers/Handlers
	KWindow *lpKWindow;
	KSound *lpBgMusic;
	KSound *lpHitSnd;
	KSound *lpCounterSnd;
	KSound *lpBuzzSnd;
	KSound *lpFailSnd;
	KBatch *lpBatch;
	KGraphic *lpBgGraphic;
	KGraphic *lpBgIntro;
	KGraphic *lpLevelInit;
	KGraphic *lpLevelInitNumbers;
	KGraphic *lpLevelStatDisplay;
	KGraphic *lpLevelStatNumbers;
	KGraphic *lpGameOver;
	KGraphic *lpMouse;


	//Game Target Vectors
	std::vector<Target> vTarget;
	std::vector<TargetDisappear> vTargetDisappear;

	 
	//Game Title
	std::wstring wsAppTitle_Str_Temp;
	std::wstring wsAppTitle_Str_Main;

	//Variables to make Singleton
	static bool bInstanceFlag;
	static GameManager *lpGameManager;

	 GameManager() 
	 {
		 //Pointer Init
		 lpKWindow = NULL;
		 lpBgMusic = NULL;
		 lpFailSnd = NULL;
		 lpHitSnd = NULL;
		 lpCounterSnd = NULL;
		 lpBuzzSnd = NULL;
		 lpMouse = NULL;
		 lpBgIntro = NULL;
		 lpBgGraphic = NULL;
		 lpLevelInit = NULL;
		 lpLevelInitNumbers = NULL;
		 lpLevelStatDisplay = NULL;
		 lpLevelStatNumbers = NULL;
		 lpGameOver = NULL;
		 lpBatch = NULL;

		 //Variable Init
		iHitCounter = 0;			
		iGameScore = 0;
		iLevel = 1;
		iHitX = KInput::getMouseX();
		iHitY = KInput::getMouseY();
		iCurrFPS = FPS;
		now = 0uL;
		lCreatedTime = 0;
		lLevelInit = 0;
		iLevelInitDelay = 5000;
		lLevelTimer = 0;
		lLevelTimeLimit = 45 * 1000;
		iTimeLeft = (int)(lLevelTimeLimit / 1000);
		iTimerClosingCounter = 0;
		iScoreDigits = 1;
		bTimeOver = false;
		bTimeLimitClosing = false;
		bLevelInitWait = true;
		bRunning = true;
		bIntro = true;
		bSuccess = false;
		bScoreChanged = true;

		wsAppTitle_Str_Main = L"Target Game | UPS:  FPS: ";
		wsAppTitle_Str_Temp.assign(wsAppTitle_Str_Main);
		 
		/********************* WINDOW SETUP AREA *************************************************/
		//Select Graphics Driver as per the system
		#ifdef K_WIN32
			lpKWindow = KPTK::createKWindow(K_DIRECTX);
		#else
			lpKWindow = KPTK::createKWindow(K_OPENGL);
		#endif

		//setting the window preferences
		lpKWindow->setFixedAspectRatio(true);
		lpKWindow->createGameWindow(
			WIDTH * SCALE,
			HEIGHT * SCALE,
			32,
			true,
			(const char*)wsAppTitle_Str_Main.c_str()
		);
		lpKWindow->setMaxFrameRate(FPS);
		lpKWindow->setPTKCallBack(myEventHandler);

		//hiding windows default mouse pointer
		KInput::hidePointer();
		

		/********************* LOADING AREA *************************************************/
		//Load & play BGMusic
		lpBgMusic = new KSound;
		bSuccess = lpBgMusic->loadStream(KMiscTools::makeFilePath("res/audio/Intro.ogg"));
		if (!bSuccess) {
			KMiscTools::messageBox((const char*)L"Error", (const char*)L"Intro.ogg not found");
		}
		lpBgMusic->playStream();

		 //SFX-Fail
		 if (lpFailSnd == NULL) {
			 lpFailSnd = new KSound;
			 bSuccess = lpFailSnd->loadSample(KMiscTools::makeFilePath("res/audio/Fail.ogg"));
			 if (!bSuccess) {
				 KMiscTools::messageBox((const char*)L"Error", (const char*)L"Fail.ogg not found");
			 }
		 }

		 //SFX-Silencer
		 if (lpHitSnd == NULL) {
			 lpHitSnd = new KSound;			 
			 bSuccess = lpHitSnd->loadSample(KMiscTools::makeFilePath("res/audio/Silencer.ogg"));
			 if (!bSuccess) {
				 KMiscTools::messageBox((const char*)L"Error", (const char*)L"Silencer.ogg not found");
			 }
		 }

		 //SFX-Counter
		 if (lpCounterSnd == NULL) {			 
			 lpCounterSnd = new KSound;
			 bSuccess = lpCounterSnd->loadSample(KMiscTools::makeFilePath("res/audio/Tick.ogg"));
			 if (!bSuccess) {
				 KMiscTools::messageBox((const char*)L"Error", (const char*)L"Tick.ogg not found");
			 }
		 }
		 //SFX-Buzz
		 if (lpBuzzSnd == NULL) {			 
			 lpBuzzSnd = new KSound;
			 bSuccess = lpBuzzSnd->loadSample(KMiscTools::makeFilePath("res/audio/Buzzer.ogg"));
			 if (!bSuccess) {
				 KMiscTools::messageBox((const char*)L"Error", (const char*)L"Buzzer.ogg not found");
			 }
		 }

		//create KGraphics 

		//Cursor Load
		lpMouse = KPTK::createKGraphic();
		bSuccess = lpMouse->loadPicture(KMiscTools::makeFilePath("res/image/crosshair.png"), true, true);
		if (!bSuccess) {
			KMiscTools::messageBox((const char*)L"Error", (const char*)L"crosshair.png not found");
		}
		lpMouse->setTextureQuality(true);

		//Intro Load
		lpBgIntro = KPTK::createKGraphic();
		bSuccess = lpBgIntro->loadPicture(KMiscTools::makeFilePath("res/image/Intro.png"), true, true);
		if (!bSuccess) {
			KMiscTools::messageBox((const char*)L"Error", (const char*)L"Intro.png not found");
		}
		lpBgIntro->setTextureQuality(true);

		//Level Bg Load
		lpBgGraphic = KPTK::createKGraphic();
		bSuccess = lpBgGraphic->loadPicture(KMiscTools::makeFilePath("res/image/BgLevel.png"), true, true);
		if (!bSuccess) {
			KMiscTools::messageBox((const char*)L"Error", (const char*)L"BgLevel.png not found");
		}
		lpBgGraphic->setTextureQuality(true);

		//Level Init Text Load
		lpLevelInit = KPTK::createKGraphic();
		bSuccess = lpLevelInit->loadPicture(KMiscTools::makeFilePath("res/image/LevelInit.png"), true, true);
		if (!bSuccess) {
			KMiscTools::messageBox((const char*)L"Error", (const char*)L"LevelInit.png not found");
		}
		lpLevelInit->setTextureQuality(true);

		//Level Init Numbers Load
		//SpriteSheet containing 0 - 9. Each sprite is 23px wide and 30px high
		lpLevelInitNumbers = KPTK::createKGraphic();
		bSuccess = lpLevelInitNumbers->loadPicture(KMiscTools::makeFilePath("res/image/LevelInitNumbers.png"), true, true);
		if (!bSuccess) {
			KMiscTools::messageBox((const char*)L"Error", (const char*)L"LevelInitNumbers.png not found");
		}
		lpLevelInitNumbers->setTextureQuality(true);

		//Level Stat Display Text Load
		lpLevelStatDisplay = KPTK::createKGraphic();
		bSuccess = lpLevelStatDisplay->loadPicture(KMiscTools::makeFilePath("res/image/Level.png"), true, true);
		if (!bSuccess) {
			KMiscTools::messageBox((const char*)L"Error", (const char*)L"Level.png not found");
		}
		lpLevelStatDisplay->setTextureQuality(true);

		//Level Stat Display Number Load
		//SpriteSheet contain 0 - 9 + '0:'. Each sprite is 15px wide and 20px high
		lpLevelStatNumbers = KPTK::createKGraphic();
		bSuccess = lpLevelStatNumbers->loadPicture(KMiscTools::makeFilePath("res/image/LevelNumbers.png"), true, true);
		if (!bSuccess) {
			KMiscTools::messageBox((const char*)L"Error", (const char*)L"LevelNumbers.png not found");
		}
		lpLevelStatNumbers->setTextureQuality(true);

		//Game Over Text Load
		lpGameOver = KPTK::createKGraphic();
		bSuccess = lpGameOver->loadPicture(KMiscTools::makeFilePath("res/image/GameOver.png"), true, true);
		if (!bSuccess) {
			KMiscTools::messageBox((const char*)L"Error", (const char*)L"GameOver.png not found");
		}
		lpGameOver->setTextureQuality(true);
						
		//Batch Load + Preference setup
		lpBatch = KPTK::createKBatch();
		lpBatch->allocateVertexBuffer(200, K_BATCH_DYNTL);
	 }

	 //Run Game Logic
	 void update() {

		 if (!bIntro) {

			 //Waiting for the Level Initialization Sequence to finish
			 if (((now - lLevelInit) > (unsigned long)iLevelInitDelay) && bLevelInitWait) {
				 bLevelInitWait = false;		//Level Initialization Sequence is Over
				 Target::setDia(iLevel);		//reseting the static Target Diameters at the start of the level
			 }

			 //Starting Level Timer
			 if (!bLevelInitWait) {
				 if (lLevelTimer == 0) {
					 lLevelTimer = now;
				 }

				 // If level cleared, then setting Level parameters for the next round.
				 if (iLevel <= LEVELMAX && iHitCounter >= LEVELHITLIMIT) {
					 iLevel++;
					 iHitCounter = 0;
					 lLevelInit = now;
					 lLevelTimer = now;
					 iTimerClosingCounter = 0;
					 bTimeLimitClosing = false;
					 bLevelInitWait = true;
				 }


				 // Checking if level timer expired
				 if (((now) - lLevelTimer) > (unsigned long)lLevelTimeLimit) {
					 bRunning = false;
					 bTimeOver = true;
				 }

				 //calculating the time left for the player to play
				 iTimeLeft = (int)(lLevelTimeLimit - (now - lLevelTimer)) / 1000;

				 //Setting parameters to notify to player the last 10 seconds
				 if (iTimeLeft <= 11 && iTimeLeft > 0) {
					 iTimerClosingCounter++;
					 bTimeLimitClosing = true;
				 }

				 //Last 10seconds Counter music
				 if ((iTimerClosingCounter % iCurrFPS == 0) && (bTimeLimitClosing == true) && (iTimeLeft > 0))
					 lpCounterSnd->playSample();

				 //Time Over Music
				 if ((iTimerClosingCounter % iCurrFPS == 0) && (bTimeLimitClosing == true) && (iTimeLeft == 0) ) {
					 lpBuzzSnd->playSample();
					 bTimeLimitClosing = false;
				 }

				 //Player reached end of Game condition
				 if (iLevel > LEVELMAX)
					 bRunning = false;


				 if (bRunning) {

					 /********************************************* TARGET UPDATION *******************************************************/
					 //checking is new target created
					 if (lCreatedTime == 0)
						 lCreatedTime = now;

					 //Creates new Target after a time delay
					 if ((now - (unsigned long)lCreatedTime) > (unsigned long)(3 * (TIMERSCALE - ((TIMERSCALE / (LEVELMAX + 1)) * iLevel)))) {
						 lCreatedTime = 0;
						 vTarget.push_back(Target(
							 (int)(getRandF() * ((WIDTH * SCALE) - Target::getDia()) + Target::getRad()), 
							 (int)(getRandF() * ((HEIGHT * 3 * SCALE / 4) - Target::getDia()) + Target::getDia() + HEIGHT / 4),
							 iLevel,
							 now));

					 }

					 // Update shots

					 if (KInput::getLeftButtonState()) {
						 iHitX = KInput::getMouseX();
						 iHitY = KInput::getMouseY();
						 if (!vTarget.empty())
							 for (Target& target : vTarget) {

								 //Checking if the shot hit a target
								 if (!target.isHit()) {
									 int iTemp;

									 //Updating the Score
									 iTemp = target.getScore(iHitX, iHitY, lpHitSnd);
									 if (iTemp != 0) {
										 bScoreChanged = true; //setting score update variable to notify the draw function
									 }
									 iGameScore += iTemp;
								 }
								 if (target.isHit())
									 iHitCounter++; //updating number of targets hit in the level
							 }
					 }

					 // Removing target

					 if (!vTarget.empty())
						 for (int i = vTarget.size() - 1; i >= 0; i--) {
							 boolean bExpire = false;

							 //checking if the Target has expired / hit
							 bExpire = vTarget[i].isExpire(now);
							 if (bExpire) {

								 //If hit, Start Disappearance sequence
								 if (vTarget[i].isHit()) {
									 vTargetDisappear.push_back(TargetDisappear(vTarget[i], iLevel, iHitX, iHitY));
								 }

								 //Remove the Target
								 vTarget.erase(vTarget.begin() + i);
							 }

						 }

					 //Remove the Target Disappear sequence after playing
					 for (int i = (vTargetDisappear.size() - 1); i >= 0; i--) {
						 if (vTargetDisappear[i].isDeactive()) {
							 vTargetDisappear.erase(vTargetDisappear.begin() + i);
						 }
					 }

				 }
			 }
		 }
		 else {

			 //Check for ending the Intro sequence
			 if (KInput::getLeftButtonState()) {
				 bIntro = false;
				 lLevelInit = now;
			 }
		 }

	
	 }

	 //Draw onto KGraphic
	 void gameDraw() {

		 //Starting Level Initialization Draw function
		 if (bLevelInitWait) {
			 // LEVEL INITIALIZATION UPDATE

			 //Setting Transparency using Sine Wave
			 if (((now - lLevelInit) < (unsigned long)iLevelInitDelay)) {
				 float transp = float(abs(sin(double(((now - lLevelInit) * 450 / iLevelInitDelay) * 3.14 / 180))));
				 if (transp > 1) {
					 transp = 1;
				 }

				 //HUD + BG

				 lpBgGraphic->blitRectFxF(0.0f, 0.0f, lpBgGraphic->getWidth(), lpBgGraphic->getHeight(), 0.0f, 0.0f, 0.0f, SCALE / 3.0f, false, false, -lpBgGraphic->getWidth() / 2.0f, -lpBgGraphic->getHeight() / 2.0f);
				 
				 // Level Init Text drawing
				 lpLevelInit->blitAlphaRectFxF(
					 0.0f,
					 0.0f,
					 lpLevelInit->getWidth(),
					 lpLevelInit->getHeight(),
					 WIDTH * SCALE / 2 - lpLevelInit->getWidth() * (SCALE / 3.0f),
					 HEIGHT * SCALE / 2 - lpLevelInit->getHeight() * (SCALE / 3.0f) / 2,
					 0.0f, (SCALE / 3.0f),
					 transp,
					 false,
					 false,
					 -lpLevelInit->getWidth() / 2.0f,	//correcting offset which is at centre by default
					 -lpLevelInit->getHeight() / 2.0f);	//correcting offset which is at centre by default

				 //Level Init Number drawing

				 //Starting Batch processing : lpLevelInitNumbers
				 lpBatch->beginBatch(lpLevelInitNumbers);
				 
				 //iLevel's tenth digit
				 lpBatch->blitAlphaRectFx(
					 ((iLevel / 10) % 10) * 23.0f,				//Sprite number is at (iLevel's tenth digit * 23px)
					 0.0f,										//height init at 0px
					 ((iLevel / 10) % 10 + 1) * 23.0f,			//Width of sprite is (number + 1) * 23px
					 lpLevelInitNumbers->getHeight(),			//height const at 30px
					 WIDTH * SCALE / 2 + 23 * (SCALE / 3.0f),	//mid of screen + one char length (considering scale factor also)
					 HEIGHT * SCALE / 2 - (lpLevelInitNumbers->getHeight() * (SCALE / 3.0f) / 2), //mid of screen + 1/2 char height(considering scale factor also)
					 0.0f,										//no rotation
					 (SCALE / 3.0f),							//Considering scale factor
					 transp,									//transparancy
					 false,										//No Xflip
					 false,										//No Y Flip
					 - 23.0f / 2.0f,							//correcting offset which is at centre by default
					 -lpLevelInitNumbers->getHeight() / 2.0f);	//correcting offset which is at centre by default

				//iLevel's unit's digit
				 lpBatch->blitAlphaRectFx(
					 (iLevel % 10) * 23.0f,						//Sprite number is at (iLevel's unit's digit * 23px)
					 0.0f,										//height init at 0px
					 (iLevel % 10 + 1) * 23.0f,					//Width of sprite is (number + 1) * 23px
					 lpLevelInitNumbers->getHeight(),			//height const at 30px
					 WIDTH * SCALE / 2 + 2 * 23 * (SCALE / 3.0f),	//mid of screen + two char length (considering scale factor + spacing also)
					 HEIGHT * SCALE / 2 - (lpLevelInitNumbers->getHeight() * (SCALE / 3.0f) / 2), //mid of screen + 1/2 char height(considering scale factor also)
					 0.0f,										//no rotation
					 (SCALE / 3.0f),							//Considering scale factor
					 transp,									//transparancy
					 false,										//No Xflip
					 false,										//No Y Flip
					 -23.0f / 2.0f,								//correcting offset which is at centre by default
					 -lpLevelInitNumbers->getHeight() / 2.0f);	//correcting offset which is at centre by default

				 //Ending Batch : lpLevelInitNumbers
				 lpBatch->endBatch();

				 //Rendering Pending TargetDisappearing Sequences
				 if (!vTargetDisappear.empty())
					 for (TargetDisappear& tD : vTargetDisappear)
						 tD.render();
			 }
		 }
		 else {
			 
			 //HUD + BG

			 lpBgGraphic->blitRectFxF(0.0f, 0.0f, lpBgGraphic->getWidth(), lpBgGraphic->getHeight(), 0.0f, 0.0f, 0.0f, SCALE / 3.0f, false, false, -lpBgGraphic->getWidth() / 2.0f, -lpBgGraphic->getHeight() / 2.0f);

			 // HUD Displays

			 //Titles
			 lpLevelStatDisplay->blitAlphaRectFxF(0.0f, 0.0f, lpLevelStatDisplay->getWidth(), lpLevelStatDisplay->getHeight(), 5.0f, 12.0f * SCALE , 0.0f, SCALE / 3.0f, 1.0f, false, false, -lpLevelStatDisplay->getWidth() / 2.0f, -lpLevelStatDisplay->getHeight() / 2.0f);

			 //Numbers

			 //Begin Numbers Batch : lpLevelStatNumbers
			 lpBatch->beginBatch(lpLevelStatNumbers);

			 //Setting Black Color to text
			 lpLevelStatNumbers->setBlitColor(0.0f, 0.0f, 0.0f, 1.0f);

			 //Level Digits Draw

			 //iLevel's tenth digit
			 lpBatch->blitAlphaRectFx(
				 ((iLevel / 10) % 10) * 15.0f,				//Sprite number is at (iLevel's tenth digit * 15px)
				 0.0f,										//height init at 0px
				 ((iLevel / 10) % 10 + 1) * 15.0f,			//Width of sprite is (number + 1) * 15px
				 lpLevelStatNumbers->getHeight(),			//height const at 20px
				 WIDTH * SCALE / 2 + 20 + 15 * (SCALE / 3.0f),	//end of "LEVEL" title + one char length (considering scale factor also)
				 12.0f * SCALE + 1,								//end of "LEVEL" title + 1/2 char height(considering scale factor also)
				 0.0f,										//no rotation
				 (SCALE / 3.0f),							//Considering scale factor
				 1.0f,										//no transparancy
				 false,										//No Xflip
				 false,										//No Y Flip
				 -15.0f / 2.0f,								//correcting offset which is at centre by default
				 -lpLevelStatNumbers->getHeight() / 2.0f);	//correcting offset which is at centre by default

			//iLevel's unit's digit
			 lpBatch->blitAlphaRectFx(
				 (iLevel % 10) * 15.0f,						//Sprite number is at (iLevel's unit's digit * 15px)
				 0.0f,										//height init at 0px
				 (iLevel % 10 + 1) * 15.0f,					//Width of sprite is (number + 1) * 15px
				 lpLevelStatNumbers->getHeight(),			//height const at 30px
				 WIDTH * SCALE / 2 + 20 + 2 * 15 * (SCALE / 3.0f),	//end of "LEVEL" title + two char length (considering scale factor also)
				 12.0f * SCALE + 1,								//end of "LEVEL" title + 1/2 char height(considering scale factor also)
				 0.0f,										//no rotation
				 (SCALE / 3.0f),							//Considering scale factor
				 1.0f,										//no transparancy
				 false,										//No Xflip
				 false,										//No Y Flip
				 -15.0f / 2.0f,								//correcting offset which is at centre by default
				 -lpLevelStatNumbers->getHeight() / 2.0f);	//correcting offset which is at centre by default

			 
			 //Timer Digits Draw
			 
			 //Normal timer Sequence
			 if (!bTimeLimitClosing || (iTimeLeft > 10)) {
				 drawTimer(1.0f);
			 }
			 else {
			 
			 //LAST !0 seconds Sequence timer

			 //Setting Transparency using Sine Wave
			 float fAlpha = (float)abs(sin(double((180 * iTimerClosingCounter / iCurrFPS) * 3.14 / 180)));
			 if (fAlpha > 1) {
				 fAlpha = 1;
			 }

			 //setting color to red and the alpha
			 lpLevelStatNumbers->setBlitColor(1.0f, 0.0f, 0.0f, fAlpha);
			 
			 drawTimer(fAlpha);
			 
			 //setting the colors back to original
			 lpLevelStatNumbers->setBlitColor(0.0f, 0.0f, 0.0f, 1.0f);
			 }

			 //Score Digits Draw

			 drawScore();

			 //End Batch : lpLevelStatNumbers
			 lpBatch->endBatch();

			// Target Rendering
			 if (!vTarget.empty()) // Target
				 for (Target& target : vTarget) {
					 target.render();
				 }

			 // Target Disappearing Rendering
			 if (!vTargetDisappear.empty())
				 for (TargetDisappear& tD : vTargetDisappear)
					 tD.render();

			 //Draw and Clip the custom cursor move area
			 if ((int)KInput::getMouseY() < (HEIGHT * SCALE) / 6)
			 {
				 lpMouse->blitAlphaRectF(0, 0, lpMouse->getWidth(), lpMouse->getHeight(), KInput::getMouseX() - lpMouse->getWidth() / 2, (HEIGHT * SCALE) / 6 - lpMouse->getHeight() / 2, false);
			 }
			 else
			 {
				 lpMouse->blitAlphaRectF(0, 0, lpMouse->getWidth(), lpMouse->getHeight(), KInput::getMouseX() - lpMouse->getWidth() / 2, KInput::getMouseY() - lpMouse->getHeight() / 2, false);
			 }
			 lpMouse->setTextureQuality(true);
		 }
	 }

	 //Draw Timer
	 void drawTimer(float fAlpha) {
		 //print "0:"
		 lpBatch->blitAlphaRectFx(
			 10 * 15.0f,								//"0:" is at 10th & 11th position in the SpriteSheet
			 0.0f,										//height init at 0px
			 12 * 15.0f,								//Width of sprite is (11th + 1) * 15px
			 lpLevelStatNumbers->getHeight(),			//height const at 30px
			 7 * 15 * (SCALE / 3.0f),					//end of "TIMER" title + one char length (considering scale factor also)
			 12.0f * SCALE * 2 + 1,						//end of "TIMER" title + 1/2 char height(considering scale factor also)
			 0.0f,										//no rotation
			 (SCALE / 3.0f),							//Considering scale factor
			 fAlpha,									//transparancy
			 false,										//No Xflip
			 false,										//No Y Flip
			 -15.0f / 2.0f,								//correcting offset which is at centre by default
			 -lpLevelStatNumbers->getHeight() / 2.0f);	//correcting offset which is at centre by default

		//Time left
		
		//iTimeLeft's tenth digit
		 lpBatch->blitAlphaRectFx(
			 ((iTimeLeft / 10) % 10) * 15.0f,			//Sprite number is at (iTimeLeft's tenth digit * 15px)
			 0.0f,										//height init at 0px
			 ((iTimeLeft / 10) % 10 + 1) * 15.0f,		//Width of sprite is (number + 1) * 15px
			 lpLevelStatNumbers->getHeight(),			//height const at 20px
			 9 * 15 * (SCALE / 3.0f),					//end of "TIMER" title + one char length (considering scale factor also)
			 12.0f * SCALE * 2 + 1,						//end of "TIMER" title + 1/2 char height(considering scale factor also)
			 0.0f,										//no rotation
			 (SCALE / 3.0f),							//Considering scale factor
			 fAlpha,									//transparancy
			 false,										//No Xflip
			 false,										//No Y Flip
			 -15.0f / 2.0f,								//correcting offset which is at centre by default
			 -lpLevelStatNumbers->getHeight() / 2.0f);	//correcting offset which is at centre by default

		//iTimeLeft's unit's digit
		 lpBatch->blitAlphaRectFx(
			 (iTimeLeft % 10) * 15.0f,					//Sprite number is at (iTimeLeft's unit's digit * 15px)
			 0.0f,										//height init at 0px
			 (iTimeLeft % 10 + 1) * 15.0f,				//Width of sprite is (number + 1) * 15px
			 lpLevelStatNumbers->getHeight(),			//height const at 30px
			 10 * 15 * (SCALE / 3.0f),					//end of "TIMER" title + one char length (considering scale factor also)
			 12.0f * SCALE * 2 + 1,						//end of "TIMER" title + 1/2 char height(considering scale factor also)
			 0.0f,										//no rotation
			 (SCALE / 3.0f),							//Considering scale factor
			 fAlpha,									//transparancy
			 false,										//No Xflip
			 false,										//No Y Flip
			 -15.0f / 2.0f,								//correcting offset which is at centre by default
			 -lpLevelStatNumbers->getHeight() / 2.0f);	//correcting offset which is at centre by default
	 }

	 //Draw Score
	 void drawScore(bool bGameOver = false) {

		 //Checking if the score changed to preserve unnecessary computation
		 if (bScoreChanged) {

			 //Compute the number of digits in the score
			 int iTemp;
			 iTemp = iGameScore;
			 iScoreDigits = 0;
			 while (iTemp > 0) {
				 iTemp /= 10;
				 iScoreDigits++;
			 }
			 if (iScoreDigits == 0) {
				 iScoreDigits = 1;
			 }
		 }

		 //Display the score differently for Gameover sequence and during game running
		 if (!bGameOver) {
			 for (int i = iScoreDigits; i > 0; i--) {
				 lpBatch->blitAlphaRectFx(
					 ((iGameScore / (int)pow(10, i - 1)) % 10) * 15.0f,	//Sprite number is at (iGameScore's tenth digit * 15px)
					 0.0f,												//height init at 0px
					 ((iGameScore / (int)pow(10, i - 1)) % 10 + 1) * 15.0f,			//Width of sprite is (number + 1) * 15px
					 lpLevelStatNumbers->getHeight(),					//height const at 20px
					 (7 * 15.0f +(iScoreDigits - i) * 15) * (SCALE / 3.0f),		//end of "Score:" title + ith char length (considering scale factor also)
					 12.0f * SCALE + 1,									//end of "Score:" title + 1/2 char height(considering scale factor also)
					 0.0f,										//no rotation
					 (SCALE / 3.0f),							//Considering scale factor
					 1.0f,										//no transparancy
					 false,										//No Xflip
					 false,										//No Y Flip
					 -15.0f / 2.0f,								//correcting offset which is at centre by default
					 -lpLevelStatNumbers->getHeight() / 2.0f);	//correcting offset which is at centre by default
			 }
		 }
		 else {
			 for (int i = iScoreDigits; i > 0; i--) {
				 lpBatch->blitAlphaRectFx(
					 ((iGameScore / (int)pow(10, i - 1)) % 10) * 23.0f,	//Sprite number is at (iGameScore's tenth digit * 23px)
					 0.0f,												//height init at 0px
					 ((iGameScore / (int)pow(10, i - 1)) % 10 + 1) * 23.0f,			//Width of sprite is (number + 1) * 23px
					 lpLevelInitNumbers->getHeight(),					//height const at 20px
					 (WIDTH * 2 +(iScoreDigits - i) * 23) * (SCALE / 3.0f),		//end of "Score:" title + ith char length (considering scale factor also)
					 HEIGHT * SCALE / 2.0f + 1,							//end of "Score:" title + 1/2 char height(considering scale factor also)
					 0.0f,										//no rotation
					 (SCALE / 3.0f),							//Considering scale factor
					 1.0f,										//no transparancy
					 false,										//No Xflip
					 false,										//No Y Flip
					 -23.0f / 2.0f,								//correcting offset which is at centre by default
					 -lpLevelInitNumbers->getHeight() / 2.0f);	//correcting offset which is at centre by default
			 }
		 }
	 }

	 //Game Intro function
	 void intro() {

		 //Entire Intro Screen Image Render

		 lpBgIntro->blitRectFx(0.0f, 0.0f, lpBgIntro->getWidth(), lpBgIntro->getHeight(), 0, 0, 0.0f, SCALE / 3.0f, false, false, -lpBgIntro->getWidth() / 2.0f, -lpBgIntro->getHeight() / 2.0f);

		 //Draw and Clip the custom cursor move area
		 if ((int)KInput::getMouseY() < (HEIGHT * SCALE) / 6)
		 {
			 lpMouse->blitAlphaRectF(0, 0, lpMouse->getWidth(), lpMouse->getHeight(), KInput::getMouseX() - lpMouse->getWidth() / 2, (HEIGHT * SCALE) / 6 - lpMouse->getHeight() / 2, false);
		 }
		 else
		 {
			 lpMouse->blitAlphaRectF(0, 0, lpMouse->getWidth(), lpMouse->getHeight(), KInput::getMouseX() - lpMouse->getWidth() / 2, KInput::getMouseY() - lpMouse->getHeight() / 2, false);
		 }
		 lpMouse->setTextureQuality(true);

	 }

	 //Game Over
	 void stop() 
	 {
		// setting game flags to close mode
		 bRunning = false;
		 bTimeLimitClosing = false;
		 if (lpBgMusic->isPlaying()) {
			 lpBgMusic->stopStream();
		 }

		 //Play Fail song only if timer over & not if player crosses the Max Level
		 if (bTimeOver) {
			 lpFailSnd->playSample();
		 }

		 do {
			 // Final Results Draw

			 // HUD + BG

			 lpBgGraphic->blitRectFxF(0.0f, 0.0f, lpBgGraphic->getWidth(), lpBgGraphic->getHeight(), 0.0f, 0.0f, 0.0f, SCALE / 3.0f, false, false, -lpBgGraphic->getWidth() / 2.0f, -lpBgGraphic->getHeight() / 2.0f);

			 // GAME OVER TEXT DISPLAY

			 float iStartPx = 0;

			 //REmove TIMEOVER script from SpriteSheet
			 if (!bTimeOver) {
				 iStartPx = 50;
			 }

			 //Draw the Image
			 lpGameOver->blitAlphaRectFxF(0.0f, iStartPx, lpGameOver->getWidth(), lpGameOver->getHeight(), WIDTH * SCALE / 3.0f, iStartPx + HEIGHT * SCALE / 3.0f, 0.0f, SCALE / 3.0f,1.0f, false, false, -lpGameOver->getWidth() / 2.0f, -lpGameOver->getHeight() / 2.0f);

			 //SCORE TEXT DISPLAY
			 lpBatch->beginBatch(lpLevelInitNumbers);

			 drawScore(true);

			 lpBatch->endBatch();

			 //Draw and Clip the custom cursor move area
			 if ((int)KInput::getMouseY() < (HEIGHT * SCALE) / 6)
			 {
				 lpMouse->blitAlphaRectF(0, 0, lpMouse->getWidth(), lpMouse->getHeight(), KInput::getMouseX() - lpMouse->getWidth() / 2, (HEIGHT * SCALE) / 6 - lpMouse->getHeight() / 2, false);
			 }
			 else
			 {
				 lpMouse->blitAlphaRectF(0, 0, lpMouse->getWidth(), lpMouse->getHeight(), KInput::getMouseX() - lpMouse->getWidth() / 2, KInput::getMouseY() - lpMouse->getHeight() / 2, false);
			 }
			 lpMouse->setTextureQuality(true);

			 // rendering Final Screen
			 lpKWindow->flipBackBuffer();

		 } while (!lpKWindow->isQuit() && !KInput::getLeftButtonState());		 
	 }

public:

	//To implement Singleton Class
	static GameManager* getInstance() 
	{
		if (!bInstanceFlag) {
			lpGameManager = new GameManager();
			bInstanceFlag = true;
		}
			return lpGameManager;
	}

	//Main Run Class
	void run() 
	{


		long lastTime = KMiscTools::getMilliseconds();
		long timer = lastTime;
		double milli = 1000.0 / FPS;
		double delta = 0;
		int fps = 0;	//speed at which the game will be rendering
		int ups = 0;	//speed at which the code will be updating
		


		//Game Loop
		do {
			//Enabling the processEvents
			lpKWindow->processEvents();

			//Set the world View Area
			lpKWindow->setWorldView(0.0f, 0.0f, 0.0f, 1.0f, false);

			ups++; // FRAMES counter

			/************ FPS & UPS COUNTERS ******************/
			
			now = KMiscTools::getMilliseconds();
			delta += (now - lastTime) / milli;
			lastTime = now;

			//UPS - Updates per Second

			ups++; // FRAMES counter

			update(); // updates the game logics

			//FPS - Frames/rendering per second
			if (delta >= 1) {

				delta--;
				fps++; // rendering counter

				//Game Draw
				if (!bIntro) {
					gameDraw(); // Draws onto the KGraphics
				}
				else {
						intro(); // Draws Intro onto the KGraphics
				}


				lpKWindow->flipBackBuffer(); //draws on to the Screen Buffer
				
				// display the UPS & FPS counters on the title bar
				if ((now - timer) > 1000) {
					timer += 1000;
					wsAppTitle_Str_Main.assign(wsAppTitle_Str_Temp);
					wsAppTitle_Str_Main.insert(24, std::to_wstring(fps));
					wsAppTitle_Str_Main.insert(18, std::to_wstring(ups));
					lpKWindow->setTitle((const char*)wsAppTitle_Str_Main.c_str());
					iCurrFPS = fps;
					ups = 0;
					fps = 0;
				}
				
			}
		
		} while (!lpKWindow->isQuit() && bRunning);

		if (!lpKWindow->isQuit()) {
			stop();
		}
	}

	~GameManager() 
	{		
		//clean up
		if (lpBatch != NULL)
		{
			lpBatch->freeBuffer();
			delete lpBatch;
			lpBatch = NULL;
		}
		if (lpGameOver != NULL)delete lpGameOver;
		if (lpLevelStatNumbers != NULL)delete lpLevelStatNumbers;
		if (lpLevelStatDisplay != NULL)delete lpLevelStatDisplay;
		if (lpLevelInitNumbers != NULL)delete lpLevelInitNumbers;
		if (lpLevelInit != NULL)delete lpLevelInit;
		if (lpBgGraphic != NULL)delete lpBgGraphic;
		if (lpBgIntro != NULL)delete lpBgIntro;
		if (lpMouse != NULL)delete lpMouse;
		if (lpBuzzSnd != NULL)delete lpBuzzSnd;
		if (lpCounterSnd != NULL)delete lpCounterSnd;
		if(lpHitSnd != NULL)delete lpHitSnd;
		if(lpFailSnd != NULL)delete lpFailSnd;		

		if(lpBgMusic != NULL )
		{
			if(lpBgMusic->isPlaying())
				lpBgMusic->stopStream();
			delete lpBgMusic;
		}
		if(lpKWindow != NULL)	delete lpKWindow;	
	}

};


