#ifndef LCL_HPP_INCLUDED
#define LCL_HPP_INCLUDED

#include <iostream>
#include <string>
#include <sstream>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <cctype>
#include <utility>
#include <cmath>
#include <regex>

namespace LTL
{
    class Kripke_Structure_Graph
    {
     protected:
        class State
        {
         protected:
            std::set<char> data;

         public:
            State(char prop) { data.emplace(prop); }

            State(std::initializer_list<char> prop_list)
            {
                add_prop(prop_list);
            }

            State(const State&) = default;
            State(State&&) noexcept = default;

            bool operator==(const State& other) const
            {
                return data == other.data;
            }

            void add_prop(char prop)
            {
                data.emplace(prop);
            }

            void add_prop(std::initializer_list<char> prop_list)
            {
                for(auto&& prop : prop_list)
                    add_prop(prop);
            }

            void remove_prop(char prop)
            {
                data.erase(prop);
            }

            void remove_prop(std::initializer_list<char> prop_list)
            {
                for(auto&& prop : prop_list)
                    remove_prop(prop);
            }

            void reset_data(std::initializer_list<char> prop_list)
            {
                data.clear();
                add_prop(prop_list);
            }

            bool contains_prop(char to_check) const
            {
                return (data.find(to_check) != data.end()) ? true : false;
            }
        };

        size_t Size;
        std::map<size_t, State> state_collection;
        std::vector<std::vector<bool>> edge_collection;

     public:
        explicit Kripke_Structure_Graph(size_t num_of_state) : Size(num_of_state), edge_collection(Size, std::vector<bool>(Size, false)) {}

        Kripke_Structure_Graph(const Kripke_Structure_Graph&) = default;
        Kripke_Structure_Graph(Kripke_Structure_Graph&&) noexcept = default;
        Kripke_Structure_Graph& operator=(const Kripke_Structure_Graph&) = default;
        Kripke_Structure_Graph& operator=(Kripke_Structure_Graph&&) noexcept = default;
        virtual ~Kripke_Structure_Graph() {}

        // Create new state with given arguments
        void give_proposition(size_t order, char prop)
        {
            if(order >= Size)
                return;

            state_collection.emplace(order, prop);
        }

        void give_proposition(size_t order, std::initializer_list<char> prop_list)
        {
            if(order >= Size)
                return;

            state_collection.emplace(order, prop_list);
        }

        // Add more proposition to an already existing state
        void add_proposition(size_t order, char prop)
        {
            if(order >= Size)
                return;

            auto iter = state_collection.find(order);
            if(iter == state_collection.end())
                return;

            iter->second.add_prop(prop);
        }

        void add_proposition(size_t order, std::initializer_list<char> prop_list)
        {
            if(order >= Size)
                return;

            auto iter = state_collection.find(order);
            if(iter == state_collection.end())
                return;

            iter->second.add_prop(prop_list);
        }

        // Set edges between states
        void set_transition(size_t first_order, size_t second_order)
        {
            if(first_order >= Size || second_order >= Size)
                return;

            edge_collection[first_order][second_order] = true;
        }

        // Utility methods
        bool check_connection(size_t first_order, size_t second_order) const
        {
            if(first_order >= Size || second_order >= Size)
                return false;

            return edge_collection[first_order][second_order];
        }

        bool check_proposition(size_t order, char proposition) const
        {
            if(order >= Size)
                return false;

            return state_collection.at(order).contains_prop(proposition);
        }

        size_t max_size() const
        {
            return Size;
        }

        size_t size() const
        {
            return std::distance(state_collection.begin(), state_collection.end());
        }

        bool empty() const
        {
            return (state_collection.begin() == state_collection.end()) ? true : false;
        }
    };

    class LTL_Model
    {
     private:
        static constexpr size_t npos = -1;

        class String_Repetition_Detector
        {
         private:
            std::vector<std::pair<int, int>> repetitions;

