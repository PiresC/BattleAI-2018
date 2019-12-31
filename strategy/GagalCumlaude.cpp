#include "GagalCumlaude.h"

#include <iostream>
#include <algorithm>

#include <vector>

/*
	DISCLAIMER
	THIS CODE IS A MESS AND WE HAVE NO IDEA ON HOW TO FIX IT DUE TO LIMITED RESOURCES
	MAINLY BECAUSE WE'RE NOT PROFICIENT USING BOOST

	#gagalcumlaude team
	- Stanley Marvin - 2101654560
	- Dessy Mae Setiawan - 2101669152
	- Herbi Moria - 2101627686
	- Jessen Adrianus Reynaldi - 2101671226 
	- Valentino Mendrofa - 2101695926
	- Vincent Sugata - 2101656433
*/

namespace sc = boost::statechart;
namespace mpl = boost::mpl;


//GLOBAL VARIABLES
std::vector<Dijkstra> pathToMine;

std::vector<Dijkstra> pathToGuardMine; //what its says, paths for unit to protect mining units

std::vector<Coor> baseLocation;
std::vector<Coor> unitLocation;
std::vector<Coor> unitSafeGuard; //Location of units 2 steps before mine
std::vector<Coor> mineLocation;

std::vector<Coor> baseLocationEnemy;

int mineFound;
int unitNearMine;

char player;

int totalMine = 0;

Coor from;
Coor to;

Coor activePoint;
std::vector<Coor> unitMiningLocation;

int activePath;

char zone[5][5];

struct DistanceCompare {
	typedef Dijkstra value_type;
	bool operator()(const value_type& lhs, const value_type& rhs)
	{
		return lhs.ret.first < rhs.ret.first;
	}
}; //COMPARING DISTANCE TRAVELED TO MINES

//FSM Declarations
struct Root;
struct StartGame;
struct EarlyGame;
struct EarlyGameFindMine;
struct MidGame;
struct MidGameGuardUnit;
struct MidGameSpreadUnit;
struct MidGameSecureBaseMine;
struct LateGame;
struct LateGameBaseBuilding;
struct LateGameAttackBase;
struct DangerPhase;

struct EvEnemyNear : sc::event<EvEnemyNear> {
	EvEnemyNear(){}
};
struct EvReset : sc::event<EvReset> {
	EvReset(){}
};
struct EvPhaseChangeEarly : sc::event<EvPhaseChangeEarly> {
	EvPhaseChangeEarly(){}
};
struct EvPhaseChangeMid : sc::event<EvPhaseChangeMid> {
	EvPhaseChangeMid(){}
};
struct EvPhaseChangeLate : sc::event<EvPhaseChangeLate> {
	EvPhaseChangeLate(){}
};
struct EvInnerPhase : sc::event<EvInnerPhase> {
	EvInnerPhase(){}
};
struct EvInnerPhaseAlt : sc::event<EvInnerPhaseAlt> {
	EvInnerPhaseAlt(){}
};

struct Machine : sc::state_machine<Machine, Root> {} brain;

bool checkEnemyUnitPresenceStraight(std::vector<Coor> baseLocation, char zone[5][5], char enemyUnit );
void mineCheck();

