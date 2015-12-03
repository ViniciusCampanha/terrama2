/*
  Copyright (C) 2007 National Institute For Space Research (INPE) - Brazil.

  This file is part of TerraMA2 - a free and open source computational
  platform for analysis, monitoring, and alert of geo-environmental extremes.

  TerraMA2 is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License,
  or (at your option) any later version.

  TerraMA2 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with TerraMA2. See LICENSE. If not, write to
  TerraMA2 Team at <terrama2-team@dpi.inpe.br>.
*/

/*!
  \file terrama2/collector/TsFilter.hpp

  \brief Tests for the TestFilter class.

  \author Jano Simas
*/

#include "TsDataFilter.hpp"

//terrama2
#include <terrama2/collector/DataFilter.hpp>

#include <terrama2/core/DataProvider.hpp>
#include <terrama2/core/DataSetItem.hpp>
#include <terrama2/core/DataSet.hpp>
#include <terrama2/core/Filter.hpp>

//QT
#include <QStringList>

//terralib
#include <terralib/datatype/TimeInstant.h>
#include <terralib/datatype/Date.h>

//STL
#include <utility>

void TsDataFilter::TestFilterNamesExact()
{
  terrama2::core::DataProvider provider;
  terrama2::core::DataSet      dataset;
  terrama2::core::DataSetItem  dataItem;
  terrama2::core::Filter       filter;
  provider.add(dataset);
  dataset.add(dataItem);
  dataItem.setFilter(filter);

  std::string exact("exact");
  dataItem.setMask(exact);

  terrama2::collector::DataFilter datafilter(dataItem);

  std::vector<std::string> names {"teste1", "teste2 ", "exc", "exact", "exact "};

  names = datafilter.filterNames(names);

  QVERIFY(names.size() == 1);
  QCOMPARE(names.at(0), exact);
}

void TsDataFilter::TestEmptyMask()
{
  terrama2::core::DataProvider provider;
  terrama2::core::DataSet      dataset;
  terrama2::core::DataSetItem  dataItem;
  terrama2::core::Filter       filter;
  provider.add(dataset);
  dataset.add(dataItem);
  dataItem.setFilter(filter);

  terrama2::collector::DataFilter datafilter(dataItem);

  std::vector<std::string> names {"teste1", "teste2 ", "exc", "exact", "exact "};

  std::vector<std::string> output = datafilter.filterNames(names);

  QVERIFY(output.size() == 0);
}

void TsDataFilter::TestDateMask()
{
  terrama2::core::DataProvider provider;
  terrama2::core::DataSet      dataset;
  terrama2::core::DataSetItem  dataItem;
  terrama2::core::Filter       filter;
  provider.add(dataset);
  dataset.add(dataItem);
  dataItem.setFilter(filter);

  std::string exact("name_%d/%M/%A");
  dataItem.setMask(exact);

  terrama2::collector::DataFilter datafilter(dataItem);

  std::vector<std::string> names {"name_12/03/2015", "name 12/03/2015 ", "name_12/3/2015", "name_12-03-2015", "names_2A/03/2015 "};

  names = datafilter.filterNames(names);

  QVERIFY(names.size() == 1);
}

void TsDataFilter::TestDiscardBeforeMask()
{
  terrama2::core::DataProvider provider;
  terrama2::core::DataSet      dataset;
  terrama2::core::DataSetItem  dataItem;
  terrama2::core::Filter       filter;
  std::unique_ptr<te::dt::TimeInstant> tDate(new te::dt::TimeInstant("20150615T000000"));
  filter.setDiscardBefore(std::move(tDate));

  provider.add(dataset);
  dataset.add(dataItem);
  dataItem.setFilter(filter);

  std::string exact("name_%d/%M/%A");
  dataItem.setMask(exact);

  terrama2::collector::DataFilter datafilter(dataItem);

  std::vector<std::string> names {"name_12/03/2015", "name_12/07/2014", "name_12/06/2015", "name_16/06/2015", "name_12-11-2015", "name_12/18/2015", };

  names = datafilter.filterNames(names);

  QVERIFY(names.size() == 1);
}

















