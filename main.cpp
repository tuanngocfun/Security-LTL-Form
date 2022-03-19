#include <iostream>
#include "LTL.hpp"
#include <string>
#include <sstream>
#include <cctype>
#include <utility>

using namespace std;
using namespace LTL;

bool isInt(const string& s)
{
    if(all_of(s.begin(), s.end(), [](unsigned char c){ return isspace(c) || ispunct(c); }))
    {
        return false;
    }
    else if(s.size() > 10)
    {
        return false;
    }
    else
    {
        int number;
        istringstream str(s);
        return (str >> number >> ws).eof();
    }
};

int main()
{
    string input;
    int choice;
    cout << "How many states? ";

    while(true)
    {
        getline(cin, input);
        if(isInt(input))
        {
            choice = stoi(input);
            if(choice > 0)
                break;
        }
        cout << "Invalid input. Please enter again: ";
    }

    Kripke_Structure_Graph graph(choice);
    for(int i = 0; i < choice; i++)
    {
        std::vector<char> label_list;
        cout << "Input label for s" << i << ": ";
        while(true)
        {
            std::vector<char> temp_label;
            getline(cin, input);
            istringstream in_str(input);
            bool validity = false;
            while(!in_str.eof() && in_str.good())
            {
                getline(in_str, input, ',');
                if(input.size() == 1 && islower(input[0]))
                {
                    temp_label.push_back(input[0]);
                    validity = true;
                }
                else
                {
                    validity = false;
                    break;
                }
            }

            if(validity == true)
            {
                label_list.swap(temp_label);
                break;
            }
            cout << "Invalid input. Please enter again: ";
        }

        graph.give_proposition(i, label_list[0]);
        for(size_t idx = 1; idx < label_list.size(); idx++)
            graph.add_proposition(i, label_list[idx]);

    }

    vector<pair<int, int>> edge_list;
    while(true)
    {
        cout << "Set transition (press e to stop): ";
        bool validity = false;
        bool escape = false;
        while(validity == false)
        {
            string temp_str;
            getline(cin, temp_str);
            if(temp_str == "e")
            {
                escape = true;
                break;
            }
            if(temp_str.empty() || !isdigit(temp_str[temp_str.size() - 1]))
            {
                cout << "Invalid input. Please enter again: ";
                continue;
            }


            istringstream in_str(temp_str);
            vector<int> temp_vec;
            temp_vec.reserve(2);
            while(!in_str.eof() && in_str.good())
            {
                getline(in_str, temp_str, '-');
                if(isInt(temp_str))
                {
                    choice = stoi(temp_str);
                    if(choice >= 0 && choice < (int)graph.size())
                    {
                        temp_vec.push_back(choice);
                    }
                    else
                    {
                        validity = false;
                        break;
                    }
                }
                else
                {
                    validity = false;
                    break;
                }
            }

            if(temp_vec.size() == 2)
            {
                edge_list.push_back(pair{temp_vec[0], temp_vec[1]});
                validity = true;
            }

            if(validity == false)
            {
                cout << "Invalid input. Please enter again: ";
            }
        }

        if(escape == true)
            break;
    }

    for(auto&& vertices_pair : edge_list)
    {
        graph.set_transition(vertices_pair.first, vertices_pair.second);
    }

    LTL_Model handle(graph);
    cout << "Enter a path: ";
    while(true)
    {
        getline(cin, input);
        if(handle.set_path(input) == true)
            break;
        cout << "Invalid path. Please enter again: ";
    }

    cout << "Enter an LTL formula: ";
    while(true)
    {
        getline(cin, input);
        if(handle.set_formula(input) == true)
            break;
        cout << "Invalid formula. Please enter again: ";
    }

    cout << "Answer: " << ((handle.run() == true) ? "Yes" : "No") << endl;

    return 0;
}
