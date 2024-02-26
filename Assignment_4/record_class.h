/* This is a skeleton code for External Memory Sorting, you can make modifications as long as you meet 
   all question requirements*/  
/* This record_class.h contains the class Records, which can be used to store tuples form Emp.csv (stored
in the EmpRecord).
*/
#include <bits/stdc++.h>

using namespace std;

class Records{
    public:
    
    struct EmpRecord {
        int eid;
        string ename;
        int age;
        int salary; // modified to int to avoid scientific notation in output
    }emp_record;

    int no_values = 0; //You can use this to check if you've don't have any more tuples

    // Function to print eid and ename of the emp_record
    void printEmpRecord() const {
        cout << "EID: " << emp_record.eid << "\t Name: " << emp_record.ename << "\t no_values: " << no_values << endl;
    }

    // clear the records
    void reset() {
        emp_record.eid = 0;
        emp_record.ename = "";
        emp_record.age = 0;
        emp_record.salary = 0;
        no_values = 0;
    }
};

// Compare two records to see which is larger. This is for sorting
bool compareByEID(const Records& record1, const Records& record2) {
    return record1.emp_record.eid < record2.emp_record.eid;
}

// Grab a single block from the Emp.csv file and put it inside the EmpRecord structure of the Records Class
Records Grab_Emp_Record(fstream &empin) {
    string line, word;
    Records emp;
    // grab entire line
    if (getline(empin, line, '\n')) {
        // turn line into a stream
        stringstream s(line);
        // gets everything in stream up to comma
        getline(s, word,',');
        emp.emp_record.eid = stoi(word);
        getline(s, word, ',');
        emp.emp_record.ename = word;
        getline(s, word, ',');
        emp.emp_record.age = stoi(word);
        getline(s, word, ',');
        emp.emp_record.salary = stoi(word);

        return emp;
    } else {
        emp.no_values = -1;
        return emp;
    }
}