std::string GagalCumlaude::move(Map* map)
{
	using namespace std;
	
	map->getMap(zone);
	updateGameCondition(map);

	brain.initiate();

	char input[5];

	if(turn == 1) {
		brain.process_event( EvReset() );
		brain.process_event( EvReset() );
		brain.process_event( EvPhaseChangeEarly() );
	}

	else if(checkEnemyUnitPresenceStraight(baseLocation, zone, tolower(enemySymbol))) {
		brain.process_event( EvEnemyNear() );

		if(zone[to.getX()][to.getY()] != '-') {
			cout << "False Danger..Continue.." << endl;
			brain.process_event( EvReset() );

			if(turn <= 7) {
				
				if (mineFound >= (totalMine / 2) + 1 && totalPlayerMaterial >= totalPlayerMaterial - 10 + 2) {
					brain.process_event( EvPhaseChangeMid() );
				}
				else brain.process_event( EvPhaseChangeEarly() );
			}
			else if(turn <= 15) {
				brain.process_event( EvPhaseChangeMid() );
				if(unitSafeGuard.empty() == true) brain.process_event( EvInnerPhaseAlt() );
				if(unitNearMine == -1) brain.process_event( EvInnerPhase() );
			}
			else {
				brain.process_event( EvPhaseChangeLate() );
			}
		}
	}

	else {
		
		mineCheck();

		// cout << "Total Mine Found : " << mineFound << endl;
		// cout << "Total Mine in Map : " << totalMine << endl;
		// cout << "Unit Near Mine : " << unitNearMine << endl;
		// cout << "Current Active Path : " << activePath << endl;

		if(turn <= 7) {
			if (mineFound >= (totalMine / 2) + 1 && totalPlayerMaterial >= totalPlayerMaterial - 10 + 2) {
					brain.process_event( EvPhaseChangeMid() );
			}
			else brain.process_event( EvPhaseChangeEarly() );
		}
		else if(turn <= 15) {
			brain.process_event( EvPhaseChangeMid() );
			if(unitSafeGuard.empty() == true) brain.process_event( EvInnerPhaseAlt() );
			if(unitNearMine == -1) brain.process_event( EvInnerPhase() );
		}
		else {
			brain.process_event( EvPhaseChangeLate() );
		}
	}
	

	convertCoorToString(input, from, to);
	cout << "Input : " << string(input).substr(0,5) << endl;

	turn++;
	baseLocation.clear();
	unitLocation.clear();
	mineLocation.clear();
	baseLocationEnemy.clear();

	totalMine = 0;
	totalPlayerBase = 0;
	totalPlayerUnit = 0;
	totalEnemyBase = 0;
	totalEnemyUnit = 0;

	from.setX(-1); from.setY(-1); to.setX(-1); to.setX(-1);

	return string(input);
}

void GagalCumlaude::getTeamSymbol() {
	if(team == 'A') {
		playerTeamSymbol = 0; enemyTeamSymbol = 1; enemySymbol =  'B'; player = 'A';
	}

	else {
		playerTeamSymbol = 1; enemyTeamSymbol = 0; enemySymbol =  'A'; player = 'B';
	}
}

void GagalCumlaude::updateGameCondition(Map* map) {

	totalMine = 0;

	for(int y = 0 ; y < 5 ; ++y) {
		for(int x = 0 ; x < 5 ; ++x) {
			Coor pointer(x,y);

			if(zone[x][y] == 'M') {
				mineLocation.push_back(pointer);
				totalMine++;
			}
			else if(zone[x][y] == GagalCumlaude::team) {
				baseLocation.push_back(pointer);
				totalPlayerBase++;
			}
			else if(zone[x][y] == tolower(GagalCumlaude::team)) {
				unitLocation.push_back(pointer);
				totalPlayerUnit++;
			}
			else if(zone[x][y] == GagalCumlaude::enemySymbol) {
				baseLocationEnemy.push_back(pointer);
				totalEnemyBase++;
			}
			else if(zone[x][y] == tolower(GagalCumlaude::enemySymbol)) totalEnemyUnit;
		}
	}

	totalPlayerMaterial = map->getTeams().at(playerTeamSymbol)->getMaterial();

	totalPlayerScore = totalPlayerBase * 20 + totalPlayerUnit * 2 + totalPlayerMaterial * 1;

	totalEnemyMaterial = map->getTeams().at(enemyTeamSymbol)->getMaterial();

	totalEnemyScore = totalEnemyBase * 20 + totalEnemyUnit * 2 + totalEnemyMaterial * 1;
}

void GagalCumlaude::convertCoorToString(char input[5], Coor from, Coor to) {
	input[0] = (from.getY() + 'A');
	input[1] = (from.getX() + '1');
	input[2] = '-';
	input[3] = (to.getY() + 'A');
	input[4] = (to.getX() + '1');
}

//THIS FUNCTION WILL CHECK THE PRESENCE OF ENEMY UNIT FROM EVERY BASE
bool checkEnemyUnitPresenceStraight(std::vector<Coor> baseLocation, char zone[5][5], char enemyUnit ) {
	for(auto it = baseLocation.begin(); it != baseLocation.end(); ++it) {

		if( (*it).getX() - 2 >= 0)
		if( zone[(*it).getX() - 2][(*it).getY()] == enemyUnit) {
			to.setX((*it).getX() - 1); to.setY((*it).getY());
		}

		if( (*it).getX() + 2 <= 4)
		if( zone[(*it).getX() + 2][(*it).getY()] == enemyUnit) {
			to.setX((*it).getX() + 1); to.setY((*it).getY());
		}

		if( (*it).getY() - 2 >= 0)
		if( zone[(*it).getX()][(*it).getY() - 2] == enemyUnit) {
			to.setX((*it).getX()); to.setY((*it).getY() - 1);
		}

		if( (*it).getY() + 2 <= 4)
		if( zone[(*it).getX()][(*it).getY() + 2] == enemyUnit) {
			to.setX((*it).getX()); to.setY((*it).getY() + 1);
		}
		
		if(to.getX() != -1) {
			from = (*it);
			return true;
			break;
		}

	}
	return false;
}

