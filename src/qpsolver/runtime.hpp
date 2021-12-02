#ifndef __SRC_LIB_RUNTIME_HPP__
#define __SRC_LIB_RUNTIME_HPP__

#include "eventhandler.hpp"
#include "instance.hpp"
#include "settings.hpp"
#include "statistics.hpp"
#include "HighsTimer.h"

enum class ProblemStatus { INDETERMINED, OPTIMAL, UNBOUNDED, INFEASIBLE };

struct Runtime {
  Instance instance;
  Settings settings;
  Statistics statistics;

  HighsTimer& timer;

  Eventhandler<Runtime&> endofiterationevent;

  Vector primal;
  Vector rowactivity;
  Vector dualvar;
  Vector dualcon;
  ProblemStatus status = ProblemStatus::INDETERMINED;

  Runtime(Instance& inst, HighsTimer& ht)
      : instance(inst),
        timer(ht),
        primal(Vector(instance.num_var)),
        rowactivity(Vector(instance.num_con)),
        dualvar(instance.num_var),
        dualcon(instance.num_con) {}
};

#endif
