#include <terminal.h>
#include "opticals.h"
#include "dc.h"
#include "math.h"
#include "statistic.h"
#include "imu.h"
#include "buzzer.h"
#include "leds.h"

TERMINAL_PARAMETER_FLOAT(smooth_pos_factor, "wheel position smoothing", 1.0);
TERMINAL_PARAMETER_FLOAT(start_wheel_delay, "starting delay for make wheel turn (sec)", 0.01);
TERMINAL_PARAMETER_FLOAT(k_vel, "gain for velocity servoing", 50.0);
TERMINAL_PARAMETER_FLOAT(k_i_vel, "integral gain for velocity servoing", 0.1);
TERMINAL_PARAMETER_FLOAT(k_rot, "gain for rotation servoing", 100.0);
TERMINAL_PARAMETER_FLOAT(k_i_rot, "integral gain for rotation servoing", 0.1);
TERMINAL_PARAMETER_FLOAT(k_f, "gain to balance friction", 0.0); /* + */
TERMINAL_PARAMETER_FLOAT(k_magic, "ratio x/y ... magic ...", 1.0);
TERMINAL_PARAMETER_FLOAT(k_turn, "gain rotation order", 0.002);
TERMINAL_PARAMETER_FLOAT(k_order, "gain dx/dycommand", 1.5);
TERMINAL_PARAMETER_FLOAT(k_order_rot, "gain rotation command", 0.02);
TERMINAL_PARAMETER_FLOAT(smooth_so, "smoothing speed order at low level", 0.50);

#define IMPOSSIBLE_BRUT_VAL 10000
#define DEFAULT_ORDER 2500
#define MIN_MOVE_ORDER 1500
#define MAX_ORDER 3000

float stamina;


/* colle vers les capteurs optiques */
float wheel_optical_get(int wheel_idx) {
  if (wheel_idx==0) return optical_get(2);
  if (wheel_idx==1) return optical_get(0);
  if (wheel_idx==2) return optical_get(1);
  return 0.0;
}

int led_of_wheel(int i) {
  if (i==0) return 1;
  if (i==1) return 3;
  if (i==2) return 2;
  return 1;
}

#define MEASURE_SIZE 10
static float meas_ord[3][MEASURE_SIZE];
static float meas_vel[3][MEASURE_SIZE];
static int meas_nb[3] = { 0, 0, 0 };

#define QUEUE_SIZE 20
static float pos_q[3][QUEUE_SIZE];
static long int time_q[3][QUEUE_SIZE];

static float old_x = 0.0;
static float old_y = 0.0;
static float old_turn = 0.0;
float dx = 0.0;
float dy = 0.0;
float turn = 0.0;

int sign(float x) { return x >= 0 ? 1 : -1; }

typedef enum {
  Immobile,
  Starting,
  Moving
} WheelState;


/* Etat de l'asservissement en rotation */
static float orient_tgt;
static float orient_I;
static bool asserv_rot;
static float error_orient;

class Wheel {
public:
  int idx;
  
  int min_optic;
  int max_optic;
  
  long int curr_t;
  long int last_t;
  int delta_t;
  
  float delta_queue;
  float queue_duration;
  float vel_soundness;
  float curr_speed;
  int queue_size;
  float sig_x;
  float sig_y;
  float queue_mean_pos;
  
  float brut_pos;
  float smooth_pos;

  float speed_tgt;
  float speed_corr_ord;

  /* le signe de l'ordre détermine le sens de rotation du robot (trigo vue du haut)
   * i.e. pour un ordre positif, la roue tourne dans le sens trigo vue de l'extérieur
   * le sticker va alors du noir vers le blanc. */
  float brut_speed_order;
  float smooth_speed_order;

  WheelState state;
  int state_init_t;
  float tgt_order;
  float starting_delay; 

  float arrach_power;

  /* turn counter */
  float count_last_pos;
  float count_last_t;
  float turn_counter;
  float turn_time;
  float last_turn_time;

  bool forcing;
  long int forcing_init;
  int forcing_turn_nb;
  int forcing_resetting;

