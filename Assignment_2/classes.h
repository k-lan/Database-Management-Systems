#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <bitset>
using namespace std;

class Record {
public:
    int id, manager_id;
    std::string bio, name;

    Record(vector<std::string> fields) {
        id = stoi(fields[0]);
        name = fields[1];
        bio = fields[2];
        manager_id = stoi(fields[3]);
    }

    void print() {
        cout << "\tID: " << id << "\n";
        cout << "\tNAME: " << name << "\n";
        cout << "\tBIO: " << bio << "\n";
        cout << "\tMANAGER_ID: " << manager_id << "\n";
    }
};


class StorageBufferManager {

private:
    
    const int BLOCK_SIZE = 3; // initialize the  block size allowed in main memory according to the question 
    int numRecords;
    // You may declare variables based on your need 
    //const int BLOCK_SIZE = 4096;
    // Insert new record 
    void insertRecord(Record record) {

        // No records written yet
        if (numRecords == 0) {
            // Initialize first block
            
        }
        // Add record to the block


        // Take neccessary steps if capacity is reached (you've utilized all the blocks in main memory)


    }

public:
    StorageBufferManager(string NewFileName) {
        
        //initialize your variables

        // Create your EmployeeRelation file 

        
    }

    // Read csv file (Employee.csv) and add records to the (EmployeeRelation)
    void createFromFile(string csvFName) {
        ifstream file(csvFName);
        string line;
        vector<Record> records; // TEMPORARY keep records in a vector for testing

        // check if the file opened
        if (!file.is_open()) {
            cerr << "Cannot open file\n";
            return;
        }

        // read each line from the file
        while (getline(file, line)) {
            stringstream ss(line);
            string field;
            vector<string> fields; // record fields

            // split the line into fields using comma delim
            while (getline(ss, field, ',')) {
                fields.push_back(field);
            }

            // create record object from the fields and store in vector
            records.push_back(Record(fields)); // TEMP use insertrecords once implemented
        }

        file.close();

        // TEMP print the records to see if working
        for (Record& record : records) {
            record.print();
        }

    }

    // Given an ID, find the relevant record and print it
    Record findRecordById(int id) {
        
    }
};
