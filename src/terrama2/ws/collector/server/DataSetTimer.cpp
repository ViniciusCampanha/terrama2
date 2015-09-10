
#include "DataSetTimer.hpp"
#include "DataProcessor.hpp"
#include "CollectorFactory.hpp"

#include "../../../core/DataSet.hpp"
#include "../../../core/DataProvider.hpp"

terrama2::ws::collector::server::DataSetTimer::DataSetTimer(terrama2::core::DataSetPtr dataSet)
  : dataSet_(dataSet)
{
  connect(&timer_, SIGNAL(timeout()), this, SLOT(timeoutSlot()), Qt::UniqueConnection);

  populateDataLst();
  prepareTimer();
}

void terrama2::ws::collector::server::DataSetTimer::timeoutSlot() const
{
  emit timerSignal(dataSet_->id());
}

void terrama2::ws::collector::server::DataSetTimer::prepareTimer()
{
  //JANO: implementar prepareTimer
  timer_.start(10000);
}

void terrama2::ws::collector::server::DataSetTimer::populateDataLst()
{
//JANO: implementar populateDataLst
}


terrama2::ws::collector::server::CollectorPtr terrama2::ws::collector::server::DataSetTimer::getCollector() const
{
  auto dataProvider = getDataSet()->getDataProvider();
  return CollectorFactory::instance().getCollector(dataProvider);
}