            std::vector<int> z_function(std::string const& s)
            {
                int n = s.size();
                std::vector<int> z(n);
                for (int i = 1, l = 0, r = 0; i < n; i++)
                {
                    if (i <= r)
                        z[i] = std::min(r-i+1, z[i-l]);

                    while (i + z[i] < n && s[z[i]] == s[i+z[i]])
                        z[i]++;

                    if (i + z[i] - 1 > r)
                    {
                        l = i;
                        r = i + z[i] - 1;
                    }
                }
                return z;
            }

            int get_z(std::vector<int> const& z, int i)
            {
                if (0 <= i && i < (int)z.size())
                    return z[i];
                else
                    return 0;
            }

            void convert_to_repetitions(int shift, bool left, int cntr, int l, int k1, int k2)
            {
                for (int l1 = std::max(1, l - k2); l1 <= std::min(l, k1); l1++)
                {
                    if (left && l1 == l)
                        break;

                    int pos = shift + (left ? cntr - l1 : cntr - l - l1 + 1);
                    repetitions.emplace_back(pos, pos + 2*l - 1);
                }
            }

            void find_repetitions_impl(std::string s, int shift = 0)
            {
                int n = s.size();
                if (n == 1)
                    return;

                int nu = n / 2;
                int nv = n - nu;
                std::string u = s.substr(0, nu);
                std::string v = s.substr(nu);
                std::string ru(u.rbegin(), u.rend());
                std::string rv(v.rbegin(), v.rend());

                find_repetitions_impl(u, shift);
                find_repetitions_impl(v, shift + nu);

                std::vector<int> z1 = z_function(ru);
                std::vector<int> z2 = z_function(v + '#' + u);
                std::vector<int> z3 = z_function(ru + '#' + rv);
                std::vector<int> z4 = z_function(v);

                for (int cntr = 0; cntr < n; cntr++)
                {
                    int l, k1, k2;
                    if (cntr < nu)
                    {
                        l = nu - cntr;
                        k1 = get_z(z1, nu - cntr);
                        k2 = get_z(z2, nv + 1 + cntr);
                    }
                    else
                    {
                        l = cntr - nu + 1;
                        k1 = get_z(z3, nu + 1 + nv - 1 - (cntr - nu));
                        k2 = get_z(z4, (cntr - nu) + 1);
                    }

                    if (k1 + k2 >= l)
                        convert_to_repetitions(shift, cntr < nu, cntr, l, k1, k2);
                }
            }

         public:
            std::vector<std::pair<int, int>> operator()(const std::string& input)
            {
                find_repetitions_impl(input);
                return repetitions;
            }

            std::pair<size_t, size_t> get_highest_indice_pair(const std::string& input)
            {
                find_repetitions_impl(input);
                if(repetitions.empty())
                    return {npos, npos};

                size_t idx = 0;
                for(size_t i = 1; i < repetitions.size(); i++)
                {
                    if(repetitions[idx].second < repetitions[i].second)
                    {
                        idx = i;
                    }
                    else if(repetitions[idx].second == repetitions[i].second && repetitions[idx].first > repetitions[i].first)
                    {
                        idx = i;
                    }
                }
                return repetitions[idx];
            }
        };

        class Formula_Analyser
        {
         private:
            // Basis regex
            static inline std::regex fgx_regex{"[FGX]" "(" "([(][[:lower:]][AV][[:lower:]][)])" "|" "[[:lower:]]" ")"};
            static inline std::regex normal_regex{"([[:lower:]][AV][[:lower:]])" "|" "[[:lower:]]"};
            static inline std::regex u_regex{"(" "([(][[:lower:]][AV][[:lower:]][)])" "|" "[[:lower:]]" ")"
                                            "U" "(" "([(][[:lower:]][AV][[:lower:]][)])" "|" "[[:lower:]]" ")"};

            std::smatch matching;

