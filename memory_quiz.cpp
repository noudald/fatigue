#include <iostream>
#include <thread>
#include <string>
#include <tuple>
#include <random>
#include <fstream>

#include <boost/filesystem.hpp>

using namespace std;
using namespace boost;

#include "memory_quiz.h"
#include "tools.h"

MemoryQuiz::MemoryQuiz() {
    MemoryQuiz(5);
}

MemoryQuiz::MemoryQuiz(
        unsigned int const number_of_problems,
        unsigned int const sleep_time,
        unsigned int const nnumbers) {
    sleep_time_in_milliseconds = sleep_time;
    number_of_numbers = nnumbers;
    ran = false;
    initQuiz(number_of_problems);
}

void MemoryQuiz::initQuiz(unsigned int const number_of_problems) {
    mt19937::result_type seed = time(0);
    uniform_int_distribution<int> distribution(0,9);
    auto dice = bind(distribution, mt19937(seed));

    for (unsigned int i = 0; i < number_of_problems; i++) {
        string problem;
        for (unsigned int j = 0; j < number_of_numbers; j++) {
            problem += to_string(dice());
        }
        MemoryProblem memory_problem(problem);
        question.push_back(memory_problem);
    }
}

void MemoryQuiz::startQuiz() {
    for (auto &problem : question) {
        cout << "> " << problem.getMemoryProblem();
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(sleep_time_in_milliseconds));
        string blanks(problem.getMemoryProblem().size() + 2, ' ');
        cout << "\r" << blanks << "\r> ";

        string answer;
        cin >> answer;
        unsigned int score = problem.correctNumber(answer);
        answers.push_back(
            tuple<MemoryProblem, string, unsigned int>{problem, answer, score}
        );
    }

    ran = true;
}

void MemoryQuiz::stopQuiz() {
    cout << "memory_problem, answer, correct" << endl;
    for (auto &answer : answers) {
       cout << get<0>(answer).getMemoryProblem()
          << ", " << get<1>(answer)
          << ", " << get<2>(answer) << endl; 
    }
}

void MemoryQuiz::writeResultsPerMemoryProblem(string const filename) {
    if (ran) {
        ofstream outfile;

        if (!filesystem::exists(filename.c_str())) {
            outfile.open(filename);
            outfile << "date,memory_problem,answer,correct" << endl;
        } else {
            outfile.open(filename, ios_base::app);
        }

        for (auto &answer : answers) {
            outfile << getCurrentTime()
                << "," << get<0>(answer).getMemoryProblem()
                << "," << get<1>(answer)
                << "," << get<2>(answer) << endl;
        }
    }
}