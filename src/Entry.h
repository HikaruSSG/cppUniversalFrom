#ifndef ENTRY_H
#define ENTRY_H

#include <string>
#include <vector>
#include <map>
#include <any> // For storing different types of data
#include <memory> // For std::shared_ptr
#include "FormDefinition.h" // To know the form structure

// Forward declaration of EntryManager
class EntryManager;

// Declare the global EntryManager instance
extern std::unique_ptr<EntryManager> currentEntryManager;

// A single entry, which is a collection of key-value pairs based on a form definition
struct Entry {
    int key; // Automatically assigned key number
    std::map<std::string, std::any> data; // Stores the actual data for each field

    // Constructor to initialize with a key
    Entry(int k) : key(k) {}
};

// Class to manage entries for a specific form
class EntryManager {
private:
    std::string formName;
    std::string entriesFilePath;
    std::vector<Entry> entries;
    int nextKey;

    void saveEntriesToFile();
    void loadEntriesFromFile();

public:
    EntryManager(const std::string& formName);

    void addEntry(const std::shared_ptr<FormDefinition>& formDef);
    void editEntry(int key, const std::shared_ptr<FormDefinition>& formDef);
    void viewEntries(int page = 1, int entriesPerPage = 5);
    void deleteEntry(int key);
    void resetEntryNumbering(); // Resets keys after deletion

    const std::vector<Entry>& getEntries() const { return entries; }
    std::string getFormName() const { return formName; } // Getter for formName
};

#endif // ENTRY_H