         public:
            Formula_Analyser() {}
            Formula_Analyser(const Formula_Analyser&) = default;
            Formula_Analyser(Formula_Analyser&&) noexcept = default;
            Formula_Analyser& operator=(const Formula_Analyser&) = default;
            Formula_Analyser& operator=(Formula_Analyser&&) noexcept = default;
            virtual ~Formula_Analyser() {}

            std::vector<std::string> proceed_input(const std::string& input)
            {
                std::vector<std::string> result;
                if(regex_search(input, matching, fgx_regex) == false)
                {
                    if(regex_search(input, matching, u_regex) == true)
                    {
                        std::string temp;
                        std::istringstream in_str(input);
                        std::getline(in_str, temp, 'U');
                        temp.erase(std::remove_if(temp.begin(), temp.end(), [](unsigned char c){ return c == '(' || c == ')'; }), temp.end());
                        result.push_back(temp);
                        result.push_back("U");
                        std::getline(in_str, temp);
                        temp.erase(std::remove_if(temp.begin(), temp.end(), [](unsigned char c){ return c == '(' || c == ')'; }), temp.end());
                        result.push_back(temp);
                    }
                    else if(regex_search(input, matching, normal_regex) == true)
                    {
                        std::string temp = input;
                        temp.erase(std::remove_if(temp.begin(), temp.end(), [](unsigned char c){ return c == '(' || c == ')'; }), temp.end());
                        result.push_back(temp);
                    }
                    else
                    {
                        return {};
                    }
                }
                else
                {
                    std::string temp = input;
                    temp.erase(std::remove_if(temp.begin(), temp.end(), [](unsigned char c){ return c == '(' || c == ')'; }), temp.end());
                    result.push_back(std::string{temp.begin(), temp.begin() + 1});
                    result.push_back(std::string{temp.begin() + 1, temp.end()});
                }
                return result;
             }
        };

        bool process_normal_proposition(size_t order, const std::string& prop_str) const
        {
            std::vector<char> prop_list;
            prop_list.reserve(prop_str.size()/2 + 1);
            char combination_type = 'O';
            for(size_t i = 0; i < prop_str.size(); i++)
            {
                if(i % 2 == 0)
                {
                    prop_list.push_back(prop_str[i]);
                }
                else
                {
                    combination_type = prop_str[i];
                }
            }

            if(combination_type != 'O')
            {
                if(combination_type == 'A')
                {
                    for(char c : prop_list)
                    {
                        if(graph.check_proposition(order, c) == false)
                            return false;
                    }
                    return true;
                }
                else
                {
                    for(char c : prop_list)
                    {
                        if(graph.check_proposition(order, c) == true)
                            return true;
                    }
                    return false;
                }
            }
            else
            {
                return graph.check_proposition(order, prop_list.front());
            }
        }

     protected:
        Kripke_Structure_Graph graph;
        std::vector<size_t> path;
        std::vector<size_t> path_pattern;
        std::vector<std::string> formula;

        String_Repetition_Detector path_pattern_finder;
        Formula_Analyser analyser;

     public:
        LTL_Model() : graph(0) {}
        explicit LTL_Model(const Kripke_Structure_Graph& KS_graph) : graph(KS_graph) {}
        LTL_Model(const Kripke_Structure_Graph& KS_graph, const std::string& path, const std::string& formula) : graph(KS_graph) { set_path(path); set_formula(formula); }
        LTL_Model(const LTL_Model&) = default;
        LTL_Model(LTL_Model&&) = default;
        LTL_Model& operator=(const LTL_Model&) = default;
        LTL_Model& operator=(LTL_Model&&) = default;

        virtual ~LTL_Model() {}

        // Setting methods
        bool set_graph(const Kripke_Structure_Graph& new_graph)
        {
            if(new_graph.empty() || new_graph.size() != new_graph.max_size())
                return false;

            graph = new_graph;
            return true;
        }

