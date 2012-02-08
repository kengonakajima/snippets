#include <mysql++.h>


#include <iostream>
#include <iomanip>


using namespace std;


int main(int argc, char *argv[])
{
    // Connect to the sample database.
    mysqlpp::Connection con(false);
	bool x = con.connect("brail_development", "localhost", "storage", "" );
	if( !x ){
		cerr << "ERROR : " << con.error();
		return 0;
	}
    // Retrieve a subset of the sample stock table set up by resetdb
    mysqlpp::Query query = con.query();
    query << "select id from entries";
    mysqlpp::Result res = query.store();

    // Display the result set
    cout << "We have:" << endl;
    if (res) {
        mysqlpp::Row row;
        mysqlpp::Row::size_type i;
        for (i = 0; row = res.at(i); ++i) {
            cout << '\t' << row.at(0) << endl;
        }
    }
    else {
        cerr << "Failed to get item list: " << query.error() << endl;
        return 1;
    }

    return 0;
}
