#include <terminal.h>
#include "math.h"
#include "statistic.h"

Statistic::Statistic() {
  reset();
}

void Statistic::reset() {
  min = 0;
  max = 0;
  N = 0;
  mean = 0.0;
  sigma = 0.0;
}

void Statistic::push_value(float x) {
  if (N==0) {
    min = x;
    max = x;
  } else {
    if (x < min) min = x;
    if (x > max) max = x;
  }
  N++;
  mean = (mean * (N-1) + x) / N; 
  mean2 = (mean2 * (N-1) + x*x) / N;
  sigma = sqrt(mean2 - mean*mean);
}

void Statistic::print_report() {
  terminal_io()->print("  N = ");
  terminal_io()->println(N);
  terminal_io()->print("  mean = ");
  terminal_io()->println(mean);
  terminal_io()->print("  sigma = ");
  terminal_io()->println(sigma);
  terminal_io()->print("  min = ");
  terminal_io()->println(min);
  terminal_io()->print("  max = ");
  terminal_io()->println(max);
}

