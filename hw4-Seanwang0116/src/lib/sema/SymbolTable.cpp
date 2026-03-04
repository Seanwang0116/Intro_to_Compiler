#include "sema/SemanticAnalyzer.hpp"

void dumpDemarcation(const char chr) {
  for (size_t i = 0; i < 110; ++i) {
    printf("%c", chr);
  }
  puts("");
}

bool SymbolTable::lookupSymbol(const char* name) {
    for (auto& entry : entries) {
        if (strcmp(entry.name, name) == 0) {
            return true;
        }
    }
    return false;
}

void SymbolTable::addSymbol(const char* name, const char* kind, int level, PType* type, const char* attribute) {
    SymbolEntry new_entry{name, kind, level, type, attribute};

    entries.emplace_back(new_entry);
}

SymbolEntry* SymbolTable::lookup_localSymbol(const char* name) {
    for (auto& entry : entries) {
        if (strcmp(entry.name, name) == 0) {
            return &entry;
        }
    }
    return NULL;
}

void SymbolManager::pushScope(SymbolTable *new_scope) {
    tables.push(new_scope);
}

void SymbolManager::popScope() {
    tables.pop();
}

void SymbolManager::printCurrentScope() {
    if (!print_or_not) {
        return;
    }
    SymbolTable *current_table = tables.top();
    dumpDemarcation('=');
    printf("%-33s%-11s%-11s%-17s%-11s\n", "Name", "Kind", "Level", "Type",
                                        "Attribute");
    dumpDemarcation('-');
    for (auto& entry : current_table->entries) {
        std::string scope = (entry.level == 0) ? "(global)" : "(local)";

        std::string type_str = entry.type->getPTypeCString();

        printf("%-33s%-11s%d%-10s%-17s%-11s\n", 
            entry.name,
            entry.kind,
            entry.level, scope.c_str(),
            type_str.c_str(),
            entry.attribute
        );
    }
    dumpDemarcation('-');
}

void SymbolManager::insertSymbol(const char* name, const char* kind, int level, PType* type, const char* attribute) {
    SymbolTable *current_table = tables.top();
    current_table->addSymbol(name, kind, level, type, attribute);
}

bool SymbolManager::lookupSymbol(const char* name) {
    return tables.top()->lookupSymbol(name);
}

void SymbolManager::set_print_table(bool opt_sbt) {
    print_or_not = opt_sbt;
}

SymbolEntry* SymbolManager::lookup_globalSymbol(const char* name) {
    auto tables_copy = tables;
    while (!tables_copy.empty()) {
        SymbolTable* current_table = tables_copy.top();
        tables_copy.pop();

        SymbolEntry* result = current_table->lookup_localSymbol(name);
        if (result != NULL) {
            return result;
        }
    }
    return NULL;
}