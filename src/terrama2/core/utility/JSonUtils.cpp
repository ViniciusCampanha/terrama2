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
  \file terrama2/core/utility/JSonUtils.hpp

  \brief

  \author Jano Simas
*/
#include "../data-model/DataProvider.hpp"
#include "../data-model/DataSeries.hpp"
#include "../data-model/DataSeriesSemantics.hpp"
#include "../data-model/DataSet.hpp"
#include "../data-model/DataSetDcp.hpp"
#include "../data-model/DataSetGrid.hpp"
#include "../data-model/DataSetOccurrence.hpp"
#include "../utility/SemanticsManager.hpp"
#include "../utility/TimeUtils.hpp"
#include "../utility/GeoUtils.hpp"
#include "../utility/Utils.hpp"
#include "../utility/Logger.hpp"
#include "../utility/Verify.hpp"
#include "../Exception.hpp"

#include "JSonUtils.hpp"

//Qt
#include <QJsonDocument>
#include <QJsonArray>
#include <QObject>

terrama2::core::DataProviderPtr terrama2::core::fromDataProviderJson(QJsonObject json)
{
  if(json["class"].toString() != "DataProvider")
  {
    QString errMsg = QObject::tr("Invalid DataProvider JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  if(!(json.contains("id")
       && json.contains("project_id")
       && json.contains("name")
       && json.contains("description")
       && json.contains("intent")
       && json.contains("uri")
       && json.contains("active")
       && json.contains("data_provider_type")))
  {
    QString errMsg = QObject::tr("Invalid DataProvider JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  terrama2::core::DataProvider* provider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr providerPtr(provider);

  provider->id = json["id"].toInt();
  provider->projectId = json["project_id"].toInt();
  provider->name = json["name"].toString().toStdString();
  provider->description = json["description"].toString().toStdString();
  provider->intent = static_cast<terrama2::core::DataProviderIntent>(json["intent"].toInt());
  provider->uri = json["uri"].toString().toStdString();
  provider->active = json["active"].toBool();
  provider->dataProviderType = json["data_provider_type"].toString().toStdString();

  if(json.contains("options"))
  {
    auto obj = json["options"].toObject();
    for(auto it = obj.begin(); it != obj.end(); ++it)
    {
      provider->options.emplace(it.key().toStdString(), it.value().toString().toStdString());
    }
  }

  try
  {
    auto timeout = provider->options.at("timeout");
    provider->timeout = std::stoi(timeout);
  }
  catch (...)
  {
    provider->timeout = 8;
  }

  return providerPtr;
}

terrama2::core::DataSeriesPtr terrama2::core::fromDataSeriesJson(QJsonObject json)
{
  if(json["class"].toString() != "DataSeries")
  {
    QString errMsg = QObject::tr("Invalid DataSeries JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  if(!(json.contains("id")
       && json.contains("data_provider_id")
       && json.contains("semantics")
       && json.contains("name")
       && json.contains("active")
       && json.contains("description")))
  {
    QString errMsg = QObject::tr("Invalid DataSeries JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);

  dataSeries->id = json["id"].toInt();
  dataSeries->dataProviderId = json["data_provider_id"].toInt();
  dataSeries->semantics = SemanticsManager::getInstance().getSemantics(json["semantics"].toString().toStdString());
  dataSeries->name = json["name"].toString().toStdString();
  dataSeries->description = json["description"].toString().toStdString();
  dataSeries->active = json["active"].toBool();

  QJsonArray dataSetArray = json["datasets"].toArray();

  std::function<terrama2::core::DataSetPtr(QJsonObject)> createDataSet = nullptr;
  switch(dataSeries->semantics.dataSeriesType)
  {
    case DataSeriesType::DCP:
      createDataSet = fromDataSetDcpJson;
      break;
    case DataSeriesType::OCCURRENCE:
      createDataSet = fromDataSetOccurrenceJson;
      break;
    case DataSeriesType::GRID:
      createDataSet = fromDataSetGridJson;
      break;
    case DataSeriesType::GEOMETRIC_OBJECT:
    case DataSeriesType::ANALYSIS_MONITORED_OBJECT:
      createDataSet = fromDataSetJson;
      break;
    default:
    {
      QString errMsg = QObject::tr("Invalid DataSeries JSON object.\nUnknown DataSet type.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
    }
  }

  for(auto json : dataSetArray)
  {
    if(json.isObject())
      dataSeries->datasetList.push_back(createDataSet(json.toObject()));
    else
      throw terrama2::core::JSonParserException() << ErrorDescription(QObject::tr("Invalid DataSet JSON object."));
  }

  return dataSeriesPtr;
}

void terrama2::core::addBaseDataSetData(QJsonObject json, terrama2::core::DataSet* dataSet)
{
  if(json["class"].toString() != "DataSet")
  {
    QString errMsg = QObject::tr("Invalid DataSet JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  if(!(json.contains("id")
       && json.contains("data_series_id")
       && json.contains("active")
       && json.contains("format")))
  {
    QString errMsg = QObject::tr("Invalid DataSet JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  dataSet->id = json["id"].toInt();
  dataSet->dataSeriesId = json["data_series_id"].toInt();
  dataSet->active = json["active"].toBool();

  auto formatObj= json["format"].toObject();
  for(auto it = formatObj.begin(); it != formatObj.end(); ++it)
  {
    dataSet->format.emplace(it.key().toStdString(), it.value().toString().toStdString());
  }

}

terrama2::core::DataSetPtr terrama2::core::fromDataSetDcpJson(QJsonObject json)
{
  terrama2::core::DataSetDcp* dataSet = new terrama2::core::DataSetDcp();
  terrama2::core::DataSetDcpPtr dataSetPtr(dataSet);

  addBaseDataSetData(json, dataSet);

  if(!json.contains("position"))
  {
    QString errMsg = QObject::tr("Invalid DataSet JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  std::string ewkt = json["position"].toString().toStdString();
  auto geom = ewktToGeom(ewkt);
  auto point = std::dynamic_pointer_cast<te::gm::Point>(geom);

  if(!point.get())
  {
    QString errMsg = QObject::tr("Invalid DataSet JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  dataSet->position = point;

  return dataSetPtr;
}


terrama2::core::DataSetPtr terrama2::core::fromDataSetJson(QJsonObject json)
{
  terrama2::core::DataSet* dataSet = new terrama2::core::DataSet();
  terrama2::core::DataSetPtr dataSetPtr(dataSet);

  addBaseDataSetData(json, dataSet);

  return dataSetPtr;
}

terrama2::core::DataSetPtr terrama2::core::fromDataSetOccurrenceJson(QJsonObject json)
{
  terrama2::core::DataSetOccurrence* dataSet = new terrama2::core::DataSetOccurrence();
  terrama2::core::DataSetOccurrencePtr dataSetPtr(dataSet);

  addBaseDataSetData(json, dataSet);

  return dataSetPtr;
}

terrama2::core::DataSetPtr terrama2::core::fromDataSetGridJson(QJsonObject json)
{
  terrama2::core::DataSetGrid* dataSet = new terrama2::core::DataSetGrid();
  terrama2::core::DataSetGridPtr dataSetPtr(dataSet);

  addBaseDataSetData(json, dataSet);

  return dataSetPtr;
}

terrama2::core::Filter terrama2::core::fromFilterJson(QJsonObject json, DataManager* dataManager)
{
  if(json.empty())
    return terrama2::core::Filter();

  if(json["class"].toString() != "Filter")
  {
    QString errMsg = QObject::tr("Invalid Filter JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  terrama2::core::Filter filter;
  if(json.contains("discard_before") && !json.value("discard_before").isNull())
  {
    std::string dateTime = json.value("discard_before").toString().toStdString();
    filter.discardBefore = TimeUtils::stringToTimestamp(dateTime, terrama2::core::TimeUtils::webgui_timefacet);
    verify::date(filter.discardBefore);
  }
  if(json.contains("discard_after") && !json.value("discard_after").isNull())
  {
    std::string dateTime = json["discard_after"].toString().toStdString();
    filter.discardAfter = TimeUtils::stringToTimestamp(dateTime, terrama2::core::TimeUtils::webgui_timefacet);
    verify::date(filter.discardAfter);
  }

  if(json.contains("region") && !json.value("region").isNull())
  {
    auto ewkt = json["region"].toString().toStdString();
    filter.region = ewktToGeom(ewkt);

    verify::srid(filter.region->getSRID());

    if (json.contains("crop_raster"))
      filter.cropRaster = json["crop_raster"].toBool();
    else
      filter.cropRaster = false;
  }

  if(json.contains("by_value") && !json.value("by_value").isNull())
  {
    filter.byValue = json["by_value"].toString().toStdString();
  }

  if(json.contains("last_values") && !json.value("last_values").isNull())
  {
    filter.lastValues = std::make_shared<size_t>(json["last_values"].toInt());
  }

  if(json.contains("data_series_id") && !json.value("data_series_id").isNull())
  {
    DataSeriesId dataSeriesId = json["data_series_id"].toInt();

    // Sets the data series for a static data filter
    if(dataSeriesId != 0)
    {
      auto dataSeries = dataManager->findDataSeries(dataSeriesId);
      filter.dataSeries = dataSeries;

      auto dataProvider = dataManager->findDataProvider(dataSeries->dataProviderId);
      filter.dataProvider = dataProvider;
    }
  }

  return filter;
}

terrama2::core::Risk terrama2::core::fromRiskJson(QJsonObject json)
{
  if(json["class"].toString() != "Risk")
  {
    QString errMsg = QObject::tr("Invalid Risk JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  if(!(json.contains("name")
//       && json.contains("description")
       && json.contains("levels")))
  {
    QString errMsg = QObject::tr("Invalid Risk JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  terrama2::core::Risk risk;
  risk.name = json["name"].toString().toStdString();
//  risk.description = json["description"].toString().toStdString();

  auto riskLevelsArray = json["levels"].toArray();
  for(const auto& value : riskLevelsArray)
  {
    auto obj = value.toObject();
    terrama2::core::RiskLevel riskLevel;
    riskLevel.name = obj["name"].toString().toStdString();
    riskLevel.level = static_cast<uint32_t>(obj["level"].toInt());
    riskLevel.value = obj["value"].toDouble();

    risk.riskLevels.push_back(riskLevel);
  }
  std::sort(std::begin(risk.riskLevels), std::end(risk.riskLevels));

  return risk;
}

QJsonObject terrama2::core::toJson(const terrama2::core::Risk& risk)
{
  QJsonObject obj;
  obj.insert("class", QString("Risk"));
  obj.insert("name", QString::fromStdString(risk.name));
  obj.insert("description", QString::fromStdString(risk.description));

  QJsonArray riskArray;
  for(const auto& riskLevel : risk.riskLevels)
  {
    QJsonObject tempoObj;
    tempoObj.insert("name", QString::fromStdString(riskLevel.name));
    tempoObj.insert("level", static_cast<int>(riskLevel.level));
    tempoObj.insert("value", riskLevel.value);

    riskArray.append(tempoObj);
  }
  obj.insert("levels", riskArray);

  return obj;
}

QJsonObject terrama2::core::toJson(const terrama2::core::Filter& filter)
{
  QJsonObject obj;
  obj.insert("class", QString("Filter"));
  if(filter.discardBefore.get())
  {
    std::string discardBefore = TimeUtils::boostLocalTimeToString(filter.discardBefore->getTimeInstantTZ(), TimeUtils::webgui_timefacet);
    obj.insert("discard_before", QString::fromStdString(discardBefore));
  }
  if(filter.discardAfter.get())
  {
    std::string discardAfter = TimeUtils::boostLocalTimeToString(filter.discardAfter->getTimeInstantTZ(), TimeUtils::webgui_timefacet);
    obj.insert("discard_after", QString::fromStdString(discardAfter));
  }

  if(filter.region.get())
  {
    std::string region = filter.region->toString();
    obj.insert("region", QString::fromStdString(region));
  }

  obj.insert("last_values", static_cast<qint32>(*filter.lastValues.get()));

  if(filter.dataSeries)
    obj.insert("data_series_id", static_cast<int32_t>(filter.dataSeries->id));

  obj.insert("by_value", QString::fromStdString(filter.byValue));

  return obj;
}

terrama2::core::Schedule terrama2::core::fromScheduleJson(QJsonObject json)
{
  if(json.empty())
    return terrama2::core::Schedule();

  if(json["class"].toString() != "Schedule")
  {
    QString errMsg = QObject::tr("Invalid Schedule JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  if(!(json.contains("id")
       && json.contains("frequency")
       && json.contains("frequency_unit")
       && json.contains("frequency_start_time")
       && json.contains("schedule")
       && json.contains("schedule_time")
       && json.contains("schedule_unit")
       && json.contains("schedule_retry")
       && json.contains("schedule_retry_unit")
       && json.contains("schedule_timeout")
       && json.contains("schedule_timeout_unit")))
  {
    QString errMsg = QObject::tr("Invalid Schedule JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  terrama2::core::Schedule schedule;
  schedule.id = json["id"].toInt();
  schedule.frequency = json["frequency"].toInt();
  schedule.frequencyUnit = json["frequency_unit"].toString().toStdString();
  schedule.frequencyStartTime = json["frequency_start_time"].toString().toStdString();
  schedule.schedule = json["schedule"].toInt();
  schedule.scheduleTime = json["schedule_time"].toString().toStdString();
  schedule.scheduleUnit = json["schedule_unit"].toString().toStdString();
  schedule.scheduleRetry = json["schedule_retry"].toInt();
  schedule.scheduleRetryUnit = json["schedule_retry_unit"].toString().toStdString();
  schedule.scheduleTimeout = json["schedule_timeout"].toInt();
  schedule.scheduleTimeoutUnit = json["schedule_timeout_unit"].toString().toStdString();

  return schedule;
}

QJsonObject terrama2::core::toJson(DataProviderPtr dataProviderPtr)
{
  QJsonObject obj;
  obj.insert("class", QString("DataProvider"));
  obj.insert("id", static_cast<int32_t>(dataProviderPtr->id));
  obj.insert("project_id", static_cast<int32_t>(dataProviderPtr->projectId));
  obj.insert("name", QString::fromStdString(dataProviderPtr->name));
  obj.insert("description", QString::fromStdString(dataProviderPtr->description));
  obj.insert("intent", static_cast<int>(dataProviderPtr->intent));
  obj.insert("uri", QString::fromStdString(dataProviderPtr->uri));
  obj.insert("active", dataProviderPtr->active);
  obj.insert("timeout", static_cast<int>(dataProviderPtr->timeout));
  obj.insert("data_provider_type", QString::fromStdString(dataProviderPtr->dataProviderType));

  return obj;
}

QJsonObject terrama2::core::toJson(DataSeriesPtr dataSeriesPtr)
{
  QJsonObject obj;
  obj.insert("class", QString("DataSeries"));
  obj.insert("id", static_cast<int32_t>(dataSeriesPtr->id));
  obj.insert("data_provider_id", static_cast<int32_t>(dataSeriesPtr->dataProviderId));
  obj.insert("semantics", QString::fromStdString(dataSeriesPtr->semantics.code));
  obj.insert("name", QString::fromStdString(dataSeriesPtr->name));
  obj.insert("description", QString::fromStdString(dataSeriesPtr->description));

  QJsonArray array;
  for(const auto& dataSet : dataSeriesPtr->datasetList)
    array.push_back(terrama2::core::toJson(dataSet, dataSeriesPtr->semantics));
  obj.insert("datasets", array);

  return obj;
}

QJsonObject terrama2::core::toJson(DataSetPtr dataSetPtr, DataSeriesSemantics semantics)
{
  QJsonObject obj;
  obj.insert("class", QString("DataSet"));
  obj.insert("id", static_cast<int32_t>(dataSetPtr->id));
  obj.insert("data_series_id", static_cast<int32_t>(dataSetPtr->dataSeriesId));
  obj.insert("data_series_id", static_cast<int32_t>(dataSetPtr->dataSeriesId));
  obj.insert("active", dataSetPtr->active);
  QJsonObject format;
  for(const auto & it : dataSetPtr->format)
  {
    format.insert(QString::fromStdString(it.first), QString::fromStdString(it.second));
  }
  obj.insert("format", format);

  switch(semantics.dataSeriesType)
  {
    case terrama2::core::DataSeriesType::DCP :
    {
      auto dataSet = std::dynamic_pointer_cast<const DataSetDcp>(dataSetPtr);
      terrama2::core::addToJson(obj, dataSet);
      break;
    }
    case terrama2::core::DataSeriesType::OCCURRENCE :
    {
      auto dataSet = std::dynamic_pointer_cast<const DataSetOccurrence>(dataSetPtr);
      terrama2::core::addToJson(obj, dataSet);
      break;
    }
    case terrama2::core::DataSeriesType::GRID :
    {
      auto dataSet = std::dynamic_pointer_cast<const DataSetGrid>(dataSetPtr);
      terrama2::core::addToJson(obj, dataSet);
      break;
    }
    default:
      /* code */
      break;
  }

  return obj;
}

void terrama2::core::addToJson(QJsonObject& obj, DataSetDcpPtr dataSetPtr)
{
  obj.insert("position", QString::fromStdString(dataSetPtr->position->toString()));
}

void terrama2::core::addToJson(QJsonObject& /*obj*/, DataSetOccurrencePtr /*dataSetPtr*/)
{

}
void terrama2::core::addToJson(QJsonObject& /*obj*/, DataSetGridPtr /*dataSetPtr*/)
{

}

QJsonObject terrama2::core::toJson(Schedule schedule)
{
  QJsonObject obj;
  obj.insert("class", QString("Schedule"));
  obj.insert("id", static_cast<int32_t>(schedule.id));
  obj.insert("frequency",static_cast<int32_t>(schedule.frequency));
  obj.insert("frequency_unit", QString::fromStdString(schedule.frequencyUnit));
  obj.insert("frequency_start_time", QString::fromStdString(schedule.frequencyStartTime));
  obj.insert("schedule",static_cast<int32_t>(schedule.schedule));
  obj.insert("schedule_time",QString::fromStdString(schedule.scheduleTime));
  obj.insert("schedule_unit",QString::fromStdString(schedule.scheduleUnit));
  obj.insert("schedule_retry",static_cast<int32_t>(schedule.scheduleRetry));
  obj.insert("schedule_retry_unit", QString::fromStdString(schedule.scheduleRetryUnit));
  obj.insert("schedule_timeout",static_cast<int32_t>(schedule.scheduleTimeout));
  obj.insert("schedule_timeout_unit", QString::fromStdString(schedule.scheduleTimeoutUnit));

  return obj;
}