GagalCumlaude::GagalCumlaude(char team) : MoveStrategy(team) {
	getTeamSymbol();
	turn = 1;
	mineFound = 0;
	activePath = 0;
	unitNearMine = 0;

	pathToMine.clear();
	pathToGuardMine.clear();
	unitSafeGuard.clear();
	unitMiningLocation.clear();
}

GagalCumlaude::~GagalCumlaude() {}


//GLOBAL FUNCTIONS, YEA ITS DIRTY...
void initiateMinePathFinding(std::vector<Coor> point1, std::vector<Coor> point2, std::vector<Dijkstra> &path) {

	//FIND SHORTEST TO ALL MINES FROM ALL BASE
	for(auto it1 = point1.begin(); it1 != point1.end(); ++it1) {
		for(auto it2 = point2.begin(); it2 != point2.end(); ++it2) {
			Dijkstra temp(zone, (*it1) , (*it2) );
			path.push_back(temp);
		}
	}

	//SORT DISTANCE TO REACH POINT FROM NEAREST TO FARTHEST
	std::sort(path.begin(), path.end(), DistanceCompare());

	for(auto it = path.begin(); it != path.end(); ++it) {
		std::cout << "Cost : " << (*it).ret.first << std::endl;
	}
}

void moveUnitToMine() {
	from = pathToMine[mineFound].ret.second[activePath];
	to = pathToMine[mineFound].ret.second[activePath+1];
	activePath++;
}

void mineCheck() {

	if(mineFound < totalMine) 
	if(zone[pathToMine[mineFound].ret.second[activePath+1].getX()][pathToMine[mineFound].ret.second[activePath+1].getY()] == 'M') {

		unitMiningLocation.push_back(pathToMine[mineFound].ret.second[activePath]);

		Dijkstra temp(zone, baseLocation[0] , unitMiningLocation.back() );
		pathToGuardMine.push_back(temp);

		mineFound++;
		if(mineFound == totalMine)
			unitNearMine++;
		activePath = 0 ;

		for(auto it = unitMiningLocation.begin(); it != unitMiningLocation.end(); ++it) {
			std::cout << (*it).getX() << ',' << (*it).getY() << std::endl;
		}
	}
}


struct Root : sc::simple_state<Root, Machine> {
	Root() {
		std::cout<< "In Root State" << std::endl;
	}
	typedef mpl::list <
		sc::transition< EvEnemyNear, DangerPhase>,
		sc::transition< EvPhaseChangeEarly, EarlyGame>,
		sc::transition< EvPhaseChangeMid, MidGame>,
		sc::transition< EvPhaseChangeLate, LateGame>,
		sc::transition< EvReset, StartGame >
	> reactions;
};

struct StartGame : sc::simple_state<StartGame, Machine> {
	StartGame() {
		std::cout<< "In StartGame State" << std::endl;
		initiateMinePathFinding(baseLocation, mineLocation, pathToMine);
	}
	~StartGame() {}
	typedef sc::transition< EvReset, Root> reactions;
};

struct EarlyGame : sc::simple_state<EarlyGame, Machine> {
	EarlyGame() {
		std::cout<< "In EarlyGame State" << std::endl;
		moveUnitToMine();
	}
	~EarlyGame() {}
	
	typedef sc::transition< EvReset, Root> reactions;
};

struct MidGame : sc::simple_state< MidGame, Machine, MidGameGuardUnit > {
	MidGame() {
		std::cout<< "In MidGame State" << std::endl;
	}
	~MidGame(){}
	typedef sc::transition< EvReset, Root > reactions;
};

