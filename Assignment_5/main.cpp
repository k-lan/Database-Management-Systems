/* This is a skeleton code for Optimized Merge Sort, you can make modifications as long as you meet 
   all question requirements*/  

#include <bits/stdc++.h>
#include "record_class.h"

using namespace std;

//defines how many blocks are available in the Main Memory 
#define buffer_size 22

Records buffers[buffer_size]; //use this class object of size 22 as your main memory
vector<pair<string, int>> emp_temp_files; // temp file names, pointer to record
vector<pair<string, int>> dept_temp_files; // temp file names, pointer to record


/***You can change return type and arguments as you want.***/

//PASS 1
//Sorting the buffers in main_memory and storing the sorted records into a temporary file (Runs) 
// size_of_sort is the amount of elements we want to 
void sortBuffer(int size_of_sort, bool isEmp){
    if (isEmp) {
        sort(buffers, buffers + size_of_sort, compareEmpByEID);
    } else {
        sort(buffers, buffers + size_of_sort, compareDeptByEID);
    }
    return;
}

//Prints out the attributes from empRecord and deptRecord when a join condition is met 
//and puts it in file Join.csv
void printJoin(fstream& outfile, Records smallest) {
    outfile << smallest.emp_record.eid << "," << smallest.emp_record.ename
                << "," << smallest.emp_record.age << "," << smallest.emp_record.salary
                << smallest.dept_record.did << "," << smallest.dept_record.dname
                << "," << smallest.dept_record.budget << "," << smallest.dept_record.managerid << endl;
    return;
}

// Function to find the index of the Records object with the smallest eid in buffers
bool findSmallestEmpEIDIndex(int &smallest_idx) {
    // Find the first valid element to be the comparison base
    int index_of_smallest = -1; // Initialize with an invalid index
    bool validRecordFound = false;
    for (int i = 0; i < emp_temp_files.size(); i++) {
        if (buffers[i].no_values != -1) {
            index_of_smallest = i; // Found the first valid record
            validRecordFound = true;
            break; // Break after finding the first valid record
        }
    }

    if (!validRecordFound) {
        smallest_idx = -1; // Indicate no valid record found
        return false;
    }

    // Start comparison from the record next to the first valid record found
    for (int i = index_of_smallest + 1; i < emp_temp_files.size(); ++i) {
        if (buffers[i].no_values != -1 && buffers[i].emp_record.eid < buffers[index_of_smallest].emp_record.eid) {
            index_of_smallest = i; // Update the index if a smaller eid is found
        }
    }
    
    smallest_idx = index_of_smallest;
    return true; // Indicates that a valid record was found and smallest_idx is updated
}

// Function to find the index of the Records object with the smallest manager eid in buffers
bool findSmallestDeptEIDIndex(int &smallest_idx) {
    // Find the first valid element to be the comparison base
    int index_of_smallest = -1; // Initialize with an invalid index
    int dept_start = emp_temp_files.size();
    int dept_end = dept_temp_files.size() + dept_start;
    bool validRecordFound = false;
    for (int i = dept_start; i < dept_end; i++) {
        if (buffers[i].no_values != -1) {
            index_of_smallest = i; // Found the first valid record
            validRecordFound = true;
            break; // Break after finding the first valid record
        }
    }

    if (!validRecordFound) {
        smallest_idx = -1; // Indicate no valid record found
        return false;
    }

    // Start comparison from the record next to the first valid record found
    for (int i = index_of_smallest + 1; i < dept_end; ++i) {
        if (buffers[i].no_values != -1 && buffers[i].dept_record.managerid < buffers[index_of_smallest].dept_record.managerid) {
            index_of_smallest = i; // Update the index if a smaller eid is found
        }
    }
    
    smallest_idx = index_of_smallest;
    return true; // Indicates that a valid record was found and smallest_idx is updated
}

