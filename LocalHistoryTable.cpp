#include <map>
#include <string>

#include "LocalHistoryTable.h"

LocalHistoryTable::LocalHistoryTable()
{
    m_adress_length = 8;
}

LocalHistoryTable::LocalHistoryTable(int adress_length)
{
    m_adress_length = adress_length;
}

int LocalHistoryTable::searchEntry(std::string adress)
{
    //legt neuen String mit der im Konstruktor festgelegten Länge an
    if(adress.length() == 8)
    {
        std::string modivied_adress = adress.substr(8 - m_adress_length);

        m_current_register = m_local_history_table.find(modivied_adress);//Iterator auf Adresse setzen

        //Falls Adresse noch nicht in m_local_history_table
        if(m_current_register == m_local_history_table.end())
        {
            m_local_history_table.insert(std::pair<std::string, int>(modivied_adress, 1));//Neuen Eintrag anlegen
            m_current_register = m_local_history_table.find(modivied_adress);//Iterator auf neuen Eintrag setzen
        }
    }

    return m_current_register->second;
}

bool LocalHistoryTable::updateState(int state)
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
