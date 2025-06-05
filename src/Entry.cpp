#include "Entry.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits> // For numeric_limits
#include <iomanip> // For std::setw
#include <algorithm> // For std::find_if, std::remove_if

// Global EntryManager instance definition
std::unique_ptr<EntryManager> currentEntryManager = nullptr;

// Helper to get input safely
template<typename T>
T getValidatedInput(const std::string& prompt) {
    T value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) {
            std::cout << "Invalid input. Please try again.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
    }
}

EntryManager::EntryManager(const std::string& formName) : formName(formName), nextKey(1) {
    entriesFilePath = "Forms/" + formName + "_entries.dat"; // Using .dat for generic data
    loadEntriesFromFile();
}

void EntryManager::saveEntriesToFile() {
    std::ofstream outFile(entriesFilePath);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not save entries to file " << entriesFilePath << std::endl;
        return;
    }

    for (const auto& entry : entries) {
        outFile << "KEY:" << entry.key << "\n";
        for (const auto& pair : entry.data) {
            outFile << pair.first << ":";
            // Need to handle different std::any types for saving
            if (pair.second.type() == typeid(std::string)) {
                outFile << "string:" << std::any_cast<std::string>(pair.second) << "\n";
            } else if (pair.second.type() == typeid(int)) {
                outFile << "int:" << std::any_cast<int>(pair.second) << "\n";
            } else if (pair.second.type() == typeid(float)) {
                outFile << "float:" << std::any_cast<float>(pair.second) << "\n";
            } else if (pair.second.type() == typeid(double)) {
                outFile << "double:" << std::any_cast<double>(pair.second) << "\n";
            }
            // Add more types as needed
        }
        outFile << "---\n"; // Separator for entries
    }
    outFile.close();
}

void EntryManager::loadEntriesFromFile() {
    std::ifstream inFile(entriesFilePath);
    if (!inFile.is_open()) {
        // File might not exist yet, which is fine for a new form
        return;
    }

    entries.clear();
    std::string line;
    std::shared_ptr<Entry> currentEntry = nullptr;
    nextKey = 1; // Reset nextKey for loading

    while (std::getline(inFile, line)) {
        if (line.rfind("KEY:", 0) == 0) { // Starts with "KEY:"
            int key = std::stoi(line.substr(4));
            currentEntry = std::make_shared<Entry>(key);
            entries.push_back(*currentEntry); // Add to vector
            nextKey = std::max(nextKey, key + 1); // Update nextKey
        } else if (line == "---") {
            currentEntry = nullptr; // End of current entry
        } else if (currentEntry) {
            std::stringstream ss(line);
            std::string fieldName, fieldType, fieldValueStr;
            std::getline(ss, fieldName, ':');
            std::getline(ss, fieldType, ':');
            std::getline(ss, fieldValueStr);

            if (fieldType == "string") {
                entries.back().data[fieldName] = fieldValueStr;
            } else if (fieldType == "int") {
                entries.back().data[fieldName] = std::stoi(fieldValueStr);
            } else if (fieldType == "float") {
                entries.back().data[fieldName] = std::stof(fieldValueStr);
            } else if (fieldType == "double") {
                entries.back().data[fieldName] = std::stod(fieldValueStr);
            }
        }
    }
    inFile.close();
}

void EntryManager::addEntry(const std::shared_ptr<FormDefinition>& formDef) {
    if (!formDef) {
        std::cout << "No form selected. Please select a form first.\n";
        return;
    }

    Entry newEntry(nextKey++);
    std::cout << "\n--- Add New Entry for Form: " << formDef->name << " ---\n";

    for (const auto& field : formDef->fields) {
        if (field->type == "string") {
            std::string value;
            std::cout << "Enter value for " << field->name << " (string): ";
            std::getline(std::cin, value);
            newEntry.data[field->name] = value;
        } else if (field->type == "number") {
            auto numField = std::static_pointer_cast<NumberField>(field);
            if (numField->numberType == "int") {
                newEntry.data[field->name] = getValidatedInput<int>("Enter value for " + field->name + " (int): ");
            } else if (numField->numberType == "float") {
                newEntry.data[field->name] = getValidatedInput<float>("Enter value for " + field->name + " (float): ");
            } else if (numField->numberType == "double") {
                newEntry.data[field->name] = getValidatedInput<double>("Enter value for " + field->name + " (double): ");
            }
        } else if (field->type == "select") {
            auto selectField = std::static_pointer_cast<SelectField>(field);
            std::cout << "Select an option for " << field->name << ":\n";
            for (const auto& option : selectField->options) {
                std::cout << option.first << ". " << option.second << "\n";
            }
            int selectedOption = getValidatedInput<int>("Enter your choice: ");
            if (selectField->options.count(selectedOption)) {
                newEntry.data[field->name] = selectField->options[selectedOption];
            } else {
                std::cout << "Invalid option selected. Storing empty value.\n";
                newEntry.data[field->name] = std::string("");
            }
        }
    }
    entries.push_back(newEntry);
    saveEntriesToFile();
    std::cout << "Entry with key " << newEntry.key << " added successfully.\n";
}

