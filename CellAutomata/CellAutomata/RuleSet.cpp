// Ruleset.cpp : Implementation of the RuleSet class for simulating cellular automata

#include "Ruleset.h"
#include <string.h>
using namespace std;

RuleSet::RuleSet(unsigned char ucRuleNum)
{
	m_ucRuleNum = ucRuleNum;
	ParseRules();
}

// Each Rule is defined by mapping the binary representation of its rule #
// across the set of possible parent configurations 0-7:
// For Rule 30 as an example with 0 being a dead parent and 1 being a live parent:
// 30 in binary is 00011110
// 000 : 0 - 3 parents dead maps to 0 in binary, the 2^0 digit of 30 in binary is 0, so this result is a dead child
// 001 : 1 - Only the right parent alive maps to 1 in binary, the 2^1 digit of 30 in binary is 1, so this result 
//				is a live child
// 010 : 2 - Alive because the 2^2 digit of 30 is 1
// and so on through 111 : 7 to fill out the array
void RuleSet::ParseRules()
{
	for(unsigned int ii = 0; ii < NUM_PARENT_CONFIGURATIONS; ii++)
	{
		if (m_ucRuleNum & (1 << ii))
			m_uiChildStates[ii] = 1;
		else
			m_uiChildStates[ii] = 0;
	}
}

unsigned int RuleSet::CalcChildState(unsigned int uiLeftParent, unsigned int uiMidParent, unsigned int uiRightParent)
{
	// The 8 possible layouts of the parent cells match the binary
	// representation of 0-7, so using 0/1 to represent dead/alive
	// for each cell we can map each state to a predefined array index
	// which makes calculating any given child just a 2 step process
	unsigned int uiCode = (uiLeftParent << 2) | (uiMidParent << 1) | uiRightParent;

	// As a safety check, bitwise and uiCode with 7(111) to mask away any values that may have popped 
	// up due to bad data being passed in
	return m_uiChildStates[(uiCode & 7)];
}