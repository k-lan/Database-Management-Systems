/*
code for storage and buffer management
run with: 
g++ -std=c++11 main.cpp -o main.out
./main.out
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

    // Create the EmployeeRelation file from Employee.csv
    StorageBufferManager manager("EmployeeRelation");
    manager.createFromFile("Employee.csv");
    
    // Loop to lookup IDs until user is ready to quit
    int userId = 1;
    while (userId != 0) {
    std::cout << "Press 0 to quit or enter the user ID to search in the data file: ";
    std::cin >> userId;
    // search for that id
    manager.findRecordById(userId);
    }
    return 0;
}