void EntryManager::editEntry(int key, const std::shared_ptr<FormDefinition>& formDef) {
    if (!formDef) {
        std::cout << "No form selected. Please select a form first.\n";
        return;
    }

    auto it = std::find_if(entries.begin(), entries.end(), [key](const Entry& e) {
        return e.key == key;
    });

    if (it == entries.end()) {
        std::cout << "Entry with key " << key << " not found.\n";
        return;
    }

    Entry& entryToEdit = *it;
    std::cout << "\n--- Editing Entry with Key: " << key << " for Form: " << formDef->name << " ---\n";

    for (const auto& field : formDef->fields) {
        std::cout << "Current value for " << field->name << ": ";
        if (entryToEdit.data.count(field->name)) {
            if (field->type == "string") {
                std::cout << std::any_cast<std::string>(entryToEdit.data[field->name]) << "\n";
            } else if (field->type == "number") {
                auto numField = std::static_pointer_cast<NumberField>(field);
                if (numField->numberType == "int") {
                    std::cout << std::any_cast<int>(entryToEdit.data[field->name]) << "\n";
                } else if (numField->numberType == "float") {
                    std::cout << std::any_cast<float>(entryToEdit.data[field->name]) << "\n";
                } else if (numField->numberType == "double") {
                    std::cout << std::any_cast<double>(entryToEdit.data[field->name]) << "\n";
                }
            } else if (field->type == "select") {
                std::cout << std::any_cast<std::string>(entryToEdit.data[field->name]) << "\n";
            }
        } else {
            std::cout << "[Not set]\n";
        }

        std::string response;
        std::cout << "Do you want to edit this field? (y/n): ";
        std::getline(std::cin, response);
        if (response == "y" || response == "Y") {
            if (field->type == "string") {
                std::string value;
                std::cout << "Enter new value for " << field->name << " (string): ";
                std::getline(std::cin, value);
                entryToEdit.data[field->name] = value;
            } else if (field->type == "number") {
                auto numField = std::static_pointer_cast<NumberField>(field);
                if (numField->numberType == "int") {
                    entryToEdit.data[field->name] = getValidatedInput<int>("Enter new value for " + field->name + " (int): ");
                } else if (numField->numberType == "float") {
                    entryToEdit.data[field->name] = getValidatedInput<float>("Enter new value for " + field->name + " (float): ");
                } else if (numField->numberType == "double") {
                    entryToEdit.data[field->name] = getValidatedInput<double>("Enter new value for " + field->name + " (double): ");
                }
            } else if (field->type == "select") {
                auto selectField = std::static_pointer_cast<SelectField>(field);
                std::cout << "Select a new option for " << field->name << ":\n";
                for (const auto& option : selectField->options) {
                    std::cout << option.first << ". " << option.second << "\n";
                }
                int selectedOption = getValidatedInput<int>("Enter your choice: ");
                if (selectField->options.count(selectedOption)) {
                    entryToEdit.data[field->name] = selectField->options[selectedOption];
                } else {
                    std::cout << "Invalid option selected. Value remains unchanged.\n";
                }
            }
        }
    }
    saveEntriesToFile();
    std::cout << "Entry with key " << key << " updated successfully.\n";
}

