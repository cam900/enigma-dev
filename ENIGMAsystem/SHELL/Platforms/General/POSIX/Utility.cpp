#include "Platforms/General/PFmain.h"

#include <time.h> //CLOCK_MONOTONIC
#include <sys/types.h>     //getpid
#include <unistd.h>        //usleep

namespace enigma {

static struct timespec time_offset;
static struct timespec time_offset_slowing;
static struct timespec time_current;

void Sleep(int ms) {
  if (ms >= 1000) enigma_user::sleep(ms / 1000);
  if (ms > 0) usleep(ms % 1000 * 1000);
}

void windowsystem_write_exename(char* x) {
  unsigned irx = 0;
  if (enigma::parameterc)
    for (irx = 0; enigma::parameters[0][irx] != 0; irx++) x[irx] = enigma::parameters[0][irx];
  x[irx] = 0;
}

void initTimer() {
  clock_gettime(CLOCK_MONOTONIC, &time_offset);
  time_offset_slowing.tv_sec = time_offset.tv_sec;
  time_offset_slowing.tv_nsec = time_offset.tv_nsec;
}

int updateTimer() {
  clock_gettime(CLOCK_MONOTONIC, &time_current);
  {
    long passed_mcs = (time_current.tv_sec - time_offset.tv_sec) * 1000000 +
                      (time_current.tv_nsec / 1000 - +time_offset.tv_nsec / 1000);
    passed_mcs = clamp(passed_mcs, 0, 1000000);
    if (passed_mcs >= 1000000) {  // Handle resetting.

      enigma_user::fps = frames_count;
      frames_count = 0;
      time_offset.tv_sec += passed_mcs / 1000000;
      time_offset_slowing.tv_sec = time_offset.tv_sec;
      time_offset_slowing.tv_nsec = time_offset.tv_nsec;
    }
  }

  long spent_mcs = 0;
  long last_mcs = 0;
  if (current_room_speed > 0) {
    spent_mcs = (time_current.tv_sec - time_offset_slowing.tv_sec) * 1000000 +
                (time_current.tv_nsec / 1000 - time_offset_slowing.tv_nsec / 1000);
    spent_mcs = clamp(spent_mcs, 0, 1000000);
    long remaining_mcs = 1000000 - spent_mcs;
    long needed_mcs = long((1.0 - 1.0 * frames_count / current_room_speed) * 1e6);
    const int catchup_limit_ms = 50;
    if (needed_mcs > remaining_mcs + catchup_limit_ms * 1000) {
      // If more than catchup_limit ms is needed than is remaining, we risk running too fast to catch up.
      // In order to avoid running too fast, we advance the offset, such that we are only at most catchup_limit ms behind.
      // Thus, if the load is consistently making the game slow, the game is still allowed to run as fast as possible
      // without any sleep.
      // And if there is very heavy load once in a while, the game will only run too fast for catchup_limit ms.
      time_offset_slowing.tv_nsec += 1000 * (needed_mcs - (remaining_mcs + catchup_limit_ms * 1000));
      spent_mcs = (time_current.tv_sec - time_offset_slowing.tv_sec) * 1000000 +
                  (time_current.tv_nsec / 1000 - time_offset_slowing.tv_nsec / 1000);
      spent_mcs = clamp(spent_mcs, 0, 1000000);
      remaining_mcs = 1000000 - spent_mcs;
      needed_mcs = long((1.0 - 1.0 * frames_count / current_room_speed) * 1e6);
    }
    if (remaining_mcs > needed_mcs) {
      const long sleeping_time = std::min((remaining_mcs - needed_mcs) / 5, long(999999));
      usleep(std::max(long(1), sleeping_time));
      return -1;
    }
  }

  //TODO: The placement of this code is inconsistent with Win32 because events are handled after, ask Josh.
  unsigned long dt = 0;
  if (spent_mcs > last_mcs) {
    dt = (spent_mcs - last_mcs);
  } else {
    //TODO: figure out what to do here this happens when the fps is reached and the timers start over
    dt = enigma_user::delta_time;
  }
  last_mcs = spent_mcs;
  enigma_user::delta_time = dt;
  current_time_mcs += enigma_user::delta_time;
  enigma_user::current_time += enigma_user::delta_time / 1000;

  return 0;
}

void set_working_directory() {
  char buffer[1024];
  char* err = getcwd(buffer, sizeof(buffer));
#ifdef DEBUG_MODE
  if (err != NULL)
    fprintf(stdout, "Current working dir: %s\n", buffer);
  else
    perror("getcwd() error");
#endif
  enigma_user::working_directory = std::string(buffer);
}

}  //namespace enigma

namespace enigma_user {

void execute_shell(std::string operation, std::string fname, std::string args) {
  if (system(NULL)) {
    system((fname + args + " &").c_str());
  } else {
    printf("execute_shell cannot be used as there is no command processor!");
    return;
  }
}

void execute_shell(std::string fname, std::string args) { execute_shell("", fname, args); }

void execute_program(std::string operation, std::string fname, std::string args, bool wait) {
  if (system(NULL)) {
    system((fname + args + (wait ? " &" : "")).c_str());
  } else {
    printf("execute_program cannot be used as there is no command processor!");
    return;
  }
}

void execute_program(std::string fname, std::string args, bool wait) { execute_program("", fname, args, wait); }

void url_open(std::string url, std::string target, std::string options) {
  execute_program("xdg-open", url, false);
}

void url_open_ext(std::string url, std::string target) { url_open(url, target); }

void url_open_full(std::string url, std::string target, std::string options) { url_open(url, target, options); }

void action_webpage(const std::string& url) { url_open(url); }

std::string environment_get_variable(std::string name) {
  char* ev = getenv(name.c_str());
  return ev ? ev : "";
}

}  //namespace enigma_user