struct MidGameGuardUnit : sc::simple_state< MidGameGuardUnit, MidGame > {
	MidGameGuardUnit() {
		std::cout<< "Guarding Mining units" << std::endl;
		checkTarget();
		if(unitNearMine != -1) sendUnit();
		else std::cout<< "No Need to send units.." << std::endl;
	}
	~MidGameGuardUnit(){}

	void sendUnit() {
		from = pathToGuardMine[unitNearMine].ret.second[activePath];
		to = pathToGuardMine[unitNearMine].ret.second[activePath+1];
		activePath++;
	}

	void checkTarget() {
		if(unitNearMine >= 0)
		if(	zone[pathToGuardMine[unitNearMine].ret.second[activePath+1].getX()][pathToGuardMine[unitNearMine].ret.second[activePath+1].getY()] == tolower(player)) {
			unitSafeGuard.push_back(pathToGuardMine[unitNearMine].ret.second[activePath]);
			unitNearMine--;
			activePath = 0 ;

			std::cout << "Unit Safe Guard" << std::endl;
			for(auto it = unitSafeGuard.begin(); it != unitSafeGuard.end(); ++it) {
				std::cout << (*it).getX() << ',' << (*it).getY() << std::endl;
			}
		}
	}
	typedef mpl::list <
		sc::transition < EvInnerPhase, MidGameSecureBaseMine>,
		sc::transition < EvInnerPhaseAlt, MidGameSpreadUnit>
		> reactions;
};


struct MidGameSpreadUnit : sc::simple_state< MidGameSpreadUnit, MidGame > {
	MidGameSpreadUnit() {
		std::cout << "Spreading Units..." << std::endl;
		if(from.getX() == -1)
			spreadUnit();
	}
	void spreadUnit() {
		for(auto it = baseLocation.begin(); it != baseLocation.end(); ++it) {
			//std::cout << std::endl <<(*it).getX() << ',' << (*it).getY() << "LOCATION";

			if( (*it).getX() - 1 >= 0)
			if( zone[(*it).getX() - 1][(*it).getY()] == '-') {
				to.setX((*it).getX() - 1); to.setY((*it).getY());
			}

			if( (*it).getX() + 1 <= 4)
			if( zone[(*it).getX() + 1][(*it).getY()] == '-') {
				to.setX((*it).getX() + 1); to.setY((*it).getY());
			}

			if( (*it).getY() - 1 >= 0)
			if( zone[(*it).getX()][(*it).getY() - 1] == '-') {
				to.setX((*it).getX()); to.setY((*it).getY() - 1);
			}

			if( (*it).getY() + 1 <= 4)
			if( zone[(*it).getX()][(*it).getY() + 1] == '-') {
				to.setX((*it).getX()); to.setY((*it).getY() + 1);
			}

			if(to.getX() != -1) {
				from = (*it);
				break;
			}
		}
	}
	~MidGameSpreadUnit(){}
	typedef sc::transition< EvInnerPhase, MidGameSecureBaseMine> reactions;
};

struct MidGameSecureBaseMine : sc::simple_state< MidGameSecureBaseMine, MidGame > {
	MidGameSecureBaseMine() {
		std::cout << "Securing bases near mine..." << std::endl;
		baseBuilding();
	}
	~MidGameSecureBaseMine(){}

	typedef sc::transition < EvInnerPhase, MidGameSpreadUnit > reactions;

	void baseBuilding() {
		if(unitSafeGuard.empty() == false) {
			from = to = unitSafeGuard.back();
			unitSafeGuard.pop_back();
		}
		else std::cout << "Empty" << std::endl;
	}

};

struct LateGame : sc::simple_state< LateGame, Machine, LateGameBaseBuilding > {
	LateGame() {
		std::cout<< "In LateGame State" << std::endl;
	}
	~LateGame(){}
	typedef sc::transition< EvReset, Root> reactions;
};


struct LateGameBaseBuilding : sc ::simple_state< LateGameBaseBuilding, LateGame > {
	LateGameBaseBuilding() {
		std::cout << "Building Base" << std::endl;
		baseBuilding();
	}
	~LateGameBaseBuilding(){}
	void baseBuilding() {
		from = to = unitLocation.front();
	}
	typedef sc::transition< EvReset, Root > reactions;
};

struct DangerPhase : sc::simple_state< DangerPhase, Machine> {
	DangerPhase() {
		std::cout<< "In DangerPhase State" << std::endl;
	}
	~DangerPhase(){}
	typedef sc::transition< EvReset, Root > reactions;
};