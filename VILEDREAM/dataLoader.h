#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "loader_constants.h"
#include "utility.h"

using namespace std;


enum monsterTableType { MTABLE_PACK, MTABLE_SOLO, MTABLE__NONE, };


struct monsterData
{
	string _id;
	int _dangerLevel;
	vector<string> _tokens;
};
typedef shared_ptr<monsterData> monsterDataPtr;


struct monsterTable
{
	string _zone;
	monsterTableType _mtype = MTABLE__NONE;
	vector<string> _ids;
};
typedef shared_ptr<monsterTable> monsterTablePtr;


struct loadedFileData
{
	vector<monsterDataPtr> _monsterData;
	vector<monsterTablePtr> _monsterTables;
};
typedef shared_ptr<loadedFileData> loadedFileDataPtr;



namespace dataLoader
{

	ifstream* openDataFile(const vector<string> directories);

	vector<string> tokenize_quoted(const string line);
	vector<string> tokenize(const string line, const char at = ' ');
	const string strip_whitespace(const string line);
	bool is_whitespace(const char c);


	monsterDataPtr loadMonsterData(ifstream* f);
	void loadMonsterFile(loadedFileDataPtr ldata, ifstream* f);

	monsterTableType getTableTypeFromTag(const string tag);
	monsterTablePtr loadMonsterTable(ifstream* f);
	void loadMonsterTables(loadedFileDataPtr ldata, ifstream* f);

	void loadAll(loadedFileDataPtr ldata);

}