void EntryManager::viewEntries(int page, int entriesPerPage) {
    if (entries.empty()) {
        std::cout << "No entries to display.\n";
        return;
    }

    if (!currentSelectedForm) {
        std::cout << "No form selected. Cannot display entries without a form definition.\n";
        return;
    }

    int totalEntries = entries.size();
    int totalPages = (totalEntries + entriesPerPage - 1) / entriesPerPage;

    if (page < 1) page = 1;
    if (page > totalPages) page = totalPages;

    int startIdx = (page - 1) * entriesPerPage;
    int endIdx = std::min(startIdx + entriesPerPage, totalEntries);

    std::cout << "\n--- Viewing Entries for Form: " << currentSelectedForm->name << " (Page " << page << "/" << totalPages << ") ---\n";

    // Determine column widths
    std::map<std::string, int> columnWidths;
    columnWidths["KEY"] = 5; // For "KEY" column
    for (const auto& field : currentSelectedForm->fields) {
        columnWidths[field->name] = field->name.length(); // Initial width is field name length
    }

    // Adjust column widths based on data
    for (int i = startIdx; i < endIdx; ++i) {
        const auto& entry = entries[i];
        columnWidths["KEY"] = std::max(columnWidths["KEY"], (int)std::to_string(entry.key).length());
        for (const auto& field : currentSelectedForm->fields) {
            if (entry.data.count(field->name)) {
                if (field->type == "string") {
                    columnWidths[field->name] = std::max(columnWidths[field->name], (int)std::any_cast<std::string>(entry.data.at(field->name)).length());
                } else if (field->type == "number") {
                    auto numField = std::static_pointer_cast<NumberField>(field);
                    if (numField->numberType == "int") {
                        columnWidths[field->name] = std::max(columnWidths[field->name], (int)std::to_string(std::any_cast<int>(entry.data.at(field->name))).length());
                    } else if (numField->numberType == "float") {
                        columnWidths[field->name] = std::max(columnWidths[field->name], (int)std::to_string(std::any_cast<float>(entry.data.at(field->name))).length());
                    } else if (numField->numberType == "double") {
                        columnWidths[field->name] = std::max(columnWidths[field->name], (int)std::to_string(std::any_cast<double>(entry.data.at(field->name))).length());
                    }
                } else if (field->type == "select") {
                    columnWidths[field->name] = std::max(columnWidths[field->name], (int)std::any_cast<std::string>(entry.data.at(field->name)).length());
                }
            }
        }
    }

    // Print header
    std::cout << std::left << std::setw(columnWidths["KEY"] + 2) << "KEY";
    for (const auto& field : currentSelectedForm->fields) {
        std::cout << std::left << std::setw(columnWidths[field->name] + 2) << field->name;
    }
    std::cout << "\n";

    // Print separator
    std::cout << std::string(columnWidths["KEY"] + 2, '-');
    for (const auto& field : currentSelectedForm->fields) {
        std::cout << std::string(columnWidths[field->name] + 2, '-');
    }
    std::cout << "\n";

    // Print entries
    for (int i = startIdx; i < endIdx; ++i) {
        const auto& entry = entries[i];
        std::cout << std::left << std::setw(columnWidths["KEY"] + 2) << entry.key;
        for (const auto& field : currentSelectedForm->fields) {
            if (entry.data.count(field->name)) {
                if (field->type == "string") {
                    std::cout << std::left << std::setw(columnWidths[field->name] + 2) << std::any_cast<std::string>(entry.data.at(field->name));
                } else if (field->type == "int") {
                    std::cout << std::left << std::setw(columnWidths[field->name] + 2) << std::any_cast<int>(entry.data.at(field->name));
                } else if (field->type == "float") {
                    std::cout << std::left << std::setw(columnWidths[field->name] + 2) << std::any_cast<float>(entry.data.at(field->name));
                } else if (field->type == "double") {
                    std::cout << std::left << std::setw(columnWidths[field->name] + 2) << std::any_cast<double>(entry.data.at(field->name));
                } else if (field->type == "select") {
                    std::cout << std::left << std::setw(columnWidths[field->name] + 2) << std::any_cast<std::string>(entry.data.at(field->name));
                }
            } else {
                std::cout << std::left << std::setw(columnWidths[field->name] + 2) << "N/A";
            }
        }
        std::cout << "\n";
    }

    // Page navigation
    std::cout << "\nPage Navigation: ";
    if (page > 1) {
        std::cout << "< Previous ";
    }
    std::cout << " (Current Page: " << page << ") ";
    if (page < totalPages) {
        std::cout << "Next >";
    }
    std::cout << "\n";
}

void EntryManager::deleteEntry(int key) {
    auto it = std::remove_if(entries.begin(), entries.end(), [key](const Entry& e) {
        return e.key == key;
    });

    if (it != entries.end()) {
        entries.erase(it, entries.end());
        std::cout << "Entry with key " << key << " deleted successfully.\n";
        resetEntryNumbering();
        saveEntriesToFile();
    } else {
        std::cout << "Entry with key " << key << " not found.\n";
    }
}

void EntryManager::resetEntryNumbering() {
    nextKey = 1;
    for (auto& entry : entries) {
        entry.key = nextKey++;
    }
    std::cout << "Entry numbering reset.\n";
}
