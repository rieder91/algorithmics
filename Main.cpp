#include <iostream>
#include "Tools.h"
#include "Instance.h"
#include "tcbvrp_ILP.h"

using namespace std;

void usage()
{
	cout << "USAGE:\t<program> -f filename -m model \n";
	cout << "EXAMPLE:\t" << "./tcbvrp -f instances/tcbvrp_10_1_T240_m2.prob -m scf \n\n";
	exit( 1 );
}

int main( int argc, char *argv[] )
{
	// read parameters
	int opt;
	// default values
	string file( "instances/tcbvrp_10_1_T240_m2.prob" );
	string model_type( "scf" );
	while( (opt = getopt( argc, argv, "f:m:" )) != EOF ) {
		switch( opt ) {
			case 'f': // instance file
				file = optarg;
				break;
			case 'm': // algorithm to use
				model_type = optarg;
				break;
			default:
				usage();
				break;
		}
	}
	// read instance
	Instance instance( file );
	// solve instance
	tcbvrp_ILP ilp( instance, model_type);
	ilp.solve();

	return 0;
}
