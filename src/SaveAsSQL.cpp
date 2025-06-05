#include "SaveAsSQL.h"
#include "FormDefinition.h" // For FormDefinition struct
#include "Entry.h"          // For Entry struct
#include <fstream>
#include <iostream>
#include <sstream> // For stringstream
#include <algorithm> // For std::replace, std::remove_if

// Helper to escape string for SQL
std::string escapeSQLString(const std::string& s) {
    std::string escaped = s;
    // Replace single quotes with two single quotes
    size_t pos = escaped.find('\'');
    while (pos != std::string::npos) {
        escaped.replace(pos, 1, "''");
        pos = escaped.find('\'', pos + 2);
    }
    return escaped;
}

void saveAsSQL(const std::string& filename, const std::shared_ptr<FormDefinition>& formDef, const std::vector<Entry>& entries) {
    if (!formDef) {
        std::cerr << "Error: No form definition provided for SQL export.\n";
        return;
    }

    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for SQL export.\n";
        return;
    }

    // Sanitize form name for table name
    std::string tableName = formDef->name;
    std::replace(tableName.begin(), tableName.end(), ' ', '_'); // Replace spaces with underscores
    // Remove any other non-alphanumeric characters (except underscore)
    tableName.erase(std::remove_if(tableName.begin(), tableName.end(), [](char c) {
        return !std::isalnum(c) && c != '_';
    }), tableName.end());

    // Create Table statement
    outFile << "DROP TABLE IF EXISTS " << tableName << ";\n";
    outFile << "CREATE TABLE " << tableName << " (\n";
    outFile << "    id INTEGER PRIMARY KEY AUTOINCREMENT"; // Assuming SQLite-like AUTOINCREMENT
    for (const auto& field : formDef->fields) {
        outFile << ",\n    " << field->name << " ";
        if (field->type == "string" || field->type == "select") {
            outFile << "TEXT";
        } else if (field->type == "number") {
            auto numField = std::static_pointer_cast<NumberField>(field);
            if (numField->numberType == "int") {
                outFile << "INTEGER";
            } else if (numField->numberType == "float" || numField->numberType == "double") {
                outFile << "REAL";
            }
        }
    }
    outFile << "\n);\n\n";

    // Insert statements
    for (const auto& entry : entries) {
        outFile << "INSERT INTO " << tableName << " (";
        bool firstField = true;
        for (const auto& field : formDef->fields) {
            if (entry.data.count(field->name)) { // Only include fields that have data
                if (!firstField) outFile << ", ";
                outFile << field->name;
                firstField = false;
            }
        }
        outFile << ") VALUES (";
        firstField = true;
        for (const auto& field : formDef->fields) {
            if (entry.data.count(field->name)) { // Only include fields that have data
                if (!firstField) outFile << ", ";
                if (field->type == "string" || field->type == "select") {
                    outFile << "'" << escapeSQLString(std::any_cast<std::string>(entry.data.at(field->name))) << "'";
                } else if (field->type == "int") {
                    outFile << std::any_cast<int>(entry.data.at(field->name));
                } else if (field->type == "float") {
                    outFile << std::any_cast<float>(entry.data.at(field->name));
                } else if (field->type == "double") {
                    outFile << std::any_cast<double>(entry.data.at(field->name));
                } else {
                    outFile << "NULL"; // Handle unknown types or missing data
                }
                firstField = false;
            }
        }
        outFile << ");\n";
    }

    outFile.close();
    std::cout << "Entries saved to " << filename << " as SQL successfully.\n";
}
