class Target
{

protected:
	/*************************** FIELD ***********************/
	
	//Non-static Target Status Variables
	int iLevel;
	int iScale;
	int iX, iY;
	unsigned long lStartTime;
	bool bExpire;
	bool bHit;

	//Level determined Static variables
	static int iDia1;
	static int iDia2;
	static int iDia3;
	static std::shared_ptr<KGraphic*> lpTargetImg;	//Smart Pointer
	static std::shared_ptr<KGraphic*> lpHitImg;		//Smart Pointer


public:

	//Constructor
	Target(int iX, int iY, int iLevel, long lStartTime) {

		bExpire = false;
		bHit = false;
		this->iX = iX;
		this->iY = iY;
		this->lStartTime = lStartTime;
		this->iLevel = iLevel;
		iScale = iLevel / 4;

		//Set Static Diameter
		if (iDia1 == 0 || iDia2 == 0 || iDia3 == 0) {
			iDia1 = ((15 - (iScale * 3)) * SCALE * 3);
			iDia2 = ((11 - (iScale * 2)) * SCALE * 3);
			iDia3 = ((5 - iScale) * SCALE * 3);
		}

		//Loading Target Image
		if (lpTargetImg == NULL) {
			//getting KGraphic
			lpTargetImg = std::make_shared<KGraphic*>(KPTK::createKGraphic());

			//loading static image
			bool bSuccess = false;
			bSuccess = (*lpTargetImg)->loadPicture(KMiscTools::makeFilePath("res/image/Target.png"), true, true);
			if (!bSuccess) {
				KMiscTools::messageBox((const char*)L"Error", (const char*)L"Target.png not found");
			}
		}


		//Loading Shot Image
		if (lpHitImg == NULL) {
			lpHitImg = std::make_shared<KGraphic*>(KPTK::createKGraphic());

			bool bSuccess = false;
			bSuccess = (*lpHitImg)->loadPicture(KMiscTools::makeFilePath("res/image/Hit.png"), true, true);
			if (!bSuccess) {
				KMiscTools::messageBox((const char*)L"Error", (const char*)L"Hit.png not found");
			}
		}
	}

	//Overridden Copy Constructor
	Target(const Target &t) {
		this->iLevel = t.iLevel;
		this->bExpire = t.bExpire;
		this->bHit = t.bExpire;
		this->iX = t.iX;
		this->iY = t.iY;
		this->lStartTime = t.lStartTime;
		iScale = t.iScale;
	}

	//renderer function
	void render() {

		//Checking if the Target has Expired / Hit
		if (!isExpire(KMiscTools::getMilliseconds())) {
			(*lpTargetImg)->blitAlphaRectFxF(0.0f, 0.0f, (*lpTargetImg)->getWidth(), (*lpTargetImg)->getHeight(), (iX - (*lpTargetImg)->getWidth() / 2.0f), (iY - (*lpTargetImg)->getHeight() / 2.0f), 0.0f, (1.0f - (0.20f * iScale)) * (SCALE / 3.0f), 1.0f);
		}
	}

	//Expired Getter
	bool isExpire(unsigned long endTime) {

		if (3 * TIMERSCALE > (100 - iLevel)) {
			if ((endTime - lStartTime) > (unsigned long)(3 * (TIMERSCALE - ((TIMERSCALE / (LEVELMAX + 1)) * iLevel))))
				bExpire = true;
		}
			return bExpire;

	}

	//Recalulate Dia at start each level
	static void setDia(int iLevel) {
		iDia1 = (15 - (iLevel * 3 / 4)) * SCALE * 3;
		iDia2 = (11 - (iLevel * 2 / 4)) * SCALE * 3;
		iDia3 = (5 - iLevel / 4) * SCALE * 3;
	}

	//Largest Dia Getter
	static int getDia() {
		return iDia1;
	}

	//Largest Radius Getter
	static int getRad() {
		return iDia1 / 2;
	}

	//Hit Getter
	bool isHit() {
		return bHit;
	}

	//Score Calculator wrt radius of the target
	int getScore(double iX, double iY, KSound *lpHitSnd) {
		if (!bHit) {
			double exp = sqrt(pow((this->iX - iX), 2) + pow((this->iY - iY), 2));
			if (exp < iDia3 / 2) {
				bExpire = true;
				bHit = true;
				lpHitSnd->playSample();
				return 10;
			}
			else if (exp < iDia2 / 2) {
				bHit = true;
				bExpire = true;
				lpHitSnd->playSample();
				return 5;
			}
			else if (exp < iDia1 / 2) {
				bHit = true;
				bExpire = true;
				lpHitSnd->playSample();
				return 1;
			}
			else
				return 0;
		}
		else {
			return 0;
		}
	}
};