//Use main memory to Merge and Join tuples 
//which are already sorted in 'runs' of the relations Dept and Emp 
void mergeJoinRuns(fstream& outputFile){
    int k = 0; // index for buffer
    int emp_end; // last index for emp files in buffer
    int dept_end; // last index for dept files in buffer
    // Load in first smallest element from emp temp files
    for (int i = 0; i < emp_temp_files.size(); i++) {
        fstream file(emp_temp_files[i].first, ios::in);
        buffers[k] = Grab_Emp_Record(file);
        emp_temp_files[i].second = file.tellg();
        file.close();
        k++;
    }
    emp_end = k - 1; // last index for emp files in buffer

    // Load in first smallest element from dept temp files
    for (int i = 0; i < dept_temp_files.size(); i++) {
        fstream file(dept_temp_files[i].first, ios::in);
        buffers[k] = Grab_Dept_Record(file);
        dept_temp_files[i].second = file.tellg();
        file.close();
        k++;
    }
    dept_end = k - 1; // last index for dept files in buffer

    int smallest_emp_idx, smallest_dept_idx;
    // get first smallest values
    findSmallestEmpEIDIndex(smallest_emp_idx);
    findSmallestDeptEIDIndex(smallest_dept_idx);
    while (smallest_emp_idx != -1 && smallest_dept_idx != -1) {
        // get the dept file index (it is stored as an offset from the emp_temp_files.size()
        int dept_file_idx = smallest_dept_idx - emp_temp_files.size();

        // Do we join?
        if (buffers[smallest_emp_idx].emp_record.eid == buffers[smallest_dept_idx].dept_record.managerid) {
            // join the records
            buffers[smallest_emp_idx].joinRecords(buffers[smallest_dept_idx]);
            printJoin(outputFile, buffers[smallest_emp_idx]);
            // read in the next record from emp and dept
            // emp here
            fstream file(emp_temp_files[smallest_emp_idx].first, ios::in);
            file.seekg(emp_temp_files[smallest_emp_idx].second);
            buffers[smallest_emp_idx] = Grab_Emp_Record(file);
            emp_temp_files[smallest_emp_idx].second = file.tellg();
            file.close();
            // dept here
            // have to find the index of the dept file
            fstream dept_file(dept_temp_files[dept_file_idx].first, ios::in);
            dept_file.seekg(dept_temp_files[dept_file_idx].second);
            buffers[smallest_dept_idx] = Grab_Dept_Record(dept_file);
            dept_temp_files[dept_file_idx].second = dept_file.tellg();
            dept_file.close();
            // Grad next two records
            findSmallestEmpEIDIndex(smallest_emp_idx);
            findSmallestDeptEIDIndex(smallest_dept_idx);

        } else if (buffers[smallest_emp_idx].emp_record.eid < buffers[smallest_dept_idx].dept_record.managerid) {
            // read in the next record from emp
            fstream file(emp_temp_files[smallest_emp_idx].first, ios::in);
            file.seekg(emp_temp_files[smallest_emp_idx].second);
            buffers[smallest_emp_idx] = Grab_Emp_Record(file);
            emp_temp_files[smallest_emp_idx].second = file.tellg();
            file.close();
            findSmallestEmpEIDIndex(smallest_emp_idx);
        } else {
            // read in the next record from dept
            fstream file(dept_temp_files[dept_file_idx].first, ios::in);
            file.seekg(dept_temp_files[dept_file_idx].second);
            buffers[smallest_dept_idx] = Grab_Dept_Record(file);
            dept_temp_files[dept_file_idx].second = file.tellg();
            file.close();
            findSmallestDeptEIDIndex(smallest_dept_idx);
        }
    }
}

// Print lowest emp value to output csv
void printEmpSorted(fstream& outfile, Records smallest){
    outfile << smallest.emp_record.eid << "," << smallest.emp_record.ename
                << "," << smallest.emp_record.age << "," << smallest.emp_record.salary << endl;
    return;
}

void printDeptSorted(fstream& outfile, Records smallest){
    outfile << smallest.dept_record.did << "," << smallest.dept_record.dname
                << "," << smallest.dept_record.budget << "," << smallest.dept_record.managerid << endl;
    return;
}

