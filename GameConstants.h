//Game Constants
#define WIDTH 300						// Minimum screen width
#define HEIGHT ((WIDTH / 12) * 9)		// Minimum screen height
#define SCALE 3							// Scale Factor Max = 3
#define FPS 120							// Minimum FPS that needs to be achieved
#define TIMERSCALE 700					// TimeScale factor for Level's new Target delay  (for fine tuning the game feel)
#define LEVELHITLIMIT 10				// Max Level Hit counter for proceeding to next level (for fine tuning the level difficulty)
#define LEVELMAX 15						// Max number of level required (For fine tuning. Game will adapt automatically)



/* Get random number in the [0..1] range */
#define getRandF() ((float) (rand() % 1001) / 1000.0f)		//Random Number Generator functions

