/*
linear hash indexing driver
*/

#include <string>
#include <ios>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include "classes.h"
using namespace std;


int main(int argc, char* const argv[]) {

    // Create the index
    LinearHashIndex emp_index("EmployeeIndex");
    emp_index.createFromFile("Employee.csv");
    
    // emp_index.readPage(6);

    // Loop to lookup IDs until user is ready to quit
    int userId = 1;
    while (userId != 0) {
        cout << "Press 0 to quit or enter the user ID to search in the data file: ";
        cin >> userId;
        // search for that id
        cout << endl;
        emp_index.findRecordById(userId);
        cout << endl;
    }

    return 0;
}
