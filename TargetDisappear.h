class TargetDisappear : public Target {
private:
	//Fields
	unsigned long lDisTimeDelay;
	unsigned long lStart;
	unsigned long lDiffTime;
	int iHitX;
	int iHitY;
	bool bDeactive;

public:
	//Constructor
	TargetDisappear(Target t, int iLevel, int iX, int iY) : Target(t) {
		bDeactive = false;
		lStart = KMiscTools::getMilliseconds();
		lDiffTime = 0uL;
		lDisTimeDelay = 1000uL;
		iHitX = iX;
		iHitY = iY;
	}


	//Render Function
	void render() {
		lDiffTime = KMiscTools::getMilliseconds() - lStart; // Get the time passed
		if (lDiffTime<lDisTimeDelay)
		{
			//Setting Transparency using Sine Wave
			float transp = (float)(abs(sin(double((lDiffTime * 900.0 / lDisTimeDelay)  * 3.14 / 180))));
			if (transp > 1) {
				transp = 1;
			}

			//Drawing Target + Shot area
			(*lpTargetImg)->blitAlphaRectFx(1.0f, 1.0f, (*lpTargetImg)->getWidth(), (*lpTargetImg)->getHeight(), (short)(iX - (*lpTargetImg)->getWidth() / 2), (short)(iY - (*lpTargetImg)->getHeight() / 2), 0.0f, (1.0f - (0.20f * iScale)) * (SCALE / 3.0f), transp);
			(*lpHitImg)->blitAlphaRectFx(1.0f, 1.0f, (*lpHitImg)->getWidth(), (*lpHitImg)->getHeight(), (short)(iHitX - (*lpHitImg)->getWidth() / 2), (short)(iHitY - (*lpHitImg)->getHeight() / 2), 0.0f, SCALE / 3.0f, transp);
		}
		else {
			bDeactive = true; //After the Disappear sequence is over, marking for removal
		}
	}

	//bDeactive Getter
	bool isDeactive() {
		return bDeactive;
	}

};