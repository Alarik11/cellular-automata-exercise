// CellAutomata.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <conio.h>
#include <iostream>
#include <windows.h>
#include <string>
using namespace std;

#include "Ruleset.h"
#include "CellAutomata.h"

// This creates a default simulation using Rule 30
RuleSet g_sRules;

int _tmain(int argc, _TCHAR* argv[])
{
	// This will hold input for verification before being converted into usable data
	string sInBuffer;

	char cOpt = '\0';
	do
	{
		cout << "Cellular Automata Simulator\n\n";
		cout << "1:Select the Simulation Rule(Currently " << (int)g_sRules.GetRuleNum() << ")\n";
		cout << "2:Run the Simulation\n";
		cout << "Q:Quit Program\n";
		cout << "Pick an option(1, 2, or Q):";
		
		// We only care about the first character, but getline is the cleanest way to get the input
		getline(cin, sInBuffer);
		if (sInBuffer.length() > 0)
			cOpt = toupper(sInBuffer.at(0));
		else
			cOpt = '\0';

		switch (cOpt)
		{
			case '1':
			{
				UserSelectRule();
				break;
			}
			case '2':
			{
				cout << "Beginning Simulation\n";
				cout << "Press any key to pause execution\n";
				// Parse the rules in case a new rule number has been selected
				g_sRules.ParseRules();
				RunSimulation();
				break;
			}
			case 'Q':
			{
				cout << "Quitting..\n";
				break;
			}
			default:
			{
				cout << "Invalid Option, please enter 1,2 or Q\n";
			}
		}
	}
	while(cOpt != 'Q');
	
	return 0;
}

// Prompts the user to enter a number between 0 and 255, then validates
// the input, prompting the user to try again if invalid data is entered
void UserSelectRule()
{
	string sInBuffer;
	bool bValid;
	unsigned int uiValue = 0;

	do
	{
		bValid = true;
		cout << "Enter a new rule number(0-255):";
		getline(cin, sInBuffer);


		const char *pTemp = sInBuffer.c_str();
		for(unsigned int ii = 0; ii < sInBuffer.length(); ii++)
		{
			// If any character is neither whitespace nor a digit
			// then this input is invalid
			if (!iswspace(pTemp[ii]) && !isdigit(pTemp[ii]))
			{
				bValid = false;
				cout << "That's not a number.\n";
				break;
			}
		}
		uiValue = atoi(pTemp);
		if (uiValue > 255)
		{
			cout << "The number you entered is too large.\n";
			bValid = false;
		}
	}while(!bValid);

	cout << "Using rule number " << uiValue << "\n";
	g_sRules.SetRuleNum(unsigned char(uiValue));

	return;
}


// Having a static generation size is a reasonable assumption given that
// this is a console application with a limited display area, and that
// assumption simplifies the array handling considerably
#define GEN_SIZE 64

// Simulates a family of cellular automata based on the currently configured ruleset
void RunSimulation()
{
	bool bPaused = false;
	unsigned int uiGenCount = 0;

	// Windows time structures, used to slow down the simulation of generations to give the
	// user time to pause, without the delay the entire simulation would run too quickly
	// for any user interaction
	FILETIME sStart, sNow;
	ULARGE_INTEGER suStart, suNow;
	// FILETIME counts intervals of 100 nanoseconds, so 3,000,000 units is .3 seconds
	unsigned int uiFileTimeWaitInterval = 3000000;

	// Making the generation array 2 sizes larger than the generation size
	// allows us to treat the first and last cell the same as any other,
	// as it will always have another empty cell to be its left or right
	// parent
	unsigned int *puiParentGen = new unsigned int[GEN_SIZE + 2];
	unsigned int *puiCurrentGen = new unsigned int [GEN_SIZE + 2];
	unsigned int *pTemp = NULL;
	// Set up the initial states for the two cell arrays
	unsigned int ii = 0;
	for(ii = 0; ii < GEN_SIZE + 2; ii++)
	{
		puiParentGen[ii] = 0;
		puiCurrentGen[ii] = 0;
	}
	puiParentGen[GEN_SIZE / 2] = 1;
	// Print the first generation before starting the loop
	for(ii = 1; ii <= GEN_SIZE; ii++)
		cout << puiParentGen[ii];
	cout << "\n";

	// Running a number of generations equal to half the generation size(counting the first parent)
	// should let the simulation run up until it reaches the edges of the displayable area when using
	// rule 30 or rules with similar patterns. Any further iterations would have
	// odd behavior as all cells outside the visible area are treated as dead
	while(uiGenCount < (GEN_SIZE / 2 - 1))
	{
		// Calculate the next generation and display it
		for(ii = 1; ii <= GEN_SIZE; ii++)
		{
			puiCurrentGen[ii] = g_sRules.CalcChildState(puiParentGen[ii - 1],puiParentGen[ii],puiParentGen[ii + 1]);
			cout << puiCurrentGen[ii];
		}
		cout << "\n";

		// Since current gen is overwritten every iteration, we can just swap the two memory locations every
		// time through: The current generation becomes the parent and the old parent is overwritten by the
		// next current generation
		pTemp = puiCurrentGen;
		puiCurrentGen = puiParentGen;
		puiParentGen = pTemp;

		// Now we delay a short time before the next loop
		// Get the start time and convert it to a value we can use in comparisons
		GetSystemTimeAsFileTime(&sStart);
		suStart.LowPart = sStart.dwLowDateTime;
		suStart.HighPart = sStart.dwHighDateTime;
		do
		{
			// If execution isn't paused, get the current time for checking at
			// the end of the loop
			if (!bPaused)
			{
				GetSystemTimeAsFileTime(&sNow);
				suNow.LowPart = sNow.dwLowDateTime;
				suNow.HighPart = sNow.dwHighDateTime;
			}
			// Check for any user input to pause,unpause or quit as appropriate
			if (_kbhit())
			{
				// kbhit doesn't pull anything out of cin, so pull the input value
				// out to clear the input buffer and have it available to check for
				// the user quitting
				int iChar = _getch();
				if (!bPaused)
				{
					bPaused = true;
					cout << "Pausing Execution, Q to quit, any other key to continue.";
				}
				// If unpausing, go ahead and force the next loop
				else
				{
					// If the user quit, just return to the menu
					if (toupper(iChar) == 'Q')
					{
						cout << "\n\n\n";
						return;
					}
					bPaused = false;
					// Back up to the start of the current line to overwrite the pause message
					// and keep the generations on successive lines
					cout << "\r";
					break;
				}
			}
		// As long as looping is paused or it hasn't been too long, keep waiting
		} while (bPaused || ((suStart.QuadPart + uiFileTimeWaitInterval) > suNow.QuadPart));
		
		uiGenCount++;
	}
}