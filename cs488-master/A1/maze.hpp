// Winter 2020

#pragma once

#include <vector>
#include <tuple>

class Maze
{
public:
	Maze( size_t dim );
	~Maze();

	int start;

	void reset();
	size_t getDim() const;
	int getValue( int x, int y ) const;
	void setValue( int x, int y, int h );

	int numBlocks();
	std::vector<std::tuple<int,int>> blocks();

	void digMaze();
	void printMaze(); // for debugging
private:
	size_t m_dim;
	int *m_values;
	void recDigMaze(int r, int c);
	int numNeighbors(int r, int c);
};
