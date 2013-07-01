// Ruleset.h : A class to store the rules for a cellular automata simulation

#define NUM_PARENT_CONFIGURATIONS 8

class RuleSet
{
private:
	unsigned char m_ucRuleNum;
	unsigned int m_uiChildStates[NUM_PARENT_CONFIGURATIONS];

public:
	unsigned int CalcChildState(unsigned int uiLeftParent, unsigned int uiMidParent, unsigned int uiRightParent);
	void ParseRules();
	RuleSet(unsigned char ucRuleNum = 30);

	void SetRuleNum(unsigned char ucRuleNum) { m_ucRuleNum = ucRuleNum; };
	unsigned char GetRuleNum() { return m_ucRuleNum; };
};