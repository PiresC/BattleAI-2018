#include <iostream>
#include <queue>
#include <vector>
#include <utility>
#include <stack>

#include "Dijkstra.h"

#define INF 9999

using namespace std;

std::vector<Coor> Dijkstra::findPath(Coor parent[5][5], Coor from, Coor to){
	vector<Coor> path;
	stack<Coor> tempPath;
	Coor tempCoor = to;
	while(tempCoor.getY() != from.getY() || tempCoor.getX() != from.getX()){
		tempPath.push (tempCoor);
		tempCoor = parent[tempCoor.getY()][tempCoor.getX()];
	}
	tempPath.push(from);
	while(!tempPath.empty() ){
		path.push_back(tempPath.top() );
		tempPath.pop();
	}
	return path;
}

bool Dijkstra::isOutofBound(Coor nextCoor) {
	if(nextCoor.getY() >= 0 && nextCoor.getY() < 5 && nextCoor.getX() >= 0 && nextCoor.getX() < 5)return 0;
	return 1;
}

std::pair<int, std::vector<Coor> > dijkstra(char grid[5][5], Coor from, Coor to){
	Coor parent[5][5];
	int distance[5][5];
	//distance from Coor 'from'
	
	int movex[4] = {1, -1, 0 ,0};
	int movey[4] = {0 ,0, 1, -1};
	
	for(int i = 0; i < 5; i++)
		for(int j = 0; j < 5; j++)
			distance[i][j] = INF;
			
	priority_queue<pair<int, pair<int,int> > > pq;
	pq.push(make_pair(0, make_pair(from.getY(), from.getX() ) ) );
	distance[from.getY()][from.getX()] = 0;
	
	while(!pq.empty()){
		int curDistance = -pq.top().first;
		Coor curCoor;
		curCoor.setY(pq.top().second.first );
		curCoor.setX(pq.top().second.second);
		
		for(int i = 0; i < 4; i++){
			Coor nextCoor;
			nextCoor.setY(curCoor.getY() + movey[i]);
			nextCoor.setX(curCoor.getX() + movex[i]);
			//i = 0 -> keatas; i = 1 -> kebawah; i = 2 -> kekiri; i = 3 -> kekanan
			
			if(Dijkstra::isOutofBound(nextCoor) == 0 && grid[nextCoor.getX()][nextCoor.getY()] != '#' && curDistance + 1 < distance[nextCoor.getY()][nextCoor.getX()]){
				distance[nextCoor.getY()][nextCoor.getX()] = curDistance + 1;
				parent[nextCoor.getY()][nextCoor.getX()]  = curCoor;
				pq.push(make_pair(-(curDistance + 1), make_pair(nextCoor.getY(),nextCoor.getX()) ) );
			}
		}
		pq.pop();
	}
	
	vector<Coor>path;
	if(distance[to.getY()][to.getX()] != INF){
		path = Dijkstra::findPath(parent, from, to);
	}
	//kalo distance INF, brti ga ad path ke to
	
	return make_pair(distance[to.getY()][to.getX()], path);
}

void Dijkstra::startPathFinding(char grid[5][5], Coor from, Coor to){
	ret = dijkstra(grid, from, to);
}


Dijkstra::Dijkstra(){}
Dijkstra::Dijkstra(char grid[5][5],Coor from, Coor to){
	this->startPathFinding(grid, from, to);
}
Dijkstra::~Dijkstra(){}
