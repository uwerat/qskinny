#include "QskTickmarksMetrics.h"

constexpr bool test_ctor0()
{
	return QskTickmarksMetrics{} == QskTickmarksMetrics{0,0,0};
}

static_assert(test_ctor0(), "test failed!");

constexpr bool test_ctor1()
{
	QskTickmarksMetrics m;
	return QskTickmarksMetrics{1,1,1} == QskTickmarksMetrics{2,2,2};
}

static_assert(test_ctor1(), "test failed!");