        bool set_path(std::string path)
        {
            if(path.empty() || path[0] != '0' )
            {
                return false;
            }
            else if(std::find_if_not(path.begin(), path.end(), [](unsigned char c) { return std::isdigit(c) || (c == '-'); }) != path.end())
            {
                return false;
            }

            std::pair<size_t, size_t> indices_pair = path_pattern_finder.get_highest_indice_pair(path);
            if(indices_pair.first == npos || indices_pair.second == npos)
            {
                return false;
            }
            else if(indices_pair.second + 1 != path.size())
            {
                return false;
            }

            std::istringstream input_path(path);
            size_t mid_indice = (indices_pair.second + indices_pair.first)/2;
            bool first_check = true;

            std::vector<size_t> temp_path;
            std::vector<size_t> temp_pattern;
            size_t counter = 0;

            while(!input_path.eof() && input_path.good())
            {
                std::string temp;
                std::getline(input_path, temp, '-');

                if(!first_check)
                    counter++;
                counter += temp.size();

                int num = std::stoi(temp);
                if(num < 0)
                {
                    return false;
                }
                else if(first_check && num != 0)
                {
                    return false;
                }

                temp_path.push_back(num);
                first_check = false;

                if(counter - 1 > mid_indice)
                    temp_pattern.push_back(num);
            }

            for(auto iter1 = temp_path.begin(), iter2 = iter1 + 1; iter2 != temp_path.end(); iter1++, iter2++)
            {
                if(graph.check_connection(*iter1, *iter2) == false)
                    return false;
            }

            this->path.swap(temp_path);
            this->path_pattern.swap(temp_pattern);

            return true;
        }

        bool set_formula(std::string formula)
        {
            this->formula = analyser.proceed_input(formula);
            return (!this->formula.empty()) ? true : false;
        }

        const Kripke_Structure_Graph& access_graph() const
        {
            return graph;
        }

        // Main method
        bool run() const
        {
            if(graph.size() != graph.max_size() || path.empty() || path_pattern.empty() || formula.empty())
            {
                std::cerr << "Invalid data!" << std::endl;
                return false;
            }

            if(formula[0] == "X" || formula[0] == "F" || formula[0] == "G")
            {
                if(formula[0] == "X")
                {
                    return process_normal_proposition(path[1], formula[1]);
                }
                else if(formula[0] == "F")
                {
                    for(size_t i : path)
                    {
                        if(process_normal_proposition(i, formula[1]) == true)
                            return true;
                    }
                    return false;
                }
                else
                {
                    for(size_t i : path)
                    {
                        if(process_normal_proposition(i, formula[1]) == false)
                            return false;
                    }
                    return true;
                }
            }
            else
            {
                auto iter = std::find(formula.begin(), formula.end(), "U");
                if(iter == formula.end())
                {
                    return process_normal_proposition(path[0], formula[0]);
                }
                else
                {
                    for(size_t i : path)
                    {
                        if(process_normal_proposition(i, formula[2]) == true)
                            return true;

                        if(process_normal_proposition(i, formula[0]) == false)
                            return false;
                    }
                    return false;
                }
            }
        }

        // Utility methods
        void display_path(std::ostream& out = std::cout) const
        {
            if(path.empty())
                return;

            out << path[0];
            for(size_t i = 1; i < path.size(); i++)
                out << " -> " << path[i];
            out << "\n";
        }

        void display_path_pattern(std::ostream& out = std::cout) const
        {
            if(path_pattern.empty())
                return;

            out << path_pattern[0];
            for(size_t i = 1; i < path_pattern.size(); i++)
                out << " -> " << path_pattern[i];
            out << "\n";
        }

        void display_formula(std::ostream& out = std::cout) const
        {
            for(auto&& str : formula)
                out << str;
            out << "\n";
        }
    };
}

#endif // LCL_HPP_INCLUDED

