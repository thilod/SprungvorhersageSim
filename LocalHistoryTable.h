#pragma once

#include <map>
#include <string>

class LocalHistoryTable
{
public:
    LocalHistoryTable();
    LocalHistoryTable(int adress_length);

    int searchEntry(std::string adress);
    bool updateState(int state);

private:
    int m_adress_length;
    std::map<std::string, int> m_local_history_table;
    std::map<std::string, int>::iterator m_current_register;
};