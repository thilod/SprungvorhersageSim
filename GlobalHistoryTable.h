#pragma once

#include <map>

class GlobalHistoryTable
{
public:
    GlobalHistoryTable();
    GlobalHistoryTable(int global_history_register_length);
    
    void updateGlobalHistoryRegister(int state);
    
    void searchEntry();
    bool updateState(int state);

    //Für gshare
    void linkLastByte(char last_byte);

private:
    unsigned int m_global_history_register;
    std::map<int, int> m_global_history_table;
    std::map<int, int>::iterator m_current_register;
    int m_global_history_register_length;
};