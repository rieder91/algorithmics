#include "tcbvrp_ILP.h"

tcbvrp_ILP::tcbvrp_ILP( Instance& _instance, string _model_type) :
instance( _instance ), model_type( _model_type )
{
	//Number of stations + depot
	n = instance.n;
	//Number of edges
	a = instance.nArcs;
	//Max. Number of vehicles
	m = instance.m;
	//Max. time limit
	T = instance.T;
}

tcbvrp_ILP::~tcbvrp_ILP()
{
	// free CPLEX resources
	cplex.end();
	model.end();
	env.end();
}

void tcbvrp_ILP::solve()
{
	try {
		// initialize CPLEX solver
		env = IloEnv();
		model = IloModel( env );

		// add model-specific constraints
		if( model_type == "scf" )
			modelSCF();
		else if( model_type == "mcf" )
			modelMCF();
		else if( model_type == "mtz" )
			modelMTZ();

		// build model
		cplex = IloCplex( model );

		// export model to a text file
		//cplex.exportModel( "model.lp" );

		// set parameters
		setCPLEXParameters();

		// solve model
		cout << "Calling CPLEX solve ...\n";
		cplex.solve();
		cout << "CPLEX finished." << "\n\n";
		cout << "CPLEX status: " << cplex.getStatus() << "\n";
		cout << "Branch-and-Bound nodes: " << cplex.getNnodes() << "\n";
		cout << "Objective value: " << cplex.getObjValue() << "\n";
		cout << "CPU time: " << Tools::CPUtime() << "\n\n";

		//Copied from: http://orinanobworld.blogspot.co.at/2010/08/iterating-over-cplex-model-sequel.html
  		for (IloIterator<IloNumVar>  it(env); it.ok(); ++it) {
  			try {
  				IloNumVar ext = *it;
  				if(cplex.getValue(ext)!=0)
  					cout << ext.getName() << ": " << cplex.getValue(ext) << "\n";
  				} catch (IloException &e){
  					//cerr << "Exception for variable " << e << "\n";
  				}
  		}

		if( false ) {
			// TODO optionally output the values of the variables
		}
	}
	catch( IloException& e ) {
		cerr << "tcbvrp_ILP: exception " << e << "\n";
		exit( -1 );
	}
	catch( ... ) {
		cerr << "tcbvrp_ILP: unknown exception.\n";
		exit( -1 );
	}
}

// ----- private methods -----------------------------------------------

void tcbvrp_ILP::setCPLEXParameters()
{
	// TODO impr0ve

	// print every line of node-log and give more details
	//cplex.setParam( IloCplex::MIPInterval, 1 );
	//cplex.setParam( IloCplex::MIPDisplay, 2 );

	// deactivate CPLEX general-purpose cuts
	//	cplex.setParam( IloCplex::EachCutLim, 0 );
	//	cplex.setParam( IloCplex::FracCuts, -1 );

	// only use a single thread
	cplex.setParam( IloCplex::Threads, 1 );
	// set time limit for cplex (in seconds)
	cplex.setParam( IloCplex::TiLim, 3600);
}

