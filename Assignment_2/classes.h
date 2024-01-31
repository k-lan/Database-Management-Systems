#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <bitset>
#include <cstring>

// Can I use?
#include <fstream>

using namespace std;

const int PAGE_SIZE = 4096;

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

    // Convert record to string representation for writing to file
    std::string toString() const {
        return std::to_string(id) + "|" + name + "|" + bio + "|" + std::to_string(manager_id) + "|";
    }
};

class Page {
private:
    vector<char> data; // The actual data of the page
    vector<pair<int, int>> slotDirectory; // (Starting position, length) of each record
    int freeSpace;

public:
    Page() : data(PAGE_SIZE, '\0'), freeSpace(PAGE_SIZE - sizeof(int)) {} // freespace is initially page_size minus slot directory N

    int getRecordSize() {
        return slotDirectory.size();
    }
    // Write record to the page and update slot directory
    bool writeRecord(const Record& record) {
        std::string recordString = record.toString();
        int recordLength = recordString.length();

        if (recordLength + sizeof(int) * 2 > freeSpace) // Check if enough space is available
            return false;

        int start = PAGE_SIZE - freeSpace;
        for (char c : recordString)
            data[start++] = c;

        slotDirectory.emplace_back(start - recordLength, recordLength);
        freeSpace -= (recordLength + sizeof(int) * 2); // record and slotdirectory info
        return true;
    }

    void writeSlotDirectory() {
        int numSlots = slotDirectory.size();
        int directoryStart = PAGE_SIZE - sizeof(int); // Start at the end of the page

        // Write the number of slots at the end of the page
        memcpy(&data[directoryStart], &numSlots, sizeof(int));

        // Write the slot directory entries
        for (auto& entry : slotDirectory) {
            int start = entry.first;
            int length = entry.second;
            directoryStart -= sizeof(int); // Move backwards
            memcpy(&data[directoryStart], &length, sizeof(int));
            directoryStart -= sizeof(int); // Move backwards
            memcpy(&data[directoryStart], &start, sizeof(int));
        }
    }
    
    // Write the page to a file
    void writeToFile(const std::string& fileName) {
        ofstream file(fileName, ios::binary | ios::app); // Open in append mode
        if (file.is_open()) {
            file.write(data.data(), PAGE_SIZE);
            file.close();
        } else {
            cerr << "Failed to open .data file in Pages writeToFile function." << endl;
            return;
        }
}
};


class StorageBufferManager {

private:
    // datafile that will hold pages
    ofstream dataFile;
    string fileName;

    // Page for writing
    Page page;

    // Insert new record 
    void insertRecord(Record record) {
        // attemp to write the record, if no space, start a new page
        if (!page.writeRecord(record)) {
            // Page was full, write the slot directory to the data, then write to file
            page.writeSlotDirectory();
            page.writeToFile(fileName);
            // start a new page
            Page newPage;
            page = newPage;
            // don't forget to write the original record that wasn't able to be written
            page.writeRecord(record);
        }
    }


public:
    StorageBufferManager(string NewFileName) {
        fileName = NewFileName; // save the name of the file for later opening
        // Create your EmployeeRelation file 
        // Open the file for writing in binary mode
        dataFile.open(NewFileName, ios::binary | ios::out);

        // error handling
        if (!dataFile.is_open()) {
            cerr << "Failed to open or create the .data file." << endl;
            return;
        }

        dataFile.close();
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
            insertRecord(Record(fields));
            // records.push_back(Record(fields)); // TEMP use insertrecords once implemented
        }

        // write remaining page data to the file if it is not empty
        if (page.getRecordSize() > 0) {
            page.writeToFile(fileName);
        }

        file.close();

        // TEMP print the records to see if working
        // for (Record& record : records) {
        //     record.print();
        // }

    }

    // Given an ID, find the relevant record and print it
    Record findRecordById(int id) {
        
    }
};

