#include "SaveAsJSON.h"
#include "FormDefinition.h" // For FormDefinition struct
#include "Entry.h"          // For Entry struct
#include <fstream>
#include <iostream>
#include <sstream> // For stringstream
#include <iomanip> // For std::setw, std::setfill

// Helper to escape string for JSON
std::string escapeJsonString(const std::string& s) {
    std::ostringstream oss;
    for (char c : s) {
        switch (c) {
            case '"': oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\b': oss << "\\b"; break;
            case '\f': oss << "\\f"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:
                if (iscntrl(c)) {
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                } else {
                    oss << c;
                }
                break;
        }
    }
    return oss.str();
}

void saveAsJSON(const std::string& filename, const std::shared_ptr<FormDefinition>& formDef, const std::vector<Entry>& entries) {
    if (!formDef) {
        std::cerr << "Error: No form definition provided for JSON export.\n";
        return;
    }

    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for JSON export.\n";
        return;
    }

    outFile << "{\n";
    outFile << "  \"formName\": \"" << escapeJsonString(formDef->name) << "\",\n";
    outFile << "  \"fields\": [\n";
    for (size_t i = 0; i < formDef->fields.size(); ++i) {
        const auto& field = formDef->fields[i];
        outFile << "    {\n";
        outFile << "      \"name\": \"" << escapeJsonString(field->name) << "\",\n";
        outFile << "      \"type\": \"" << escapeJsonString(field->type) << "\"\n";
        if (field->type == "number") {
            auto numField = std::static_pointer_cast<NumberField>(field);
            outFile << "      ,\"numberType\": \"" << escapeJsonString(numField->numberType) << "\"\n";
        } else if (field->type == "select") {
            auto selectField = std::static_pointer_cast<SelectField>(field);
            outFile << "      ,\"options\": {\n";
            size_t j = 0;
            for (const auto& option : selectField->options) {
                outFile << "        \"" << option.first << "\": \"" << escapeJsonString(option.second) << "\"";
                if (j < selectField->options.size() - 1) {
                    outFile << ",\n";
                } else {
                    outFile << "\n";
                }
                j++;
            }
            outFile << "      }\n";
        }
        outFile << "    }";
        if (i < formDef->fields.size() - 1) {
            outFile << ",\n";
        } else {
            outFile << "\n";
        }
    }
    outFile << "  ],\n";

    outFile << "  \"entries\": [\n";
    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& entry = entries[i];
        outFile << "    {\n";
        outFile << "      \"key\": " << entry.key << ",\n";
        outFile << "      \"data\": {\n";
        size_t k = 0;
        for (const auto& pair : entry.data) {
            outFile << "        \"" << escapeJsonString(pair.first) << "\": ";
            if (pair.second.type() == typeid(std::string)) {
                outFile << "\"" << escapeJsonString(std::any_cast<std::string>(pair.second)) << "\"";
            } else if (pair.second.type() == typeid(int)) {
                outFile << std::any_cast<int>(pair.second);
            } else if (pair.second.type() == typeid(float)) {
                outFile << std::any_cast<float>(pair.second);
            } else if (pair.second.type() == typeid(double)) {
                outFile << std::any_cast<double>(pair.second);
            } else {
                outFile << "null"; // Handle unknown types
            }
            if (k < entry.data.size() - 1) {
                outFile << ",\n";
            } else {
                outFile << "\n";
            }
            k++;
        }
        outFile << "      }\n";
        outFile << "    }";
        if (i < entries.size() - 1) {
            outFile << ",\n";
        } else {
            outFile << "\n";
        }
    }
    outFile << "  ]\n";
    outFile << "}\n";

    outFile.close();
    std::cout << "Entries saved to " << filename << " as JSON successfully.\n";
}