  Wheel() {
    max_optic = 0;
    min_optic = IMPOSSIBLE_BRUT_VAL;
    idx = 0;
    curr_t = -1;
    last_t = -1;
    brut_pos = -1;
    speed_tgt = 0;

    count_last_pos = -1;
    count_last_t = -1;
    turn_counter = 0;
    last_turn_time = -1;

    starting_delay = 1.0; 
    arrach_power = 2000;
    speed_tgt = 0;
    meas_increasing = true;
    smooth_speed_order = 0.0;

    forcing = false;
    forcing_turn_nb = 0;
    forcing_init = -1;
    forcing_resetting = -1;
  }

  void init(int _idx) {
    idx = _idx;
    for (int i=0; i<QUEUE_SIZE; i++) {
      time_q[idx][i] = -1;
      pos_q[idx][i] = -1;
    }
    set_brut_speed_order(0);
    state = Immobile;
  }

  int direction() {
    if (tgt_order == 0) return 0;
    return sign(tgt_order);
  }

  bool check_forcing() {
    if (!forcing) {
      if (is_immobile() && fabs(brut_speed_order) > MIN_MOVE_ORDER) {
	forcing_turn_nb++;
	forcing_resetting = 0;
      }
      else {
	forcing_resetting++;
	if (forcing_resetting > 5) {
	  forcing_turn_nb=0;
	  forcing_init=-1;
	  forcing_resetting = 0;
	}
      }

      if (forcing_turn_nb == 200) {
	forcing_init = micros();
	buzzer_play(MELODY_CUSTOM);
	led_set_mode(LEDS_CUSTOM);
	led_color_set(led_of_wheel(idx), 255, 0, 0);
	terminal_io()->println("CAUTION: forcing");
	forcing = true;
      }
    }
    
    if (forcing && (micros() - forcing_init) > 3*1000000) {
      led_set_mode(LEDS_CUSTOM);
      led_color_set(led_of_wheel(idx), 255, 255, 255);
      forcing_turn_nb=0;
      forcing_init=-1;
      forcing = false;
    }
    
    if (forcing) {
      set_speed(0);
    }

    return forcing;
  }

  bool is_forcing() {
    return forcing;
  }

  bool meas_increasing;

  void push_measure(float order, float vel) {
    if (meas_nb[idx] == MEASURE_SIZE) {
      terminal_io()->println("Caution: too much measure");
      return;
    }
    int i=0;
    while (i<meas_nb[idx] && meas_ord[idx][i] < order) i++;
    for (int j=meas_nb[idx]; j>i; j--) {
      meas_ord[idx][j] = meas_ord[idx][j-1]; 
      meas_vel[idx][j] = meas_vel[idx][j-1];
    }
    meas_ord[idx][i] = order;
    meas_vel[idx][i] = vel;
    meas_nb[idx]++;

    /* on checke si les measures forment une fonction croissante
     * auquel cas, on pourra faire une interpolation linéaire. */
    meas_increasing = true;
    for (i=0; i<meas_nb[idx]-1; i++)
      if (meas_vel[idx][i] > meas_vel[idx][i+1]) {
	meas_increasing = false;
	break;
      }
  }

  float get_order_from_lin_interpolation(float vel) {
    if (meas_nb[idx] == 0) return DEFAULT_ORDER;
    int i=0;
    while (i<meas_nb[idx] && meas_vel[idx][i] < vel) i++;
    if (i==0) return meas_ord[idx][0];
    if (i==meas_nb[idx]) return meas_ord[idx][i-1];
    float div = meas_vel[idx][i]-meas_vel[idx][i-1];
    if (div == 0) return 0.0;
    return meas_ord[idx][i-1] + 
	     (vel - meas_vel[idx][i-1]) / div *
	     (meas_ord[idx][i] - meas_ord[idx][i-1]);
  }

  float get_static_order(float vel) {
    if (vel == 0) return 0.0;
    if (meas_nb[idx] == 0) return DEFAULT_ORDER;
    if (meas_nb[idx] == 1) return meas_ord[idx][0];
    if (meas_increasing)
      return get_order_from_lin_interpolation(vel);
    // TODO moindres carrés ?
    return DEFAULT_ORDER;
  }

