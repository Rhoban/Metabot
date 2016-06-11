#include <terminal.h>
#include "math.h"

class Statistic {
 public:
  float min;
  float max;
  float N;
  float mean;
  float mean2;
  float sigma;
  Statistic();
  void reset();
  void push_value(float x);  
  void print_report();
};
