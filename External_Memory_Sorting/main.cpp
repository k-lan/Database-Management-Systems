/* External memory sorting */
// Kaelan Trowbridge 934-369-714
// trowbrka@oregonstate.edu

#include <bits/stdc++.h>
#include "record_class.h"
#include <algorithm> // For sort
#include <string>
#include <vector> // hold file pointers


using namespace std;
//defines how many blocks are available in the Main Memory 
#define buffer_size 22

Records buffers[buffer_size]; //use this class object of size 22 as your main memory
vector<pair<string, int>> temp_files; // temp files
// vector<int> temp_files_idx; // hold the index at which a filepointer points

//PASS 1
//Sorting the buffers in main_memory and storing the sorted records into a temporary file (Runs) 
// size_of_sort is the amount of elements we want to 
void sortBuffer(int size_of_sort){
    sort(buffers, buffers + size_of_sort, compareByEID);
    return;
}

// Function to find the index of the Records object with the smallest eid in buffers
bool findSmallestEIDIndex(int &smallest_idx) {
    // Find the first valid element to be the comparison base
    int index_of_smallest = -1; // Initialize with an invalid index
    bool validRecordFound = false;
    for (int i = 0; i < temp_files.size(); i++) {
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
    for (int i = index_of_smallest + 1; i < temp_files.size(); ++i) {
        if (buffers[i].no_values != -1 && buffers[i].emp_record.eid < buffers[index_of_smallest].emp_record.eid) {
            index_of_smallest = i; // Update the index if a smaller eid is found
        }
    }
    
    smallest_idx = index_of_smallest;
    return true; // Indicates that a valid record was found and smallest_idx is updated
}

// Print lowest value to output csv
void printSorted(fstream& outfile, Records smallest){
    outfile << smallest.emp_record.eid << "," << smallest.emp_record.ename
                << "," << smallest.emp_record.age << "," << smallest.emp_record.salary << endl;
    return;
}

//PASS 2
//Use main memory to Merge the Runs 
//which are already sorted in 'runs' of the relation Emp.csv 
void mergeRuns(fstream& outputFile){
    // buffer should be clear at this point, load one record in from each file to start
    for (int i = 0; i < temp_files.size(); i++) {
        fstream file(temp_files[i].first, ios::in);
        buffers[i] = Grab_Emp_Record(file);
        temp_files[i].second = file.tellg();
        file.close();
        // buffers[i].printEmpRecord();
    }
    // index to hold smallest value
    int smallest_idx;
    while (findSmallestEIDIndex(smallest_idx)) {
        // write the smallest file to memory
        printSorted(outputFile, buffers[smallest_idx]);
        // read in the next record
        fstream file(temp_files[smallest_idx].first, ios::in);
        file.seekg(temp_files[smallest_idx].second);
        buffers[smallest_idx] = Grab_Emp_Record(file);
        temp_files[smallest_idx].second = file.tellg();
        file.close();
    }
    return;
}

// this function writes the buffer to a temporary file with number index
void writeBufferToTempFile(int size_of_buffer, int &file_index) {
    string filename = "temp_file_" + to_string(file_index) + ".txt";
    // Open a new fstream for output
    fstream outfile(filename, ios::out | ios::in | ios::trunc);

    if (outfile.is_open()) {
        // file created, write to this file
        temp_files.push_back(make_pair(filename, 0));
        for (int i = 0; i < size_of_buffer; ++i) {
            printSorted(outfile, buffers[i]);
        }
        file_index++;
        outfile.close();

    } else {
        // Handle the case where the file could not be opened
        cout << "Failed to open file: " << filename << endl;
    }
    return;
}

// close and delete all temporary files at the end
void closeAndDeleteFiles() {
    for (size_t i = 0; i < temp_files.size(); ++i) {
        // Delete the file using the filename
        string file_to_remove = "temp_file_" + to_string(i) + ".txt";
        if (remove(file_to_remove.c_str()) != 0) {
            cerr << "Error deleting file: " << file_to_remove << endl;
        }
    }
}

// empty the buffer to let more records into main memory.
void clearBuffer() {
    for(int i = 0; i < buffer_size; ++i) {
        buffers[i].reset();
    }
}

int main(int argc, char* const argv[]) {

    //Open file streams to read and write
    //Opening out the Emp.csv relation that we want to Sort
    fstream empin;
    empin.open("Emp.csv", ios::in);
   
    //Creating the EmpSorted.csv file where we will store our sorted results
    fstream SortOut;
    SortOut.open("EmpSorted.csv", ios::out | ios::app);

    int temp_file_idx = 0; // index of temp files, used for naming/deleting temps
    int buffer_idx = 0; // index to keep track of buffers
    int records_read = 0; // keep track of records read, for debugging
    buffers[buffer_idx] = Grab_Emp_Record(empin);

    while (buffers[buffer_idx].no_values != -1) {
        //1. Create runs for Emp which are sorted using Sort_Buffer()

        // Sort if buffer is full
        if (buffer_idx == buffer_size - 1) {
            // sort the buffer, write the sorted values to temp files
            sortBuffer(buffer_size);
            writeBufferToTempFile(buffer_size, temp_file_idx);
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

    // sort whatever is left over in the buffer
    sortBuffer(buffer_idx);
    writeBufferToTempFile(buffer_idx, temp_file_idx);

    //2. Use Merge_Runs() to Sort the runs of Emp relations 
    mergeRuns(SortOut); // write the sorted files out
    // delete the temporary files
    closeAndDeleteFiles();
    return 0;
}