  float mean_pos(float duration) {
    float m = 0.0;
    int n=0;
    while (n < QUEUE_SIZE && time_q[idx][n] >= curr_t-duration*1000000) {
      m += pos_q[idx][n];
      n++;
    }
    if (n==0) return 0.0;
    m /= n;
    return m;
  }

  void update_turn_counter() {
    float count_thresh = 2000.0;

    float curr_pos = mean_pos(0.010);
    if (count_last_t > 0) {
      int s = sign(brut_speed_order);
      if ((s * (curr_pos - count_last_pos)) > count_thresh) {
	  if (last_turn_time > 0) {
	    turn_counter += s * 1.0 / 3.0;
	    turn_time = 3 * ((float) (curr_t - last_turn_time)) / 1000000;
	  }
	  last_turn_time = curr_t;
	  count_last_pos = curr_pos;
	}
      if (-s*count_last_pos < -s*curr_pos)
	count_last_pos = curr_pos;
    }
    else
      count_last_pos = curr_pos;
    count_last_t = curr_t;
  }

  void reset_count() {
    turn_counter = 0;
  }

  float get_speed() {
    if (turn_time == 0) return 0.0;
    if (brut_speed_order == 0) return 0.0;
    if (is_immobile()) return 0.0;
    if (state == Immobile) return 0.0;
    float abs_v = 1.0 / turn_time;
    if (abs_v > get_absolute_speed_bound())
      abs_v = get_absolute_speed_bound();
    return sign(brut_speed_order) * abs_v;
  }

  float get_absolute_speed_bound() {
    if (curr_t == last_turn_time) return 0;
    return 1.0 / (3 * ((float) (curr_t - last_turn_time)) / 1000000);
  }

  /* retourne la vitesse en tour/sec.
   * le signe est le sens trigo en regardant la roue de l'extérieur */
  float update_queue() {
    // TODO A optimiser...

    if (time_q[idx][0] < 0) return 0.0;
    int min_t = time_q[idx][0];
    for (int i=1; i<QUEUE_SIZE; i++) {
      if (time_q[idx][i] < -1) break;
      if (min_t > time_q[idx][i]) min_t = time_q[idx][i];
    }
      
    queue_mean_pos = 0;
    float T = 0;
    int N=0;
    for (int i=0; i<QUEUE_SIZE; i++) {
      if (time_q[idx][i] < 0) break;
      T += time_q[idx][i] - min_t;
      queue_mean_pos += pos_q[idx][i];
      N++;
    }
    if (N==0) return 0.0;
    T /= N;
    queue_mean_pos /= N;

    delta_queue = pos_q[idx][N-1] - pos_q[idx][0];
    queue_duration = ((float) (time_q[idx][0] - time_q[idx][N-1])) / 1000000;

    float sig_xy = 0.0;
    float sig2_x = 0.0;
    float sig2_y = 0.0;
    for (int i=0; i<N; i++) {
      float a = (time_q[idx][i]-min_t) - T;
      float b = pos_q[idx][i]-queue_mean_pos;
      sig_xy += a*b;
      sig2_x += a*a;
      sig2_y += b*b;
    }
    sig_xy /= N;
    sig2_x /= N;
    sig_x = sqrt(sig2_x);
    sig2_y /= N;
    sig_y = sqrt(sig2_y);
    if (sig_x==0 || sig_y==0) return 0.0;
    vel_soundness = sig_xy / (sig_x * sig_y);
    if ((max_optic - min_optic)==0) return 0.0;
    float res = 1000000 * sig_xy / sig2_x / (max_optic - min_optic);
    if (fabs(vel_soundness) > 0.80) /* 0.80 fixé par expérience */ {
      curr_speed = res;
    }
    return res;
  }

  void push_pos(int t, float pos) {
    // implementation rapide (faire une vraie fifo) ...
    for (int i=QUEUE_SIZE-2; i>=0; i--) {
      pos_q[idx][i+1] = pos_q[idx][i];
      time_q[idx][i+1] = time_q[idx][i];
    }
    pos_q[idx][0] = pos;
    time_q[idx][0] = t;
    if (queue_size < QUEUE_SIZE)
      queue_size++;
  }
  
