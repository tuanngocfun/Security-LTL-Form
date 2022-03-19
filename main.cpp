#include <iostream>
#include <vector>
#include <sstream>
#include <unordered_map>
using namespace std;
int n;
vector<int> edges[100110];
vector<string> propositional_atomic[100110];
char op[6] = {
    'A',
    'V',
    'U',
    'F',
    'G',
    'X'};
vector<int> path;
bool rec(int, string);
bool funcU(int, string, string);
bool funcG(int, string);
bool funcX(int, string);
bool funcF(int, string);
using size_type = size_t;
static const size_type npos = -1;
void Tokenizes(string s, string del, vector<string> &vertice)
{
	int start = 0;
	int end = s.find(del);
	//int i = 0;
	while (end != -1)
	{
		vertice.push_back(s.substr(start, end - start));
		start = end + del.size();
		end = s.find(del, start);
	}
	vertice.push_back(s.substr(start, end - start));
}
bool CheckPath(string s)
{
	vector<string> vertice;
	Tokenizes(s, "-", vertice);
	stringstream ss;
	ss << vertice[0];
	cout << "vertice[0]: " << endl;
	cout << vertice[0] << endl;
	int x;
	ss >> x;
	cout << "x: " << endl;
	cout << x << endl;
	path.push_back(x);
	cout << "vector<int> path: " << endl;
    for(auto&& it : path)
        cout << it << endl;
	for (std::size_t i = 1; i < vertice.size(); i++)
	{
		bool flag = false;
		string t = vertice[i];
		cout << "t: " << endl;
		cout << t << endl;
		ss.clear();
		ss << vertice[i];
		/*
		cout << "vertice[i]: " << endl;
		cout << vertice[i] << endl;*/
		int temp;
		ss >> temp;
		cout << "temp: " << endl;
		cout << temp << endl;
		path.push_back(temp);
		cout << "vector<int> path: " << endl;
		for(auto&& it : path)
            cout << it << endl;
		for (auto j = edges[x].begin(); j < edges[x].end(); j++)
		{
		    cout << "edges[x]: " << endl;
		    cout << *j << endl;
			if (*j == temp)
			{
				x = temp;
				flag = true;
				break;
			}
		}
		if (!flag){
            return false;
        }
	}
	return true;
}
int isOp(char x)
{
	for (int i = 0; i < 6; i++)
		if (x == op[i]){
            return i;
        }
	return -1;
}
bool rec(int idx, string s)
{
	if (s.length() == 1)
	{
		for (auto i = propositional_atomic[path[idx]].begin(); i < propositional_atomic[path[idx]].end(); i++)
			if (*i == s){
                return true;
			}
		return false;
	}
	int bracket = 0, op_2ele = 0;
	for (std::size_t i = 0; i < s.length(); i++)
	{
		if (s[i] == '(')
			bracket++;
		if (s[i] == ')')
			bracket--;
		if (bracket == 0)
		{
			int op_curr = isOp(s[i]);
			if (op_curr != -1)
			{
				if (op_curr < 3)
				{
					op_2ele++;
					if (op_2ele >= 2)
						return false;
					string s1 = s.substr(0, i);
					string s2 = s.substr(i + 1, s.length() - i);
					if (s1.length() > 1 && s1[0] == '(')
						s1 = s1.substr(1, s1.length() - 2);
					if (s2.length() > 1 && s2[0] == '(')
						s2 = s2.substr(1, s2.length() - 2);
					if (op_curr == 0)
						return rec(idx, s1) && rec(idx, s2);
					else if (op_curr == 1)
						return rec(idx, s1) || rec(idx, s2);
					else
						return funcU(idx, s1, s2);
				}
				else
				{
					string s1 = s.substr(i + 1, s.length());
					if (s1.length() > 1 && s1[0] == '(')
						s1 = s1.substr(1, s1.length() - 2);
					if (op_curr == 3)
						return funcF(idx, s1);
					else if (op_curr == 4)
						return funcG(idx, s1);
					else
						return funcX(idx, s1);
				}
			}
		}
	}
	return false;
}
bool funcU(int idx, string s1, string s2)
{
	for (std::size_t i = idx; i < path.size() - 1; i++)
		if (rec(i, s1))
		{
			if (rec(i + 1, s2))
				return true;
		}
		else
			return false;
	return false;
}
bool funcX(int idx, string ltl)
{
	if (ltl.length() == 1)
	{
		for (auto i = propositional_atomic[idx + 1].begin(); i < propositional_atomic[idx + 1].end(); i++)
			if (*i == ltl)
				return true;
		return false;
	}
	else
	{
		//ltl = ltl.substr(1, ltl.length() - 1);
		return rec(idx + 1, ltl);
	}
}
bool funcG(int idx, string ltl)
{
	for (std::size_t i = idx; i < path.size(); i++)
	{
		//int temp = path[i];
		if (!rec(i, ltl))
			return false;
	}
	return true;
}
bool funcF(int idx, string ltl)
{
	for (std::size_t i = idx; i < path.size(); i++)
		if (rec(i, ltl))
			return true;
	return false;
}
int main()
{
	string s;
	cin >> n;
	for (int i = 0; i < n; i++)
	{
		cout << "Input propositional atomic for s" << i << ": " << endl;
		cin >> s;
		vector<string> prop_atomic;
		Tokenizes(s, ",", prop_atomic);
		for (auto j = prop_atomic.begin(); j < prop_atomic.end(); j++)
			propositional_atomic[i].push_back(*j);
	}
	while (1)
	{
		cout << "Set transition (press e to stop): " << endl;
		cin >> s;
		if (s == "e")
			break;
		vector<string> vertice;
		Tokenizes(s, "-", vertice);
		stringstream ss;
		ss << vertice[0];
        cout << "vertice[0]: " << endl;
        cout << vertice[0] << endl;
		int x, y;
		ss >> x;
		ss.clear();
		ss << vertice[1];
		ss >> y;
		cout << "y: " << endl;
		cout << y << endl;
		edges[x].push_back(y);
	}
	cout << "Enter a path: " << endl;
	string paths, ltl;
	cin >> paths;
	cout << "Enter LTL formula: " << endl;
	cin >> ltl;
	if (CheckPath(paths))
	{
		int rep = -1;
		for (std::size_t i = 1; i <= path.size() / 2; i++)
		{
			int k = path.size();
			bool flag = true;
			for (std::size_t j = 0; j < i; j++)
			{
				//int a = path[k - j - 1];
				//int b = path[k - j - i - 1];
				if (path[k - j - 1] != path[k - j - i - 1])
				{
					flag = false;
					break;
				}
			}
			if (flag){
                rep = k - i;
                //cout << rep << endl;
			}
		}
		cout << (rec(0, ltl) ? "YES" : "NO");
	}
	else
		cout << "NO";
	return 0;
}
