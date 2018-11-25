// Copyright 2018 Noud Aldenhoven

#include <sqlite3.h>

#include <fstream>
#include <iostream>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

using boost::program_options::bool_switch;
using boost::program_options::notify;
using boost::program_options::options_description;
using boost::program_options::value;
using boost::program_options::variables_map;

#include "./sqlite3_helper.h"

int main(int argc, char **argv) {
  boost::filesystem::path csv_input_file;
  boost::filesystem::path sqlite_output_file;

  options_description desc("Allowed options");
  desc.add_options()("help,h", "Print help usage.")(
      "in,i", value(&csv_input_file), "CSV input file.")(
      "out,o", value(&sqlite_output_file), "SQLite3 output file.");

  variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);

  if (vm.count("help") || vm.count("h") || !(vm.count("in") || vm.count("i")) ||
      !(vm.count("out") || vm.count("o"))) {
    std::cout << "A small too to convert CSV speed file to sqlite database."
              << std::endl
              << desc;
    return 0;
  }

  SQLite3 db(sqlite_output_file);
  db.execute(
      "create table speed("
      "id int primary key not null,"
      "date string not null,"
      "problem string not null,"
      "solution int not null,"
      "number_of_tries int not null,"
      "time_to_solve real not null"
      ");");
  db.commit();

  if (!boost::filesystem::exists(csv_input_file)) {
    std::cerr << "CSV input file doesn't exist: " << csv_input_file
              << std::endl;
    return 1;
  }
  std::ifstream infile(csv_input_file.c_str());
  std::string sinput;
  std::getline(infile, sinput);

  std::vector<std::string> sinput_split;
  int i = 0;
  while (std::getline(infile, sinput)) {
    boost::split(sinput_split, sinput, boost::is_any_of(","));
    db.execute(
        "insert into speed (id, date, problem, solution, number_of_tries, "
        "time_to_solve) values (" +
        std::to_string(i) + ",\"" + sinput_split.at(0) + "\",\"" +
        sinput_split.at(1) + "\"," + sinput_split.at(2) + "," +
        sinput_split.at(3) + "," + sinput_split.at(4) + ");");
    db.commit();
    if (i % 10 == 0) {
      std::cout << ".";
      std::flush(std::cout);
    }
    i++;
  }
  std::cout << std::endl;

  return 0;
}