  void change_state(WheelState st) {
    state = st;
    state_init_t = micros();
  }

  void push_new_order(float _tgt_order) {
    tgt_order = _tgt_order;
    if (tgt_order == 0) set_brut_speed_order(0);
    if (state == Moving) set_brut_speed_order(tgt_order);
  }

  bool is_immobile() {
    return sig_y < 60.0;
  }

  void tick() {    

    curr_t = micros();
    if (curr_t > 0 && last_t > 0)
      delta_t = curr_t - last_t; 
    last_t = curr_t;

    brut_pos = wheel_optical_get(idx); /* note: valeur faible pour le blanc, élevée pour le noir */
    if (min_optic > brut_pos) min_optic = brut_pos;
    if (max_optic < brut_pos) max_optic = brut_pos;
    
    /* lissage de la position brute */
    if (brut_pos < 0) smooth_pos = brut_pos;
    else smooth_pos = (1-smooth_pos_factor) * smooth_pos + smooth_pos_factor * brut_pos;

    /* on conserve l'historique des positions */
    push_pos(curr_t, smooth_pos);
    update_queue(); // TODO: a optimiser ! Tout le temps de calcul est là...
    update_turn_counter();

    if (check_forcing()) return; 

    if (state == Starting) {
      if (curr_t - state_init_t > start_wheel_delay*1000000) {
	set_brut_speed_order(tgt_order);
	change_state(Moving);
      }
      else {
	set_brut_speed_order(sign(tgt_order) * arrach_power);
      }
    }
    if (is_immobile())
      change_state(Immobile);
    if (state == Immobile && tgt_order != 0.0) 
      change_state(Starting);

    if (state == Moving) {
      float error = speed_tgt - get_speed();
      speed_corr_ord += k_i_vel * error;

      /* asservissement en rotation */
      bool asserv_rot = (dx != 0 || dy != 0);
      float p_corr_rot = 0.0;
      if (asserv_rot) {
	orient_tgt += k_turn * turn;
	while (orient_tgt < -180.0) orient_tgt += 360.0;
	while (orient_tgt > 180.0) orient_tgt -= 360.0;	  
	error_orient = orient_tgt - imu_gyro_yaw();
	while (error_orient < -180.0) error_orient += 360.0;
	while (error_orient > 180.0) error_orient -= 360.0;	  
	orient_I = k_i_rot * error_orient;
	speed_corr_ord += orient_I;
	p_corr_rot = k_rot * error_orient;
      }
      set_brut_speed_order(tgt_order + k_vel * error + speed_corr_ord + p_corr_rot);
    }
  }

  void set_brut_speed_order(float s) {
    if (s==0) change_state(Immobile);
    brut_speed_order = s;
    smooth_speed_order = (1.0-smooth_so)*smooth_speed_order + smooth_so*s;
    if (s==0) smooth_speed_order = 0;
    s = smooth_speed_order;

    if (s < -MAX_ORDER) s = -MAX_ORDER;
    if (s > MAX_ORDER) s = MAX_ORDER;

    /* corrections pour coller au bas niveau */
    if (idx == 0) {}
    if (idx == 1) { s = -s; }
    if (idx == 2) { s = -s; }

    if (s>1400) { s = 1400 + stamina*(s-1400); }
    if (s<-1400) { s = -1400 + stamina*(s+1400); }

    dc_single_command(idx, (int) s);
  }

  /* speed est exprimée en tour/sec */
  void set_speed(float speed) {
    speed_tgt = speed;
    speed_corr_ord = 0.0;
    if (speed_tgt==0) {
      push_new_order(0);
      return;
    }
    float abs_order = get_static_order(fabs(speed_tgt));
    push_new_order(sign(speed_tgt) * abs_order);
  }