// this function writes the buffer to a temporary file with number index
void writeBufferToTempFile(int size_of_buffer, int &file_index, bool isEmp) {
    string filename;
    if (isEmp) {
        filename = "emp_temp_file_" + to_string(file_index) + ".txt";
    } else {
        filename = "dept_temp_file_" + to_string(file_index) + ".txt";
    }
    // Open a new fstream for output
    fstream outfile(filename, ios::out | ios::in | ios::trunc);

    if (outfile.is_open()) {
        // file created, write to this file, make sure to write to the right file
        if (isEmp) {
            emp_temp_files.push_back(make_pair(filename, 0));
            for (int i = 0; i < size_of_buffer; ++i) {
                printEmpSorted(outfile, buffers[i]);
            }
        } else {
            dept_temp_files.push_back(make_pair(filename, 0));
            for (int i = 0; i < size_of_buffer; ++i) {
                printDeptSorted(outfile, buffers[i]);
            }
        }

        file_index++;
        outfile.close();

    } else {
        // Handle the case where the file could not be opened
        cout << "Failed to open file: " << filename << endl;
    }
    return;
}

// empty the buffer to let more records into main memory.
void clearBuffer() {
    for(int i = 0; i < buffer_size; ++i) {
        buffers[i].reset();
    }
}

// close and delete all temporary files at the end
void closeAndDeleteFiles() {
    // Detete all of the emp temp files
    for (size_t i = 0; i < emp_temp_files.size(); ++i) {
        // Delete the file using the filename
        string file_to_remove = "emp_temp_file_" + to_string(i) + ".txt";
        if (remove(file_to_remove.c_str()) != 0) {
            cerr << "Error deleting file: " << file_to_remove << endl;
        }
    }
    // Delete all of the dept temp files
    for (size_t i = 0; i < dept_temp_files.size(); ++i) {
        string file_to_remove = "dept_temp_file_" + to_string(i) + ".txt";
        if (remove(file_to_remove.c_str()) != 0) {
            cerr << "Error deleting file: " << file_to_remove << endl;
        }
    }
}


int main() {

    //Open file streams to read and write
    //Opening out two relations Emp.csv and Dept.csv which we want to join
    fstream empin;
    fstream deptin;
    empin.open("Emp.csv", ios::in);
    deptin.open("Dept.csv", ios::in);
   
    //Creating the Join.csv file where we will store our joined results
    fstream joinout;
    joinout.open("Join.csv", ios::out | ios::app);

    int emp_temp_file_idx = 0; // index of temp files, used for naming/deleting temps
    int dept_temp_file_idx = 0;
    int buffer_idx = 0; // index to keep track of buffers
    int records_read = 0; // keep track of records read, for debugging
    buffers[buffer_idx] = Grab_Emp_Record(empin);

    while (buffers[buffer_idx].no_values != -1) {
        //1. Create runs for Emp which are sorted using Sort_Buffer()

        // Sort if buffer is full
        if (buffer_idx == buffer_size - 1) {
            // sort the buffer, write the sorted values to temp files
            sortBuffer(buffer_size, true);
            writeBufferToTempFile(buffer_size, emp_temp_file_idx, true);
            clearBuffer();
            buffer_idx = 0;
        } else {
            // Go to the next 
            buffer_idx++;
        }

        // Get the next record
        buffers[buffer_idx] = Grab_Emp_Record(empin);
        records_read++;
    }

    // sort whatever is left over in the buffer, write it to temp file
    sortBuffer(buffer_idx, true);
    writeBufferToTempFile(buffer_idx, emp_temp_file_idx, true);

    buffer_idx = 0;
    clearBuffer();
    buffers[buffer_idx] = Grab_Dept_Record(deptin);
    while (buffers[buffer_idx].no_values != -1) {
        //1. Create runs for Dept which are sorted using Sort_Buffer()

        // Sort if buffer is full
        if (buffer_idx == buffer_size - 1) {
            // sort the buffer, write the sorted values to temp files
            sortBuffer(buffer_size, false);
            writeBufferToTempFile(buffer_size, dept_temp_file_idx, false);
            clearBuffer();
            buffer_idx = 0;
        } else {
            // Go to the next 
            buffer_idx++;
        }

        // Get the next record
        buffers[buffer_idx] = Grab_Dept_Record(deptin);
        records_read++;
    }

    // sort whatever is left over in the buffer
    sortBuffer(buffer_idx, false);
    writeBufferToTempFile(buffer_idx, dept_temp_file_idx, false);

    //2. Use Merge_Runs() to Sort the runs of Emp relations 
    mergeJoinRuns(joinout); // write the sorted files out
    // delete the temporary files
    closeAndDeleteFiles();
    return 0;
}