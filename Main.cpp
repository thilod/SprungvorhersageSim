#include <iostream>
#include <fstream>
#include <string>

#include "GlobalHistoryTable.h"
#include "LocalHistoryTable.h"

void printResults(int total_jumps, int correct_jumps)
{
    double genauigkeit = (double)correct_jumps * 100 / total_jumps;
    std::cout << std::endl
              << "Gesamtanzahl Sprünge: " << total_jumps;
    std::cout << std::endl
              << "Richtige Sprünge: " << correct_jumps;
    std::cout << std::endl
              << "Genauigkeit: " << genauigkeit << "%" << std::endl;
}

// Sprungvorhersagen

// 1. Lokaler 2-Bit-Prädiktor
void localPredictor(std::string trace_file, int adress_length) // int adress_length
{
    std::ifstream read_stream;
    read_stream.open(trace_file, std::ios::in);

    LocalHistoryTable *p_local_history_table = new LocalHistoryTable(adress_length);

    unsigned int total_jumps = 0;
    unsigned int correct_jumps = 0;

    while (!read_stream.eof())
    {
        // Einlesen der Adresse
        std::string adress;
        read_stream >> adress;

        // Sucht den Eintrag der Adresse in der LocalHistoryTable, setzt den Iterator in dem Objekt auf den Eintrag
        // Ist kein Eintrag vorhanden, wird ein neuer erstellt und der Iterator auf den neuen Eintrag gesetzt
        p_local_history_table->searchEntry(adress);

        // Einlesen des tatsächlichen Ergebnisses des Sprungs
        int state;
        read_stream >> state;

        // Prüft, ob die Vorhersage korrekt war und updated den State der Adresse
        bool is_correct = p_local_history_table->updateState(state);
        if (is_correct)
            correct_jumps++;

        total_jumps++;
    }

    printResults(total_jumps, correct_jumps);

    delete p_local_history_table;
}

// 2. Two-level global predictor
void twoLevelGlobalPredictor(std::string trace_file, int global_history_register_length)
{
    std::ifstream read_stream;
    read_stream.open(trace_file, std::ios::in);

    GlobalHistoryTable *p_global_history_table = new GlobalHistoryTable(global_history_register_length);

    unsigned int total_jumps = 0;
    unsigned int correct_jumps = 0;

    while (!read_stream.eof())
    {
        // Sucht den Eintrag des aktuellen Global History Registers in der LocalHistoryTable, setzt den Iterator in dem Objekt auf den Eintrag
        // Ist kein Eintrag vorhanden, wird ein neuer erstellt und der Iterator auf den neuen Eintrag gesetzt
        p_global_history_table->searchEntry();

        // Einlesen der Adresse um den State einlesen zu können, wird nicht verwendet
        std::string adresse;
        read_stream >> adresse;

        // Einlesen des tatsächliche Ergebnis des Sprungs
        int state;
        read_stream >> state;

        // Prüft, ob die Vorhersage korrekt war und updated den State des Global History Registers
        bool is_correct = p_global_history_table->updateState(state);
        if (is_correct)
        {
            correct_jumps++;
        }

        // aktualisiert das Global History Register
        p_global_history_table->updateGlobalHistoryRegister(state);

        total_jumps++;
    }

    printResults(total_jumps, correct_jumps);

    delete p_global_history_table;
}

// 3. gshare
void gshare(std::string trace_file, int global_history_register_length)
{
    std::ifstream read_stream;
    read_stream.open(trace_file, std::ios::in);

    GlobalHistoryTable *p_global_history_table = new GlobalHistoryTable(global_history_register_length);

    unsigned int correct_jumps = 0;
    unsigned int total_jumps = 0;

    while (!read_stream.eof())
    {
        // Einlesen der Adresse
        std::string adress;
        read_stream >> adress;

        // Letzter Byte der Adresse wird mit XOR mit dem Global History Register verknüpft
        p_global_history_table->linkLastByte(adress.back());

        // Sucht den Eintrag des aktuellen Global History Registers in der LocalHistoryTable, setzt den Iterator in dem Objekt auf den Eintrag
        // Ist kein Eintrag vorhanden, wird ein neuer erstellt und der Iterator auf den neuen Eintrag gesetzt
        p_global_history_table->searchEntry();

        // Einlesen des tatsächlichen Ergebnisses des Sprungs
        int state;
        read_stream >> state;

        // Prüft, ob die Vorhersage korrekt war und updated den State der Adresse
        bool is_correct = p_global_history_table->updateState(state);
        if (is_correct)
        {
            correct_jumps++;
        }

        // aktualisiert das Global History Register
        p_global_history_table->updateGlobalHistoryRegister(state);

        total_jumps++;
    }

    printResults(total_jumps, correct_jumps);

    delete p_global_history_table;
}