  void print_state() {
    terminal_io()->print("Wheel ");
    terminal_io()->print(idx);
    terminal_io()->println(":");
    terminal_io()->print("  min pos :");
    terminal_io()->println(min_optic);
    terminal_io()->print("  max pos :");
    terminal_io()->println(max_optic);
    terminal_io()->print("  delta t :");
    terminal_io()->println(delta_t);
    terminal_io()->print("  brut position: ");
    terminal_io()->println(brut_pos);
    terminal_io()->print("  brut order: ");
    terminal_io()->println(brut_speed_order);
    terminal_io()->print("  velocity tgt: ");
    terminal_io()->print(speed_tgt);
    terminal_io()->println("tour/sec");
    terminal_io()->print("  immediate velocity: ");
    terminal_io()->print(curr_speed);
    terminal_io()->println("tour/sec");
    terminal_io()->print("  vel soundness :");
    terminal_io()->println(vel_soundness);
    terminal_io()->print("  delta_queue :");
    terminal_io()->println(delta_queue);
    terminal_io()->print("  queue_duration :");
    terminal_io()->print(queue_duration);
    terminal_io()->println(" sec");
    terminal_io()->print("  sig_y :");
    terminal_io()->println(sig_y);
    terminal_io()->print("  turn :");
    terminal_io()->println(turn_counter);
    terminal_io()->print("  speed :");
    terminal_io()->print(get_speed());
    terminal_io()->println("  turn/sec");
  }

};

static float default_ord_vel[][2] = {
  { 1500.00, 0.39 },
  { 1666.00, 0.47 },
  { 1832.00, 0.55 },
  { 1998.00, 0.63 },
  { 2164.00, 0.75 },
  { 2330.00, 0.91 },
  { 2496.00, 1.08 },
  { 2662.00, 1.17 },
  { 2828.00, 1.21 },
  { 2994.00, 1.22 }
};
static int default_meas_nb = 10;

static float default_min_optical[3] = { 205, 143, 221 };
static float default_max_optical[3] = { 4095, 4080, 4083 };

static Wheel wheel[3];

static int t_init = -1;

typedef enum {
  WheelCalibration,
  WheelReady
} WheelMode;

static WheelMode wheel_mode = WheelReady;
static int wheel_idx = 0;
static int curr_val;
static float curr_order = 0;
static float last_measure = 0.0;

#define CALIB_MIN_ORDER 1500
#define CALIB_MAX_ORDER 3000

typedef enum {
  CalibMinPower,
  CalibMinMax,
  CalibSpeedInit,
  CalibSpeedStart,
  CalibSpeedMeasure,
} CalibState;

CalibState calib_state;

Statistic stat;

void print_calib_state() {
  terminal_io()->println("- Wheel Calibration -");
  for (int i=0; i<3; i++) {
    terminal_io()->print("* wheel ");
    terminal_io()->print(i);
    terminal_io()->print(" min:");
    terminal_io()->print(wheel[i].min_optic);
    terminal_io()->print(' ');
    terminal_io()->print(" max:");
    terminal_io()->print(wheel[i].max_optic);
    terminal_io()->println();
  }
}

void wheel_init() {
  for (int i=0; i<3; i++) { 
    wheel[i].init(i);
    wheel[i].min_optic = default_min_optical[i];
    wheel[i].max_optic = default_max_optical[i];
    for (int k=0; k<default_meas_nb; k++)
      wheel[i].push_measure(default_ord_vel[k][0], default_ord_vel[k][1]);
  }
  wheel_mode = WheelReady;
  t_init = -1;
  stamina = 1.0;
}

void calib_wheel(int idx) {
  terminal_io()->print("Calibration procedure for wheel ");
  terminal_io()->println(idx);
  wheel_mode = WheelCalibration;
  calib_state = CalibSpeedInit;
  t_init = millis();
  wheel_idx = idx;
  meas_nb[idx] = 0;
}

void stop_calib_wheel() {
  wheel_mode = WheelReady;
  curr_order = 0;
  wheel[wheel_idx].push_new_order(0);
}

