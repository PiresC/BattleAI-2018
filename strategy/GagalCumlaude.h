#include <boost\statechart\simple_state.hpp>
#include <boost\statechart\state_machine.hpp>
#include <boost\statechart\event.hpp>
#include <boost\statechart\transition.hpp>
#include <boost\statechart\custom_reaction.hpp>

#include "MoveStrategy.h"
#include "Dijkstra.h"

#include "../game/Coor.h"
#include "../game/Map.h"

class Map;

class GagalCumlaude : public MoveStrategy
{

public:
	std::string move(Map* map);
	int turn;
	char enemySymbol;

	GagalCumlaude(char team);
	~GagalCumlaude();

public:
	void getTeamSymbol();
	void updateZone(std::string zone);
	void updateGameCondition(Map* map);
	void convertCoorToString(char input[5], Coor from, Coor to);

private:
	int playerTeamSymbol;
	int enemyTeamSymbol;

	//PS, gtw penghitungan totalbase dll penting ato ga
	int totalPlayerBase;
	int totalPlayerUnit;
	int totalPlayerMaterial;
	int totalPlayerScore;

	int totalEnemyBase;
	int totalEnemyUnit;
	int totalEnemyMaterial;
	int totalEnemyScore;
	//end of useless shit
};