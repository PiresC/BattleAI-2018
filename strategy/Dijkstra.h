#include <vector>

#include "..\game\Coor.h"

class Dijkstra {
public :
	char output[100];
	void startPathFinding(char grid[5][5], Coor from, Coor to);

	static bool isOutofBound(Coor nextCoor);
	static std::vector<Coor> findPath(Coor parent[5][5], Coor from, Coor to);
	static std::pair<int, std::vector<Coor> > dijkstraPathFinding(char grid[5][5], Coor from, Coor to);

	Dijkstra();
	Dijkstra(char grid[5][5],Coor from, Coor to);
	~Dijkstra();

	std::pair<int, std::vector<Coor> > ret;
};