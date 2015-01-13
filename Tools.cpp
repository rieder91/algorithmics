#include "Tools.h"

string Tools::indicesToString( string prefix, int i, int j, int v, int w)
{
	stringstream ss;
	ss << prefix << "(" << i;
	if (j >= 0) ss << ',' << j;
	if (v >= 0) ss << ',' << v;
	if (w >= 0) ss << ',' << w;
	ss << ')';
	return ss.str();
}

double Tools::CPUtime()
{
	tms t;
	times( &t );
	double ct = sysconf( _SC_CLK_TCK );
	return t.tms_utime / ct;
}
