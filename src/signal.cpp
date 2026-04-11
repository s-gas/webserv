#include "signal.hpp"
#include <cstring>
#include <signal.h>

volatile std::sig_atomic_t SignalState::serverRunning = 1;

void signalHandler(int sig) {
  if (sig == SIGINT || sig == SIGTERM) {
    SignalState::serverRunning = 0;
  }
}

void setSignals(void) {
  struct sigaction sigInterupt;

  std::memset(&sigInterupt, 0, sizeof(sigInterupt));
  sigInterupt.sa_handler = signalHandler;
  sigemptyset(&sigInterupt.sa_mask);
  sigInterupt.sa_flags = 0;

  sigaction(SIGINT, &sigInterupt, NULL);
  sigaction(SIGTERM, &sigInterupt, NULL);
}
