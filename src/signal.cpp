#include "signal.hpp"

volatile sig_atomic_t SignalState::serverRunning = 1;

void signalHandler(int sig) {
  if (sig == SIGINT || sig == SIGTERM) {
    SignalState::serverRunning = 0;
  }
}

void setSignals(void) { 
  std::signal(SIGTERM, signalHandler);
  std::signal(SIGINT, signalHandler);
}