void print_measure_array() {
  terminal_io()->println("static float default_ord_vel[][2] = {");
  for (int i=0; i<meas_nb[wheel_idx]; i++) {
    terminal_io()->print("  { ");
    terminal_io()->print(meas_ord[wheel_idx][i]);
    terminal_io()->print(", ");
    terminal_io()->print(meas_vel[wheel_idx][i]);
    terminal_io()->print(" }");
    if (i==meas_nb[wheel_idx]-1)
      terminal_io()->println();
    else
      terminal_io()->println(",");
  }
  terminal_io()->println("};");

  terminal_io()->print("static int default_meas_nb = ");
  terminal_io()->print(meas_nb[wheel_idx]);
  terminal_io()->println(";");
}

void print_measure_values() {
  for (int i=0; i<meas_nb[wheel_idx]; i++) {
    terminal_io()->print(meas_ord[wheel_idx][i]);
    terminal_io()->print(" ");
    terminal_io()->print(meas_vel[wheel_idx][i]);
    terminal_io()->println();
  }
}

void wheel_calib_tick() {
  // 1. Detection le min power les roues
  if (t_init < 0) { 
    t_init = millis();
    calib_state = CalibMinMax;
    curr_val = wheel_optical_get(wheel_idx);
  }

  if (calib_state == CalibMinMax) {
    for (int i=0; i<3; i++) wheel[i].set_brut_speed_order(2500);
    for (int i=0; i<3; i++) {
      if (wheel[i].min_optic > wheel_optical_get(i)) wheel[i].min_optic = wheel_optical_get(i);
      if (wheel[i].max_optic < wheel_optical_get(i)) wheel[i].max_optic = wheel_optical_get(i);
    }
    if (millis() > t_init + 2500) {
      for (int i=0; i<3; i++) wheel[i].set_brut_speed_order(0);
      wheel_mode = WheelReady;
      print_calib_state();
    }
  }

  if (calib_state == CalibSpeedInit) {
    for (int i=0; i<3; i++) {
      wheel[i].set_brut_speed_order(0);
      meas_nb[i]=0;
    }

    if ((millis() - t_init) > 1000) {
      calib_state = CalibSpeedStart;
      t_init = millis();
    }
  }

  if (calib_state == CalibSpeedStart) {
    if (curr_order == 0) 
      curr_order = CALIB_MIN_ORDER;
    else { 
      if (curr_order > 0) 
	curr_order = -curr_order;
      else 
	curr_order = -curr_order + (CALIB_MAX_ORDER - CALIB_MIN_ORDER) / (MEASURE_SIZE-1);
    }
    if (curr_order <= CALIB_MAX_ORDER) {
      terminal_io()->print("- testing order ");
      terminal_io()->println(curr_order);
      wheel[wheel_idx].push_new_order(curr_order);
      t_init = millis();
      stat.reset();
      calib_state = CalibSpeedMeasure;
    }
    else {
      stop_calib_wheel();
      print_measure_array();
      print_measure_values();
    }
  }

  if (calib_state == CalibSpeedMeasure) {
    int elapsed = millis() - t_init; 
    if (elapsed > 1000) {
      float v = wheel[wheel_idx].get_speed();
      stat.push_value(v);
    }
    if (elapsed > 3 * 1000) {
      stat.print_report();
      if (curr_order > 0) last_measure = stat.mean;
      if (curr_order < 0) {
	float v = (last_measure - stat.mean) / 2;
	if (meas_nb[wheel_idx]==0 || meas_vel[wheel_idx][meas_nb[wheel_idx]-1] < v) {
	  terminal_io()->print("push measure : ");
	  terminal_io()->print(fabs(curr_order));
	  terminal_io()->print(" -> ");
	  terminal_io()->println(v);
	  wheel[wheel_idx].push_measure(fabs(curr_order), v);
	  print_measure_array();
	} 
      }
      calib_state = CalibSpeedStart;
      t_init = millis();
    }
  }

}

long int last_stam_t = -1;
double stam_dt = 0;

void update_stamina() {
  if (last_stam_t < 0) {
    last_stam_t = micros();
    return;
  }
  else {
    long int curr_t = micros();
    stam_dt = ((double) (curr_t - last_stam_t)) / 1000000;
    last_stam_t = curr_t;
  }

  float power = 0;
  for (int i=0; i<3; i++)
    if (fabs(wheel[i].speed_tgt) > 0.5) power = 1;
  float dstam = power > 0 ? -0.02 : 0.06; 
  // stamina += dstam * stam_dt;
  if (stamina < 0.2) stamina = 0.2;
  if (stamina > 1.0) stamina = 1.0;
}

