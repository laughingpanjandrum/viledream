#include "dataLoader.h"


//	Input a vector of directories, ending with the file name.
//	Opens the file as an ifstream & returns it, or nullptr if the file isn't found.
ifstream * dataLoader::openDataFile(const vector<string> directories)
{
	//	assemble the filename
	string fname = "";
	for (auto dr : directories)
		fname += dr + "/";
	fname.pop_back();	//	remove the final '/'

	//	try opening the file
	auto f = new ifstream(fname);
	if (f->is_open())
		return f;
	else
	{
		cout << "ERROR: File named " << fname << " failed to open!" << endl;
		return nullptr;
	}
}


//	Takes a list of tokens in quote marks, eg: "id1" "value1" "id2" "value2"
//	Tokenizes each element in quotes, ignoring things outside of quotes.
vector<string> dataLoader::tokenize_quoted(const string line)
{
	vector<string> tokens;
	string token = "";
	bool in_quotes = false;
	for (unsigned i = 0; i < line.size(); i++)
	{
		if (line[i] == '"')
		{
			if (in_quotes)
			{
				tokens.push_back(token);
				in_quotes = false;
			}
			else
			{
				in_quotes = true;
				token = "";
			}
		}
		else if (in_quotes)
			token += line[i];
	}
	return tokens;
}


//	Return a vector of string tokens, created from the given line, splitting at each instance of the given character.
//	The tokenizing character is excluded from the tokens.
//	ie the string "big,small,happy,sad" would return { "big", "small", "happy", "sad" }
//	it is permissable to end the string with the tokenizing character.
vector<string> dataLoader::tokenize(const string line, const char at)
{
	vector<string> tokens;
	string token = "";
	for (auto c : line)
	{
		if (c == at)
		{
			tokens.push_back(token);
			token = "";
		}
		else
			token += c;
	}
	if (!token.empty())
		tokens.push_back(token);
	return tokens;
}


//	Removes whitespace from the beginning AND end of the line.
//	Internal whitespace (anything occuring before the 1st non-ws character & after the last) is left in.
const string dataLoader::strip_whitespace(const string line)
{
	if (line.empty())
		return "";

	string st = "";

	//	front
	for (unsigned i = 0; i < line.size(); i++)
	{
		if (!is_whitespace(line[i]))
		{
			st = line.substr(i, line.size());
			break;
		}
	}

	if (st.empty())
		return "";

	//	back
	for (unsigned i = st.size() - 1; i >= 0; i--)
	{
		if (!is_whitespace(st[i]))
		{
			st = st.substr(0, i + 1);
			break;
		}
	}

	return st;
}

bool dataLoader::is_whitespace(const char c)
{
	return c == ' ' || c == '\t';
}


//	Load a single monster from the current stream. Ends when we find a '}' line.
monsterDataPtr dataLoader::loadMonsterData(ifstream * f)
{
	string line;
	auto mdp = monsterDataPtr(new monsterData());
	while (getline(*f, line))
	{
		//	we did it, the data is complete
		if (strip_whitespace(line) == "}")
			return mdp;

		//	add new tokens
		else
		{
			for (auto t : tokenize_quoted(line))
				mdp->_tokens.push_back(t);
		}
	}

	//	oops, file ended before we found a terminating character! BAD
	cout << "ERROR: File ended before reaching a terminating character! " << endl;
	return mdp;
}


//	Load a monster data file.
void dataLoader::loadMonsterFile(loadedFileDataPtr ldata, ifstream * f)
{
	string line;
	while (getline(*f, line))
	{
		//	start of a monster
		if (strip_whitespace(line) == "{")
		{
			//	get the list of tokens
			auto mdp = loadMonsterData(f);

			//	once we have all the tokens, sort through and find the ID and DL
			for (unsigned i = 0; i < mdp->_tokens.size(); i++)
			{
				if (mdp->_tokens[i] == "id")
					mdp->_id = mdp->_tokens[i + 1];
				else if (mdp->_tokens[i] == "dl")
					mdp->_dangerLevel = stoi(mdp->_tokens[i + 1]);
			}
			cout << endl;

			//	testing
			cout << "Loaded monster with id " << mdp->_id << ", danger level " << mdp->_dangerLevel << endl;
			ldata->_monsterData.push_back(mdp);
		}
	}
	f->close();
}


monsterTableType dataLoader::getTableTypeFromTag(const string tag)
{
	if		(tag == "pack")		return MTABLE_PACK;
	else if (tag == "solo")		return MTABLE_SOLO;
	else						return MTABLE__NONE;
}


//	Load a monster table from the current line in the file
monsterTablePtr dataLoader::loadMonsterTable(ifstream * f)
{
	auto mt = monsterTablePtr(new monsterTable());
	string line;
	vector<string> tokens;

	//	get tokens from each line
	while (getline(*f, line))
	{
		if (strip_whitespace(line) == "}")
			break;
		else if (!line.empty())
		{
			for (auto t : tokenize_quoted(line))
				tokens.push_back(t);
		}
	}

	//	parse tokens
	for (unsigned t = 0; t < tokens.size(); t++)
	{
		if (tokens[t] == "id")
			mt->_zone = tokens[++t];
		else if (tokens[t] == "type")
			mt->_mtype = getTableTypeFromTag(tokens[++t]);
		else
			mt->_ids.push_back(tokens[t]);
	}

	//	make sure we got all the required data
	if (mt->_zone.empty())
		cout << "ERROR: Monster table didn't provide a zone id" << endl;
	if (mt->_mtype == MTABLE__NONE)
		cout << "ERROR: Monster table didn't give a type" << endl;
	if (mt->_ids.empty())
		cout << "ERROR: Monster table didn't contain any monsters" << endl;
	
	return mt;
}


//	All monster tables come from this file
void dataLoader::loadMonsterTables(loadedFileDataPtr ldata, ifstream * f)
{
	string line;
	while (getline(*f, line))
	{
		if (strip_whitespace(line) == "{")
		{
			auto mt = loadMonsterTable(f);
			ldata->_monsterTables.push_back(mt);
			cout << "Loaded monster table for zone " << mt->_zone << endl;
		}
	}
	f->close();
}


//	Loads everything from every data file.
void dataLoader::loadAll(loadedFileDataPtr ldata)
{
	//	Monster data
	auto df = openDataFile({ LOADER_DATA_FOLDER, LOADER_MONSTER_FOLDER, LOADER_DATA_MASTER_FILE });
	if (df != nullptr)
	{
		string line;
		while (getline(*df, line))
		{
			auto f = openDataFile({ LOADER_DATA_FOLDER, LOADER_MONSTER_FOLDER, line });
			if (f->is_open())
			{
				cout << "Loading " << line << "..." << endl;
				loadMonsterFile(ldata, f);
			}
			else
				cout << "ERROR: Couldn't find monster data file " << line << endl;
		}
	}
	else
		cout << "ERROR: Couldn't find master data file!" << endl;


	//	Monster tables (all from a single file)
	df = openDataFile({ LOADER_DATA_FOLDER, LOADER_MONSTER_FOLDER, LOADER_MONSTER_TABLES });
	if (df != nullptr)
		loadMonsterTables(ldata, df);
	else
		cout << "ERROR: Couldn't find monster tables file!" << endl;
}
