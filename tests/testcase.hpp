/**
 * Real Time Protocol Music Instrument Digital Interface Daemon
 * Copyright (C) 2019 David Moreno Montero <dmoreno@coralbits.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include <string>
#include <functional>
#include "../src/logger.hpp"

#define TEST(fn) {#fn, fn}

class TestFail : public std::exception{
public:
  std::string msg;
  const char *filename;
  int line;
  std::string error;

  TestFail(const char *file, int line, const std::string &error){
    this->filename = file;
    this->line = line;
    this->error = error;
    msg = fmt::format("{}:{} Test Fail: {}{}{}", file, line, "\033[1;31m", error, "\033[0m");
  }
  virtual const char *what() const noexcept{
    return msg.c_str();
  }
};

struct Test {
  std::string name;
  std::function<void(void)> fn;
};

class TestCase{
public:
  std::vector<Test> tests;
  bool error = false;

  TestCase(const std::initializer_list<Test> &tests_){
    tests = tests_;
  }

  bool run() {
    int errors = 0;
    int total = tests.size();
    int count = 0;
    for (auto &tcase: tests){
      count += 1;
      INFO("");
      INFO("Test ({}/{}) {} Run", count, total, tcase.name);
      try{
        tcase.fn();
        SUCCESS("Test {} OK", tcase.name);
      } catch (const TestFail &e) {
        logger::log(e.filename, e.line, logger::ERROR, e.error);
        errors += 1;
      } catch (const std::exception &e) {
        ERROR("{}", e.what());
        errors += 1;
      } catch (...){
        ERROR("Unknown exception");
        errors += 1;
      }
    }
    if (errors == 0) {
      INFO("No errors.");
      return true;
    } else {
      error = true;
      ERROR("{} Errors", errors);
      return false;
    }
  }

  int exit_code(){
    return error ? 1 : 0;
  }
};


#define ASSERT_EQUAL(A, B) if ((A) != (B)){ throw TestFail(__FILE__, __LINE__, "Assert [" #A " == " #B "] failed"); }
#define ASSERT_NOT_EQUAL(A, B) if ((A) == (B)){ throw TestFail(__FILE__, __LINE__, "Assert [" #A " != " #B "] failed"); }
#define ASSERT_GT(A, B) if ((A) <= (B)){ throw TestFail(__FILE__, __LINE__, "Assert [" #A " > " #B "] failed"); }
#define ASSERT_GTE(A, B) if ((A) < (B)){ throw TestFail(__FILE__, __LINE__, "Assert [" #A " >= " #B "] failed"); }
#define ASSERT_LT(A, B) if ((A) >= (B)){ throw TestFail(__FILE__, __LINE__, "Assert [" #A " < " #B "] failed"); }
#define ASSERT_LTE(A, B) if ((A) > (B)){ throw TestFail(__FILE__, __LINE__, "Assert [" #A " <= " #B "] failed"); }
#define FAIL(msg) { throw TestFail(__FILE__, __LINE__, msg); }
