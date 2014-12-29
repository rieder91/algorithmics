#ifndef __INSTANCE__H__
#define __INSTANCE__H__

#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <list>
#include <fstream>
#include <cstring>
#include <cstdlib>

using namespace std;

/** A class for TCBVRP instances. */
class Instance {

private:
	// travel times between nodes
	std::vector<double> t;

	// store supply and demand nodes separately
	std::vector<int> supplyNodes;
	std::vector<int> demandNodes;

public:

	// total number of nodes (depot plus stations)
	int n;

	// time limit (per vehicle)
	int T;

	/// number of vehicles
	int m;

	// get distance between two nodes
	double getDistance(int i, int j) { return t[(i*n+j)]; };


	struct Arc
	{
		unsigned int v1, v2; //arc from v1 to v2
		int weight;
	};

	// arc variables
	unsigned int nArcs;
	// array of edges
	vector<Arc> arcs;
	// incident edges denoted by index in vector <edges>
	vector<list<unsigned int> > incidentArcs;


	// loads TCBVRP instance in the specified filename.
	void initialize( const std::string &fname );

	//Constructor
	Instance( const std::string &fname) {
		initialize(fname);
	};

	//standard constructor, initialize must be called
	Instance() {};

	// Returns true if node s is a supply node
	bool isSupplyNode(int s) {
		for (uint i = 0; i < supplyNodes.size(); i++) {
			if (supplyNodes[i] == s)
				return true;
		}
		return false;
	}

	// Returns true if node s is a demand node
	bool isDemandNode(int s){
		for (uint i = 0; i < demandNodes.size(); i++) {
			if (demandNodes[i] == s)
				return true;
		}
		return false;
	}

};

#endif //__INSTANCE__H__
