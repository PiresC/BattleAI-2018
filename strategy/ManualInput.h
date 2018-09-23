#include "MoveStrategy.h"

#include "../game/Map.h"

class Map;

class ManualInput :	public MoveStrategy
{
public:
	std::string move(Map* map);

	ManualInput(char team);
	~ManualInput();
private:
	bool validateMove(std::string move);
};