void tcbvrp_ILP::modelSCF()
{
	cout << "Building SCF model\n";

	cout << "There are " << n << " nodes \n";
	cout << "There are " << m << " cars \n";

	// lists containing the respective nodes
	vector<int> supplyNodes;
	vector<int> demandNodes;

	// we start from 1 because 0 is the depot node
	for (unsigned int i = 0; i < n; i++) {
		if (instance.isSupplyNode(i)) {
			supplyNodes.push_back(i);
			cout << i << " is a supply node\n";
		} else if (instance.isDemandNode(i)) {
			cout << i << " is a demand node\n";
			demandNodes.push_back(i);
		}
	}

	cout << "There are " << supplyNodes.size() << " supply nodes\n";
	cout << "There are " << demandNodes.size() << " demand nodes\n";


	cout << "Creating the \"main\" decision variable\n";
	IloBoolVarArray x = IloBoolVarArray(env, m * n * n);
	u_int x_cnt = 0;
	for (u_int k = 0; k < m; k++) {
		for (u_int i = 0; i < n; i++) {
			for (u_int j = 0; j < n; j++) {
				x[x_cnt] = IloBoolVar(env, Tools::indicesToString("x", k, i, j).c_str());

				// verify that our getIndexFor() method works
				if (getIndexFor(k, i, j) != x_cnt) {
					cout << "\n\ngetIndexFor() failed!!!\n";
					cout << getIndexFor(k, i, j) << "/" << x_cnt;
				}

				x_cnt++;
			}
		}
	}


	cout << "Creating the decision variable to indicate whether a car drives around\n";
	IloBoolVarArray y = IloBoolVarArray(env, m);
	u_int y_cnt = 0;
	for (u_int k = 0; k < m; k++) {
		y[y_cnt] = IloBoolVar(env, Tools::indicesToString("y", k).c_str());
		y_cnt++;
	}


	// we minimize the total travelling time
	cout << "Adding the objective function\n";
	IloExpr e_objective(env);
	for (u_int k = 0; k < m; k++) {
		for (u_int i = 0; i < n; i++) {
			for (u_int j = 0; j < n; j++) {
				e_objective += x[getIndexFor(k, i, j)] * instance.getDistance(i, j);
			}
		}
	}
	model.add(IloMinimize(env, e_objective));
	e_objective.end();


	cout << "const1 - Each demand node is visited once\n";
	for (u_int d = 0; d < demandNodes.size(); d++) {
		IloExpr e_demand_visited_once(env);
		for (u_int k = 0; k < m; k++) {
			for (u_int i = 0; i < n; i++) {
				e_demand_visited_once += x[getIndexFor(k, i, demandNodes[d])];
			}
		}
		model.add(e_demand_visited_once == 1);
		e_demand_visited_once.end();
	}


	cout << "const2 - Each supply node is visited at most once\n";
	for (u_int s = 0; s < supplyNodes.size(); s++) {
		IloExpr e_supply_visited_once(env);
		for (u_int k = 0; k < m; k++) {
			for (u_int i = 0; i < n; i++) {
				e_supply_visited_once += x[getIndexFor(k, i, supplyNodes[s])];
			}
		}
		model.add(e_supply_visited_once <= 1);
		e_supply_visited_once.end();
	}


	cout << "const3 - Each car that enters a node, leaves it again\n";
	for (u_int n1 = 0; n1 < n; n1++) {
		for (u_int k = 0; k < m; k++) {
			IloExpr e_entries_equals_leavers(env);
			for (u_int j = 0; j < n; j++) {
				e_entries_equals_leavers += x[getIndexFor(k, j, n1)];
				e_entries_equals_leavers -= x[getIndexFor(k, n1, j)];
			}
			model.add(e_entries_equals_leavers == 0);
			e_entries_equals_leavers.end();
		}
	}


	cout << "const4 - No car can go from a supply node to another supply node\n";
	IloExpr e_deny_supply_to_supply(env);
	for (u_int k = 0; k < m; k++) {
		for (u_int i = 0; i < supplyNodes.size(); i++) {
			for (u_int j = 0; j < supplyNodes.size(); j++) {
				e_deny_supply_to_supply += x[getIndexFor(k, supplyNodes[i], supplyNodes[j])];
			}
		}
	}
	model.add(e_deny_supply_to_supply == 0);
	e_deny_supply_to_supply.end();


	cout << "const5 - No car can go from a demand node to another demand node\n";
	IloExpr e_deny_demand_to_demand(env);
	for (u_int k = 0; k < m; k++) {
		for (u_int i = 0; i < demandNodes.size(); i++) {
			for (u_int j = 0; j < demandNodes.size(); j++) {
				e_deny_demand_to_demand += x[getIndexFor(k, demandNodes[i], demandNodes[j])];
			}
		}
	}
	model.add(e_deny_demand_to_demand == 0);
	e_deny_demand_to_demand.end();


	cout << "const6 - No car goes from the depot to a demand node\n";
	IloExpr e_deny_depot_to_demand(env);
	for (u_int k = 0; k < m; k++) {
		for (u_int d = 0; d < demandNodes.size(); d++) {
			e_deny_depot_to_demand += x[getIndexFor(k, 0, demandNodes[d])];
		}
	}
	model.add(e_deny_depot_to_demand == 0);
	e_deny_depot_to_demand.end();


	cout << "const7 - The last node must not be a supply node\n";
	IloExpr e_deny_supply_to_depot(env);
	for (u_int k = 0; k < m; k++) {
		for (u_int s = 0; s < supplyNodes.size(); s++) {
			e_deny_supply_to_depot += x[getIndexFor(k, supplyNodes[s], 0)];
		}
	}
	model.add(e_deny_supply_to_depot == 0);
	e_deny_supply_to_depot.end();


	cout << "const8 - If a car leaves the depot, it has to come back\n";
	for (u_int k = 0; k < m; k++) {
		IloExpr e_depot_exit_equals_entry(env);
		for (u_int d = 0; d < demandNodes.size(); d++) {
			e_depot_exit_equals_entry += x[getIndexFor(k, demandNodes[d], 0)];
		}
		for (u_int s = 0; s < supplyNodes.size(); s++) {
			e_depot_exit_equals_entry -= x[getIndexFor(k, 0, supplyNodes[s])];
		}
		model.add(e_depot_exit_equals_entry == 0);
		e_depot_exit_equals_entry.end();
	}


	cout << "const9 - Each car leaves the depot once\n";
	for (u_int k = 0; k < m; k++) {
		IloExpr e_car_leaves_once(env);
		for (u_int s = 0; s < supplyNodes.size(); s++) {
			e_car_leaves_once += x[getIndexFor(k, 0, supplyNodes[s])];
		}
		model.add(e_car_leaves_once <= 1);
		e_car_leaves_once.end();
	}


	cout << "const10 - No cycles allowed\n";
	IloExpr e_deny_cycles(env);
	for (u_int k = 0; k < m; k++) {
		for (u_int i = 0; i < n; i++) {
			e_deny_cycles += x[getIndexFor(k, i, i)];
		}
	}
	model.add(e_deny_cycles == 0);
	e_deny_cycles.end();


	cout << "const11 - Each car stays within the time limit\n";
	for (u_int k = 0; k < m; k++) {
		IloExpr e_car_within_time_limit(env);
		for (u_int i = 0; i < n; i++) {
			for (u_int j = 0; j < n; j++) {
				e_car_within_time_limit += (x[getIndexFor(k, i, j)] * instance.getDistance(i, j));
			}
		}

		model.add(e_car_within_time_limit < T);
		e_car_within_time_limit.end();
	}


	cout << "const12 - y gets set if the car drives around\n";
	for (u_int k = 0; k < m; k++) {
		IloExpr e_car_drives_around(env);
		for (u_int i = 0; i < n; i++) {
			for (u_int j = 0; j < n; j++) {
				e_car_drives_around += x[getIndexFor(k, i, j)];
			}
		}
		model.add((e_car_drives_around > 0) == y[k]);
		e_car_drives_around.end();
	}


	cout << "const13 - If a cars drives a tour, he has to visit the depot\n";
	for (u_int k = 0; k < m; k++) {
		IloExpr e_car_uses_depot(env);
		for (u_int s = 0; s < supplyNodes.size(); s++) {
			e_car_uses_depot += x[getIndexFor(k, 0, supplyNodes[s])];
		}
		model.add(e_car_uses_depot == y[k]);
		e_car_uses_depot.end();
	}


	//
	//
	// END OF GENERAL PART
	// BEGIN OF SCF
	//
	//


	cout << "Creating the SCF decision variable\n";
	IloIntVarArray f = IloIntVarArray(env, m * n * n);
	u_int f_cnt = 0;
	for (u_int k = 0; k < m; k++) {
		for (u_int i = 0; i < n; i++) {
			for (u_int j = 0; j < n; j++) {
				f[f_cnt] = IloIntVar(env, Tools::indicesToString("f", k, i, j).c_str());

				// verify that our getIndexFor() method works
				if (getIndexFor(k, i, j) != f_cnt) {
					cout << "\n\ngetIndexFor() failed!!!\n";
					cout << getIndexFor(k, i, j) << "/" << f_cnt;
				}

				f_cnt++;
			}
		}
	}


	cout << "scf1 - Outflow from depot for each car\n";
	for (u_int k = 0; k < m; k++) {
		IloExpr e_scf_outflow_from_depot(env);

		for (u_int i = 0; i < supplyNodes.size(); i++) {
			e_scf_outflow_from_depot += f[getIndexFor(k, 0, supplyNodes[i])];
		}

		model.add(e_scf_outflow_from_depot == ((int) demandNodes.size()) * y[k]);
		e_scf_outflow_from_depot.end();
	}



	cout << "scf2 - consumation properties\n";
	for (u_int k = 0; k < m; k++) {
		// skip the depot node 0
		for (u_int j = 1; j < n; j++) {
			IloExpr e_scf_consumation(env);

			for (u_int i = 0; i < n; i++) {
				if (i != j) {
					e_scf_consumation += f[getIndexFor(k, i, j)];
				}
			}
			for (u_int k1 = 0; k1 < n; k1++) {
				if (k1 != j) {
					e_scf_consumation -= f[getIndexFor(k, j, k1)];
				}
			}


			if (instance.isSupplyNode(j)) {
				model.add(e_scf_consumation == 0);
			} else if (instance.isDemandNode(j)) {
				model.add(e_scf_consumation == 1 * y[k]);
			}

			e_scf_consumation.end();
		}
	}


	cout << "scf3 - flow constraints\n";
	for (u_int k = 0; k < m; k++) {
		for (u_int i = 0; i < n; i++) {
			for (u_int j = 0; j < n; j++) {
				if (i != j) {
					IloExpr e_scf_range(env);

					for (u_int k1 = 0; k1 < m; k1++) {
						e_scf_range += x[getIndexFor(k1, i, j)];
					}
					e_scf_range *= ((int) demandNodes.size());

					model.add(f[getIndexFor(k, i, j)] >= 0);
					model.add(f[getIndexFor(k, i, j)] <= e_scf_range);

					e_scf_range.end();
				}
			}
		}
	}
}

void tcbvrp_ILP::modelMCF()
{
	cout << "Building MCF model\n";
	// ++++++++++++++++++++++++++++++++++++++++++
	// TODO build multi commodity flow model
	// ++++++++++++++++++++++++++++++++++++++++++
}

void tcbvrp_ILP::modelMTZ()
{
	cout << "Building MTZ model\n";
	// ++++++++++++++++++++++++++++++++++++++++++
	// TODO build Miller-Tucker-Zemlin model
	// ++++++++++++++++++++++++++++++++++++++++++
}


u_int tcbvrp_ILP::getIndexFor(u_int k, u_int i, u_int j) {
	u_int idx = 0;

	if (k > 0) {
		idx += n*n*k;
	}

	if (i > 0) {
		idx += n*i;
	}

	if (j > 0) {
		idx += j;
	}

	return idx;
}