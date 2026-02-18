#include "interval.h"

const float Interval::infinity = Math::INF;
const Interval Interval::empty = Interval(+Interval::infinity, -Interval::infinity);
const Interval Interval::universe = Interval(-Interval::infinity, Interval::infinity);
