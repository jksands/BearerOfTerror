#ifndef __Score_H_
#define __Score_H_

#include <stdio.h>

namespace Simplex
{
	class Score
	{
		static int score;
	public:
		/*
		USAGE: Change the score by a value
		ARGUMENTS:
		-integer to change score by
		OUTPUT: none
		*/
		static void ChangeScore(int points);

		/*
		USAGE: Returns The Score Integer
		ARGUMENTS: None
		OUTPUT: integer value of the score
		*/
		static int GetScore(void);

		/*
		USAGE: Returns The Score as a character array
		ARGUMENTS: None
		OUTPUT: char array of score
		*/
		static char* GetScoreChar(void);

	};


}
#endif __Score_H_
