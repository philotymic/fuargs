#include <kvan/csv-io.h>

void CSVColumnsVisitor::visit_null(const LOBKey& path)
{
}

void CSVColumnsVisitor::visit_key(const LOBKey& path)
{
}

void CSVColumnsVisitor::visit_enum(const LOBKey& path, const string& enum_s)
{
  cols.push_back(path);
}
    
void CSVColumnsVisitor::visit_string(const LOBKey& path, const string& s)
{
  cols.push_back(path);
}

void CSVColumnsVisitor::visit_fundamental(const LOBKey& path, const string& v)
{
  cols.push_back(path);
}

void CSVColumnsVisitor::visit_start_map(const LOBKey& path)
{
}
  
void CSVColumnsVisitor::visit_end_map()
{
}

void CSVColumnsVisitor::visit_delimiter()
{
}

void CSVColumnsVisitor::visit_start_array()
{
}

void CSVColumnsVisitor::visit_end_array()
{
}

// ...

void CSVVisitor::visit_null(const LOBKey& path)
{
}

void CSVVisitor::visit_key(const LOBKey& path)
{
}

void CSVVisitor::visit_enum(const LOBKey& path, const string& enum_s)
{
  out << enum_s;
}

void CSVVisitor::visit_string(const LOBKey& path, const string& s)
{
  out << s;
}

void CSVVisitor::visit_fundamental(const LOBKey& path, const string& v)
{
  out << v;
}

void CSVVisitor::visit_start_map(const LOBKey& path)
{
}
void CSVVisitor::visit_end_map()
{
}

void CSVVisitor::visit_delimiter()
{
  out << ",";
}
void CSVVisitor::visit_start_array()
{
  throw runtime_error("not implemented");
}

void CSVVisitor::visit_end_array()
{
  throw runtime_error("not implemented");
}

// ....

vector<string> parse_csv_line(const string& line)
{
  enum class parser_state {
    in_cell, in_cell_expect_second_quote,
    out_of_cell
  };

  vector<string> ret;
  parser_state state{parser_state::out_of_cell};

  string cell;
  for (auto c: line) {
    //cout << "c: " << c << endl;
    if (c == ',') {
      switch (state) {
      case parser_state::in_cell:
	ret.push_back(cell); cell = "";
	state = parser_state::out_of_cell;
	break;
      case parser_state::in_cell_expect_second_quote:
	ret.push_back(cell); cell = "";
	state = parser_state::out_of_cell;
	break;      
      case parser_state::out_of_cell:
	ret.push_back(""); cell = "";
	state = parser_state::out_of_cell;
	break;
      }
    } else if (c == '"') {
      switch (state) {
      case parser_state::in_cell:
	state = parser_state::in_cell_expect_second_quote;
	break;
      case parser_state::in_cell_expect_second_quote: 
	cell += '"';
	state = parser_state::in_cell;
	break;
      case parser_state::out_of_cell:
	state = parser_state::in_cell;
	break;
      }
    } else {
      switch (state) {
      case parser_state::in_cell:
	cell += c;
	state = parser_state::in_cell;
	break;
      case parser_state::in_cell_expect_second_quote:
	{
	  ostringstream m;
	  m << "malformed csv line: " << line
	    << ", cell: " << cell
	    << ", c: " << c;
	  throw runtime_error(m.str());
	}
	break;
      case parser_state::out_of_cell:
	cell += c;
	state = parser_state::in_cell;
	break;	
      }
    }
  }
  
  ret.push_back(cell);
  
  return ret;
}

