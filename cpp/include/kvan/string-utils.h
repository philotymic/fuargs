// -*- c++ -*-
#ifndef __KVAN__STRING_UTILS__HH__
#define __KVAN__STRING_UTILS__HH__

#include <vector>
#include <string>
using namespace std;

vector<string> string_split(const string& s, char del);
string string_join(const vector<string>& vs, char del);
vector<string> string_join(const vector<vector<string>>& vvs, char del);
string string_strip(const string&);
vector<string> string_strip(const vector<string>&);

string evaluate_dollar_var_expr(const string& dv_expr);
string getusername();

string replace_all(string str, const string& from, const string& to);

#endif
