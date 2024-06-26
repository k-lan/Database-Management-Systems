/* This is a skeleton code for Optimized Merge Sort, you can make modifications as long as you meet 
   all question requirements*/  
/* This record_class.h contains the class Records, which can be used to store tuples form Emp.csv (stored
in the EmpRecords) and Dept.csv (Stored in DeptRecords) files.
*/
#include <bits/stdc++.h>

using namespace std;

class Records{
    public:
    
    struct EmpRecord {
        int eid;
        string ename;
        int age;
        int salary;
    }emp_record;

    struct DeptRecord {
        int did;
        string dname;
        int budget;
        int managerid;
    }dept_record;

    /*** You can add more variables if you want below ***/

    int no_values = 0; //You can use this to check if you've don't have any more tuples
    int number_of_emp_records = 0; // Tracks number of emp_records you have on the buffer
    int number_of_dept_records = 0; //Track number of dept_records you have on the buffer

    // clear the records
    void reset() {
        emp_record.eid = 0;
        emp_record.ename = "";
        emp_record.age = 0;
        emp_record.salary = 0;
        dept_record.did = 0;
        dept_record.dname = "";
        dept_record.budget = 0;
        dept_record.managerid = 0;
        no_values = 0;
    }

    void printEmpRecord() {
        cout << emp_record.eid << " " << emp_record.ename << " " << emp_record.age << " " << emp_record.salary << endl;
    }

    void printDeptRecord() {
        cout << dept_record.did << " " << dept_record.dname << " " << dept_record.budget << " " << dept_record.managerid << endl;
    }

    // Take a dept record and join it with the emp record
    void joinRecords(Records &dept) {
        dept_record.did = dept.dept_record.did;
        dept_record.dname = dept.dept_record.dname;
        dept_record.budget = dept.dept_record.budget;
        dept_record.managerid = dept.dept_record.managerid;
    }
};

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

        //Ensuring that you cannot use both structure (EmpEecord, DeptRecord) at the same memory block / time 
        emp.dept_record.did = 0;
        emp.dept_record.dname = "";
        emp.dept_record.budget = 0;
        emp.dept_record.managerid = 0;

        return emp;
    } else {
        emp.no_values = -1;
        return emp;
    }
}

// Grab a single block from the Dept.csv file and put it inside the DeptRecord structure of the Records Class
Records Grab_Dept_Record(fstream &deptin) {
    string line, word;
    //DeptRecord dept;
    Records dept;
    if (getline(deptin, line, '\n')) {
        stringstream s(line);
        getline(s, word,',');
        dept.dept_record.did = stoi(word);
        getline(s, word, ',');
        dept.dept_record.dname = word;
        getline(s, word, ',');
        dept.dept_record.budget = stoi(word);
        getline(s, word, ',');
        dept.dept_record.managerid = stoi(word);

        //Ensuring that you cannot use both structure (EmpEecord, DeptRecord) at the same memory block / time 
        dept.emp_record.eid = 0;
        dept.emp_record.ename = "";
        dept.emp_record.age = 0;
        dept.emp_record.salary = 0;

        return dept;
    } else {
        dept.no_values = -1;
        return dept;
    }
}

// Compare two employee records to see which is larger. This is for sorting
bool compareEmpByEID(const Records& record1, const Records& record2) {
    return record1.emp_record.eid < record2.emp_record.eid;
}

// Compare two department records to see which is larger. This is for sorting
bool compareDeptByEID(const Records& record1, const Records& record2) {
    return record1.dept_record.managerid < record2.dept_record.managerid;
}