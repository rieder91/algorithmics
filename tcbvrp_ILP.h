#ifndef __TCBVRP_ILP__H__
#define __TCBVRP_ILP__H__

#include "Tools.h"
#include "Instance.h"
#include <ilcplex/ilocplex.h>
#include <vector>
#include <list>

using namespace std;
ILOSTLBEGIN

class tcbvrp_ILP
{

	typedef IloArray<IloIntVarArray> IloIntVarArray2;
	typedef IloArray<IloBoolVarArray> IloBoolVarArray2;

private:

	Instance& instance;
	string model_type;

	unsigned int n; // Number of Stations + Depot
	unsigned int a; // Number of arcs
	unsigned int m; // Number of Vehicles
	unsigned int T; // Time budget

	IloEnv env;
	IloModel model;
	IloCplex cplex;

	// lists containing the respective nodes
	vector<int> supplyNodes;
	vector<int> demandNodes;

	IloBoolVarArray x;
	IloBoolVarArray y;

	void initCPLEX();
	void setCPLEXParameters();

	void modelSCF();
	void modelMCF();
	void modelMTZ();

	u_int getIndexFor(u_int k, u_int i, u_int j, u_int l);
	u_int getIndexFor(u_int k, u_int i, u_int j);
	u_int getIndexFor(u_int k, u_int i);
	void init();
	void addConstraints();

public:

	tcbvrp_ILP( Instance& _instance, string _model_type);
	~tcbvrp_ILP();
	void solve();

};

#endif //__TCBVRP_ILP__H__
