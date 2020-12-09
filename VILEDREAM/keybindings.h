#pragma once

#include <iostream>
#include <fstream>

#include <string>
#include <vector>
#include <memory>

#include "libtcod.hpp"
#include "loader_constants.h"
#include "utility.h"

using namespace std;


#define KEY_MARK_QUIT			"KEY_QUIT"
#define KEY_MARK_ACCEPT			"KEY_ACCEPT"
#define KEY_MARK_EXIT			"KEY_EXIT"




//					Key action codings.


#define KEY_ACTION_NONE			-1
#define KEY_ACTION_ACCEPT		0
#define KEY_ACTION_EXIT			1
#define KEY_ACTION_QUIT			2
#define KEY_ACTION_COUNT		3

inline int getKeyActionFromTag(string tag)
{
	if		(tag == KEY_MARK_ACCEPT)	return KEY_ACTION_ACCEPT;
	else if (tag == KEY_MARK_EXIT)		return KEY_ACTION_EXIT;
	else if (tag == KEY_MARK_QUIT)		return KEY_ACTION_QUIT;

	return KEY_ACTION_NONE;
}



//					LibTCOD codings.
inline TCOD_keycode_t getKeycodeFromTag(string tag)
{
	if		(tag == "K_ENTER")		return TCODK_ENTER;
	else if (tag == "K_ESCAPE")		return TCODK_ESCAPE;
	return TCODK_0;
}



//		Parallel vectors: actions, & then either the TCODK key or character associated.
struct keybindings
{
	vector<char>			_c;
	vector<TCOD_keycode_t>	_vk;
};



//		Loads key-bindings from the file.
inline keybindings getAllKeybindings()
{
	//		Keybindings object we're going to create.
	keybindings kb;
	for (int i = 0; i < KEY_ACTION_COUNT; i++)
	{
		kb._c.push_back(0);
		kb._vk.push_back(TCODK_0);
	}

	//		Open the keybindings file.
	string fname = LOADER_DATA_FOLDER;
	fname		+= LOADER_KEYS_FILE;
	ifstream f(fname);

	//		Read in all key-codes.
	if (f.is_open())
	{
		string line;
		while (getline(f, line))
		{
			if (!line.empty())
			{
				//		Split at the ' mark
				auto data	= splitLine(line, '=');

				//		Strip out all the whitespace.
				data.first	= stripWhitespace(data.first);
				data.second = stripWhitespace(data.second);
				cout << data.first << ',' << data.second << endl;

				//		Interpret the first part.
				auto ka = getKeyActionFromTag(data.first);

				//		Interpret the second part.
				//	If it's only 1 char long, use it as a character.
				if (data.second.size() == 1)
					kb._c[ka] = data.second[0];
				else
					kb._vk[ka] = getKeycodeFromTag(data.second);
			}
		}
	}

	//		ERROR!
	else
		cout << "Keybindings file " << fname << " is missing!" << endl;

	//		If the file didn't open, this will be empty.
	return kb;
}