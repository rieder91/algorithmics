
#include "Instance.h"

void Instance::initialize( const string &fname )
{
	string d_s = "";
	int d_i;

	// input file stream
	ifstream is( fname.c_str(), ios::in );

	if ( is.is_open() )
	{
		// read number of nodes
		std::getline(is, d_s);
		n = atoi(d_s.c_str()) + 1;

		// read global time limit per route
		getline(is, d_s);
		T = atoi(d_s.c_str());

		// read number of vehicles
		getline(is, d_s);
		m = atoi(d_s.c_str());

		// read supply and demand nodes
		for (int i = 0; i < n - 1; i++) {
			is >> d_i;
			is >> d_s;
			if (d_s == "S")
				supplyNodes.push_back(d_i);
			else
				demandNodes.push_back(d_i);
		}

		// read distances
		for (int i = 0;  i < n * n; i++) {
			is >> d_i;
			t.push_back(d_i);
		}

	}
	else {
		cerr << "Cannot open file " << fname.c_str() << endl;
	}

	is.close();

	//store arcs
	arcs.resize(n*n);
	nArcs = n * n;
	incidentArcs.resize(n);

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			arcs[j + n * i].v1 = i;
			arcs[j + n * i].v2 = j;
			arcs[j + n * i].weight = getDistance(i, j);
			incidentArcs[i].push_back(j + n * i);
			incidentArcs[j].push_back(j + n * i);
		}
	}
}
