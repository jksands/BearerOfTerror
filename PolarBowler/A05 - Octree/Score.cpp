#include "Score.h"
using namespace Simplex;

int Score::score = 0;

//alter the score by an integer
void Score::ChangeScore(int points)
{
	score += points;
}

//returns the core integer
int Simplex::Score::GetScore(void)
{
	return Score::score;
}

//returnsa character pointer of the score
char* Simplex::Score::GetScoreChar(void)
{
	char toReturn[128];
	int ret = snprintf(toReturn, sizeof(toReturn), "%ld", score);
	char* toReturnPointer = toReturn; 
	return toReturnPointer;
}
