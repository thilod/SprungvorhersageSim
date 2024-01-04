#include <map>
#include <sstream>
#include <cmath>

#include "GlobalHistoryTable.h"

GlobalHistoryTable::GlobalHistoryTable()
{
    m_global_history_register_length = 4;
    m_global_history_register = 0;
    m_global_history_table.insert(std::pair<int, int>(m_global_history_register, 1));
    m_current_register = m_global_history_table.find(m_global_history_register);
}

GlobalHistoryTable::GlobalHistoryTable(int global_history_register_length)
{
    m_global_history_register_length = global_history_register_length;
    m_global_history_register = 0;
    std::map<int, int> global_history_table;

}

//Aktualisiert Global History Register: fügt letzten State hinten an, entfernt ersten bit
void GlobalHistoryTable::updateGlobalHistoryRegister(int state)
{
    m_global_history_register = (m_global_history_register << 1) + state;
    int overflow_bit = std::pow(2, m_global_history_register_length);
    if(m_global_history_register > overflow_bit - 1)
    {
        m_global_history_register -= overflow_bit;
    }
}

void GlobalHistoryTable::searchEntry()
{
    m_current_register = m_global_history_table.find(m_global_history_register);//Iterator auf Eintrag des global_history_registers in Tabelle setzen

    if(m_current_register == m_global_history_table.end())//Falls nicht vorhanden:
    {
        m_global_history_table.insert(std::pair<int, int>(m_global_history_register, 1));//Neuen Eintrag anlegen
        m_current_register = m_global_history_table.find(m_global_history_register);//Und Iterator auf neuen Eintrag setzen
    }
}

bool GlobalHistoryTable::updateState(int state)
{
    switch(m_current_register->second)
    {
        case 0:
            switch(state)
            {
                case 0:
                    return true;
                case 1:
                    m_current_register->second++;
                    return false;
            }
            break;
        case 1:
            switch(state)
            {
                case 0:
                    m_current_register->second--;
                    return true;
                case 1:
                    m_current_register->second++;
                    return false;
            }
            break;
        case 2:
            switch(state)
            {
                case 0:
                    m_current_register->second--;
                    return false;
                case 1:
                    m_current_register->second++;
                    return true;
            }
            break;
        case 3:
            switch(state)
            {
                case 0:
                    m_current_register->second--;
                    return false;
                case 1:
                    return true;
            }
            break;
    }
    return false;
}

//Für gshare
void GlobalHistoryTable::linkLastByte(char last_byte)
{
    //Wandelt letztes Byte der Adresse in eine Zahl um
    int int_last_byte;
    std::stringstream string_stream;
    string_stream << std::hex << last_byte;
    string_stream >> int_last_byte;

    //XOR Verknüpfung
    m_global_history_register ^= int_last_byte;
}