uint32 wheel_glb_t;
double stam_x = 0;

void wheel_tick() {
  update_stamina();

  led_set_mode(LEDS_CUSTOM);
  float freq = 1.0 + (1.0/(pow(stamina,2)) - 1);
  stam_x += 2*M_PI*freq*stam_dt;
  int v = (int) (255 * (1 + cos(stam_x))/2);
  bool some_forcing = false;
  for (int i=0; i<3; i++)
    if (wheel[i].forcing) some_forcing = true;
  if (some_forcing) {
    for (int i=0; i<3; i++)
      if (!wheel[i].forcing) led_color_set(led_of_wheel(i), v,v,v);
  }
  else {
    led_all_color_set(v,v,v);
  }

  for (int i=0; i<3; i++)
    wheel[i].tick();

  if (wheel_mode == WheelCalibration) {
    wheel_calib_tick();
  }
} 

TERMINAL_COMMAND(pw, "print wheel states")
{
  for (int i=0; i<3; i++)
    wheel[i].print_state();
}

TERMINAL_COMMAND(wheel_calib, "calibrate the speed of a wheel w_calib <wheel idx>")
{
  if (argc == 1) {
    calib_wheel(atoi(argv[0]));
  }
}

TERMINAL_COMMAND(ws, "wheel speed - ws <wheel idx> <speed rev/sec>")
{
  if (argc == 2) {
    wheel[atoi(argv[0])].set_speed(atof(argv[1]));
  }

  if (argc == 3) {
    wheel[0].set_speed(atof(argv[0]));
    wheel[1].set_speed(atof(argv[1]));
    wheel[2].set_speed(atof(argv[2]));
  }
}

TERMINAL_COMMAND(s, "stop all the wheels")
{
  wheel[0].set_speed(0);
  wheel[1].set_speed(0);
  wheel[2].set_speed(0);
}

void mov(float x, float y, float dtheta) {
  
  if (old_x == 0 && old_y == 0) {
    orient_tgt = imu_gyro_yaw();
    orient_I = 0;
  }
  old_x = x;
  old_y = y;
  old_turn = dtheta;

  y = y * (1 + k_magic);

  float x0 = -cos(M_PI/6);
  float y0 = -sin(M_PI/6);
  
  float x1 = -cos(M_PI/6);
  float y1 = sin(M_PI/6);

  float x2 = 1.0;
  float y2 = 0.0;

  static float fric[3];
  fric[0] = fabs(-y*x0 + x*y0);
  fric[1] = fabs(-y*x1 + x*y1);
  fric[2] = fabs(-y*x2 + x*y2);

  float a1 = x*x0 + y*y0;
  float a2 = x*x1 + y*y1;
  
  float rot_order = k_order_rot * dtheta;
  wheel[2].set_speed( a1 * (1 + k_f * fric[0]) + rot_order);
  wheel[1].set_speed( a2 * (1 + k_f * fric[1]) + rot_order);
  wheel[0].set_speed( (-a1-a2) * (1 + k_f * fric[2]) + rot_order);
}

void update_move_order() {
  /* correction pour orienter la commande */
  float v1 = -k_order * dy/100;
  float v2 = -k_order * dx/100;
  /* update */
  mov(v1,v2, turn);
}

TERMINAL_COMMAND(mv, "move toward a speed vector / with a rotation speed x y (rho)")
{
  if (argc == 2) {
    mov(atof(argv[0]), atof(argv[1]), 0.0);
  }

  if (argc == 3) {
    mov(atof(argv[0]), atof(argv[1]), atof(argv[2]));
  }
}

TERMINAL_COMMAND(dx, "Dx") {
  dx = atof(argv[0]);
  update_move_order();
}

TERMINAL_COMMAND(dy, "Dy") {
  dy = atof(argv[0]);
  update_move_order();
}

TERMINAL_COMMAND(turn, "Turn") {
  turn = atof(argv[0]);
  update_move_order();
}
