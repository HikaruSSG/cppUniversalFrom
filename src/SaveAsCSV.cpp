#include "SaveAsCSV.h"
#include "FormDefinition.h" // For FormDefinition struct
#include "Entry.h"          // For Entry struct
#include <fstream>
#include <iostream>
#include <algorithm> // For std::replace

void saveAsCSV(const std::string& filename, const std::shared_ptr<FormDefinition>& formDef, const std::vector<Entry>& entries) {
    if (!formDef) {
        std::cerr << "Error: No form definition provided for CSV export.\n";
        return;
    }

    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for CSV export.\n";
        return;
    }

    // Write CSV header
    outFile << "KEY";
    for (const auto& field : formDef->fields) {
        outFile << "," << field->name;
    }
    outFile << "\n";

    // Write entries
    for (const auto& entry : entries) {
        outFile << entry.key;
        for (const auto& field : formDef->fields) {
            outFile << ",";
            if (entry.data.count(field->name)) {
                // Handle different std::any types for writing to CSV
                if (field->type == "string") {
                    std::string value = std::any_cast<std::string>(entry.data.at(field->name));
                    // Escape commas and quotes for CSV
                    std::replace(value.begin(), value.end(), '"', '\''); // Replace double quotes with single quotes
                    outFile << "\"" << value << "\"";
                } else if (field->type == "int") {
                    outFile << std::any_cast<int>(entry.data.at(field->name));
                } else if (field->type == "float") {
                    outFile << std::any_cast<float>(entry.data.at(field->name));
                } else if (field->type == "double") {
                    outFile << std::any_cast<double>(entry.data.at(field->name));
                } else if (field->type == "select") {
                    std::string value = std::any_cast<std::string>(entry.data.at(field->name));
                    std::replace(value.begin(), value.end(), '"', '\'');
                    outFile << "\"" << value << "\"";
                }
            }
        }
        outFile << "\n";
    }

    outFile.close();
    std::cout << "Entries saved to " << filename << " as CSV successfully.\n";
}
