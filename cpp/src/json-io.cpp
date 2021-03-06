#include <yajl/yajl_parse.h>
#include <kvan/json-io.h>
#include <kvan/struct-descriptor.h>

void JSONVisitor::visit_null(const LOBKey& path)
{
  out << "null";
}

void JSONVisitor::visit_key(const LOBKey& path)
{
  out << "\"" << path.back() << "\": ";
}

void JSONVisitor::visit_enum(const LOBKey& path, const string& enum_s)
{
  out << "\"" << enum_s << "\"";
}

void JSONVisitor::visit_string(const LOBKey& path, const string& s)
{
  out << "\"" << replace_all(s, "\"", "\\\"") << "\"";
}

void JSONVisitor::visit_fundamental(const LOBKey& path, const string& v)
{
  out << v;
} 

void JSONVisitor::visit_start_map(const LOBKey& path)
{
  out << "{";
}

void JSONVisitor::visit_end_map()
{
  out << "}";
}

void JSONVisitor::visit_delimiter()
{
  out << ", ";
}

void JSONVisitor::visit_start_array()
{
  out << "[";
}

void JSONVisitor::visit_end_array()
{
  out << "]";
}

// json input


struct parser_ctx {
  vector<pair<path_t, optional_string_t>>* ret;
  path_t curr_path;

  parser_ctx(vector<pair<path_t, optional_string_t>>* ret) {
    this->ret = ret;
    curr_path.push_back(make_pair("", mindex_t()));
  }
  vector<pair<string, mindex_t>> get_curr_path();
  string get_mindex(const mindex_t&);
  
  int process_null();
  int process_boolean(int boolean);
  int process_number(const string&);
  int process_string(const string&);
  int process_map_key(const string&);
  int process_start_map();
  int process_end_map();
  int process_start_array();
  int process_end_array();
};

string parser_ctx::get_mindex(const mindex_t& mi)
{
  ostringstream ret;
  if (mi.size() == 1) {
    ret << mi[0];
  } else {
    ret << "(";
    for (size_t i = 0; i < mi.size(); i++) {
      ret << mi[i];
      if (i + 1 < mi.size()) {
	ret << ",";
      }
    }
    ret << ")";
  }
  return ret.str();
}

vector<pair<string, mindex_t>> parser_ctx::get_curr_path()
{
  return curr_path;
}

int parser_ctx::process_null()
{
  //cout << __func__ << endl;
  //cout << get_curr_path() << ": " << "null" << endl;
  ret->push_back(make_pair(get_curr_path(), optional_string_t{})); // null is empty string
  return 1;
}

int parser_ctx::process_boolean(int b)
{
  //cerr << "parser_ctx::process_boolean: " << b << endl;
  //cout << __func__ << endl;
  //cout << get_curr_path() << ": " << b << endl;
  ret->push_back(make_pair(get_curr_path(), optional_string_t{to_string(b)}));
  if (curr_path.back().second.size() > 0) {
    curr_path.back().second.back()++;
  }
  return 1;
}

int parser_ctx::process_number(const string& n)
{
  //cout << __func__ << endl;
  //cout << get_curr_path() << ": " << n << endl;
  ret->push_back(make_pair(get_curr_path(), optional_string_t{n}));
  if (curr_path.back().second.size() > 0) {
    curr_path.back().second.back()++;
  }
  return 1;
}

int parser_ctx::process_string(const string& s)
{
  //cout << __func__ << endl;
  //cout << get_curr_path() << ": " << s << endl;
  ret->push_back(make_pair(get_curr_path(), optional_string_t{s}));
  if (curr_path.back().second.size() > 0) {
    curr_path.back().second.back()++;
  }
  return 1;
}

int parser_ctx::process_map_key(const string& k)
{
  //cerr << __func__ << " " << get_curr_path() << endl;
  curr_path.back().first = k;
  return 1;
}
  
int parser_ctx::process_start_map()
{
  //cerr << __func__ << " " << get_curr_path() << endl;
  curr_path.push_back(make_pair("", mindex_t()));
  return 1;
}

int parser_ctx::process_end_map()
{
  //cerr << __func__ << " " << get_curr_path() << endl;
  curr_path.pop_back();
  if (curr_path.back().second.size() > 0) {
    curr_path.back().second.back()++;
  }
  return 1;
}

int parser_ctx::process_start_array()
{
  //cerr << __func__ << " " << get_curr_path() << endl;
  curr_path.back().second.push_back(0);
  return 1;
} 

int parser_ctx::process_end_array()
{
  //cerr << __func__ << " " << get_curr_path() << endl;
  curr_path.back().second.pop_back();
  if (curr_path.back().second.size() > 0) {
    curr_path.back().second.back()++;
  }    
  return 1;
}

static int reformat_null(void * ctx)
{
  auto t = (parser_ctx*)ctx;
  return t->process_null();
}

static int reformat_boolean(void * ctx, int boolean)
{
  auto t = (parser_ctx*)ctx;
  return t->process_boolean(boolean);
}

static int reformat_number(void * ctx, const char * s, size_t l)
{
  auto t = (parser_ctx*)ctx;
  string v(s, l);
  return t->process_number(v);
}

static int reformat_string(void * ctx, const unsigned char * stringVal, size_t stringLen)
{
  auto t = (parser_ctx*)ctx;
  string v((char*)stringVal, stringLen);
  return t->process_string(v);
}

static int reformat_map_key(void * ctx, const unsigned char * stringVal, size_t stringLen)
{
  auto t = (parser_ctx*)ctx;
  string map_key((char*)stringVal, stringLen);
  return t->process_map_key(map_key);
}

static int reformat_start_map(void * ctx)
{
  auto t = (parser_ctx*)ctx;
  return t->process_start_map();
}

static int reformat_end_map(void * ctx)
{
  auto t = (parser_ctx*)ctx;
  return t->process_end_map();
}

static int reformat_start_array(void * ctx)
{
  auto t = (parser_ctx*)ctx;
  return t->process_start_array();
}

static int reformat_end_array(void * ctx)
{
  auto t = (parser_ctx*)ctx;
  return t->process_end_array();
}

static yajl_callbacks cbs = {
  reformat_null,
  reformat_boolean,
  NULL,
  NULL,
  reformat_number,
  reformat_string,
  reformat_start_map,
  reformat_map_key,
  reformat_end_map,
  reformat_start_array,
  reformat_end_array
};

vector<pair<path_t, optional_string_t>> from_json(const string& json_s)
{
  vector<pair<path_t, optional_string_t>> ret;
  parser_ctx ctx(&ret);
  
  auto hand = yajl_alloc(&cbs, NULL, &ctx);
  yajl_config(hand, yajl_allow_comments, 1);
  
  yajl_status stat = yajl_parse(hand, (const unsigned char*)json_s.c_str(), json_s.size());
  if (stat != yajl_status_ok) {
    stat = yajl_complete_parse(hand);
    if (stat != yajl_status_ok) {
      throw runtime_error("from_json: yajl failed");
    }
  }

  yajl_free(hand);

  return ret;
}
