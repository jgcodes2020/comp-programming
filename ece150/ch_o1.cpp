#include <iomanip>
#include <ios>
#include <iostream>

void print_calendar(int sdow, int days) {
  int week1_days = 7 - sdow;
  int rem_days = days - week1_days;
  int rem_weeks = rem_days / 7;
  int weekn_days = rem_days % 7;

  std::cout << std::right << std::setfill(' ');

  // 1st week
  int day = 1;
  for (int i = 0; i < sdow; i++) {
    std::cout << "    ";
  }
  for (int i = 0; i < week1_days; i++) {
    std::cout << std::setw(3) << day << ' ';
    ++day;
  }
  std::cout << '\n';

  // remaining weeks
  for (int i = 0; i < rem_weeks; i++) {
    for (int j = 0; j < 7; j++) {
      std::cout << std::setw(3) << day << ' ';
      ++day;
    }
    std::cout << '\n';
  }
  // final week (if month doesn't end on Sat.)
  if (weekn_days > 0) {
    for (int i = 0; i < weekn_days; i++) {
      std::cout << std::setw(3) << day << ' ';
      ++day;
    }
    std::cout << '\n';
  }
}

int main() {
  int year = 1970;

  std::cin >> year;
  if (year < 1583) {
    return 1;
  }

  // for calculating DoW of Jan. 1st, you need to know number of years/leap years leading up to current year (not including)
  int last_y = year - 1;
  // (# years divisible by 4 since 1583) - (# years divisible by 400 since 1583)
  int count_ly = ((last_y - 1580) / 4) - ((last_y - 1200) / 400);
  int count_y = last_y - 1583;

  // start DoW on Jan. 1
  int curr_dow = (6 + count_y + count_ly) % 7;

  #define PRINT_CAL(n) do { print_calendar(curr_dow, n); curr_dow = (curr_dow + n) % 7; } while (false)

  std::cout << "January\n";
  PRINT_CAL(31);

  std::cout << "February\n";
  if ((year % 4 == 0) && (year % 400 != 0)) {
    PRINT_CAL(29);
  }
  else {
    PRINT_CAL(28);
  }

  std::cout << "March\n";
  PRINT_CAL(31);

  std::cout << "April\n";
  PRINT_CAL(30);
  
  std::cout << "May\n";
  PRINT_CAL(31);
  
  std::cout << "June\n";
  PRINT_CAL(30);
  
  std::cout << "July\n";
  PRINT_CAL(31);
  
  std::cout << "August\n";
  PRINT_CAL(31);
  
  std::cout << "September\n";
  PRINT_CAL(30);
  
  std::cout << "October\n";
  PRINT_CAL(31);
  
  std::cout << "November\n";
  PRINT_CAL(30);
  
  std::cout << "December\n";
  PRINT_CAL(31);

  return 0;
}