#ifndef SAVE_AS_CSV_H
#define SAVE_AS_CSV_H

#include <string>
#include <vector>
#include <map>
#include <any>
#include <memory>

// Forward declarations to avoid circular dependencies
struct FormDefinition;
struct Entry;

void saveAsCSV(const std::string& filename, const std::shared_ptr<FormDefinition>& formDef, const std::vector<Entry>& entries);

#endif // SAVE_AS_CSV_H