// 4. Tournament
void tournament(std::string trace_file, int meta_predictor_adress_length, int local_predictor_adress_length, int global_history_register_length)
{
    /*
    Der Tournament Prädiktor wählt über einen Meta Prädiktor zwischen einem lokalen und einem globalen
    Prädiktor
    */
    std::ifstream read_stream;
    read_stream.open(trace_file, std::ios::in);

    LocalHistoryTable *p_meta_predictor = new LocalHistoryTable(meta_predictor_adress_length);
    LocalHistoryTable *p_local_history_table = new LocalHistoryTable(local_predictor_adress_length);
    GlobalHistoryTable *p_global_history_table = new GlobalHistoryTable(global_history_register_length);

    unsigned int total_jumps = 0;
    unsigned int correct_jumps = 0;

    while (!read_stream.eof())
    {
        std::string adress;
        read_stream >> adress;

        // Sucht Eintrag der Adresse in dem Meta Predictor, returned State für die Adresse
        int which_predictor = p_meta_predictor->searchEntry(adress);

        // Beide Prädiktoren werden eingelesen um später die Ergebnisse zu vergleichen
        p_global_history_table->searchEntry();
        p_local_history_table->searchEntry(adress);

        // Einlesen des tatsächlichen Ergebnisses des Sprungs
        int state;
        read_stream >> state;

        bool global_prediction = false;
        bool local_prediction = false;

        // Prüft beide Vorhersagen darauf, ob sie korrekt waren, updated den State beider
        global_prediction = p_global_history_table->updateState(state);
        local_prediction = p_local_history_table->updateState(state);

        // Prüft, ob die Vorhersage des ausgewählten Prädiktors korrekt war
        // für State 0 und 1 im Meta Prädiktor wird der Globale Prädiktor ausgewählt,
        // ansonsten der lokale
        if (which_predictor < 2 && global_prediction)
        {
            // Globaler Prädiktor war ausgewählt und korrekt
            correct_jumps++;
        }
        else if (which_predictor > 1 && local_prediction)
        {
            // Local Prädiktor war ausgewählt und korrekt
            correct_jumps++;
        }

        // Update des States des Meta Prädiktors
        if (local_prediction && !global_prediction)
        {
            // Wenn der Lokale Prädiktor richtig, und der Globale falsch, State wenn möglich inkrementieren
            p_meta_predictor->updateState(1);
        }
        else if (global_prediction && !local_prediction)
        {
            // Wenn der Globale Prädiktor richtig, und der Lokale falsch, State wenn möglich dekrementieren
            p_meta_predictor->updateState(0);
        }
        // Ansonsten State nicht verändern

        p_global_history_table->updateGlobalHistoryRegister(state);
        total_jumps++;
    }

    printResults(total_jumps, correct_jumps);

    delete p_meta_predictor;
    delete p_local_history_table;
    delete p_global_history_table;
}

int main()
{
    std::cout << std::endl
              << "2-Bit local Predictor";
    localPredictor("Tracefiles/trace_perl.txt", 8); // Parameter: Tracefile und Länge der Adresse im Speicher in Byte
    // localPredictor("Tracefiles/trace_jpeg.txt", 4);

    std::cout << std::endl
              << "Two-level global Predictor";
    twoLevelGlobalPredictor("Tracefiles/trace_perl.txt", 4); // Parameter: Tracefile und Länge des Global History Registers in Bit
    twoLevelGlobalPredictor("Tracefiles/trace_perl.txt", 16);

    std::cout << std::endl
              << "gshare";
    gshare("Tracefiles/trace_perl.txt", 4); // Parameter: s. two LevelGlobalPredictor
    gshare("Tracefiles/trace_perl.txt", 16);

    std::cout << std::endl
              << "tournament";
    tournament("Tracefiles/trace_perl.txt", 8, 8, 4); // Parameter: Tracefile, Adressenlänge im Metaprädiktor, Adressenlänge im lokalen Prädiktor, Länge des GHR
    // tournament("Tracefiles/trace_gcc.txt", 4, 4, 16);
}

/*
Beobachtungen:
-globale Prädiktoren treffen mit längerem Register immer bessere Vorhersagen
-gshare bietet so gut wie keine Verbesserung gegenüber dem two level global predictor
-bei lokalen Prädiktoren können mehrere Bytes ohne große Auswirkung auf die Vorhersagegenauigkeiten gestrichen werden,
in meinen Tests waren die Genauigkeiten bei wenigeren Bytes oft sogar besser
-Der Tournament Predictor ist nicht immer der genaueste (vor allem von der Länge des Global History Registers abhängig),
hat aber das größte Potential
*/