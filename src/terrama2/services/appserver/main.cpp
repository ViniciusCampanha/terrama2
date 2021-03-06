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
  \file terrama2/services/collector/main.cpp

  \brief Collector service main.

  \author Jano Simas
  \author Vinicius Campanha
 */

// TerraMA2
#ifdef TERRAMA2_COLLECTOR_ENABLED
#include <terrama2/services/collector/core/Service.hpp>
#include <terrama2/services/collector/core/DataManager.hpp>
#include <terrama2/services/collector/core/CollectorLogger.hpp>
#endif

#ifdef TERRAMA2_ANALYSIS_CORE_ENABLED
#include <terrama2/services/analysis/core/Service.hpp>
#include <terrama2/services/analysis/core/DataManager.hpp>
#include <terrama2/services/analysis/core/utility/PythonInterpreterInit.hpp>
#endif

#ifdef TERRAMA2_VIEW_ENABLED
#include <terrama2/services/view/core/Service.hpp>
#include <terrama2/services/view/core/DataManager.hpp>
#include <terrama2/services/view/core/ViewLogger.hpp>
#endif

#ifdef TERRAMA2_ALERT_ENABLED
#include <terrama2/services/alert/core/Service.hpp>
#include <terrama2/services/alert/core/DataManager.hpp>
#include <terrama2/services/alert/core/AlertLogger.hpp>
#include <terrama2/services/alert/impl/Utils.hpp>
#endif

#include <terrama2/core/network/TcpManager.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/Logger.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/utility/Service.hpp>
#include <terrama2/impl/Utils.hpp>
#include <terrama2/core/ErrorCodes.hpp>
#include <terrama2/Version.hpp>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

// STL
#include <memory>
#include <iostream>
#include <utility>
#include <tuple>
#include <locale>

// Qt
#include <QCoreApplication>
#include <QtGui/QGuiApplication>
#include <QTimer>

namespace po = boost::program_options;

const std::string analysisType = "analysis";
const std::string collectorType = "collector";
const std::string viewType = "view";
const std::string alertType = "alert";

bool checkServiceType(const std::string& serviceType)
{
#ifdef TERRAMA2_COLLECTOR_ENABLED
  if(serviceType == collectorType)
    return true;
#endif

#ifdef TERRAMA2_ALERT_ENABLED
  if(serviceType == alertType)
    return true;
#endif

#ifdef TERRAMA2_ANALYSIS_CORE_ENABLED
  if(serviceType == analysisType)
    return true;
#endif

#ifdef TERRAMA2_VIEW_ENABLED
  if(serviceType == viewType)
    return true;
#endif

  return false;
}
#ifdef TERRAMA2_COLLECTOR_ENABLED
std::tuple<std::shared_ptr<terrama2::core::DataManager>, std::shared_ptr<terrama2::core::Service>, std::shared_ptr<terrama2::core::ProcessLogger> >
createCollector()
{
  auto dataManager = std::make_shared<terrama2::services::collector::core::DataManager>();
  auto service = std::make_shared<terrama2::services::collector::core::Service>(dataManager);
  auto logger = std::make_shared<terrama2::services::collector::core::CollectorLogger>();

  service->setLogger(logger);

  return std::make_tuple(dataManager, service, logger);
}
#endif

#ifdef TERRAMA2_ANALYSIS_CORE_ENABLED
std::tuple<std::shared_ptr<terrama2::core::DataManager>, std::shared_ptr<terrama2::core::Service>, std::shared_ptr<terrama2::core::ProcessLogger> >
createAnalysis()
{
  auto dataManager = std::make_shared<terrama2::services::analysis::core::DataManager>();
  auto service = std::make_shared<terrama2::services::analysis::core::Service>(dataManager);

  auto logger = std::make_shared<terrama2::services::analysis::core::AnalysisLogger>();

  service->setLogger(logger);

  return std::make_tuple(dataManager, service, logger);
}
#endif

#ifdef TERRAMA2_VIEW_ENABLED
std::tuple<std::shared_ptr<terrama2::core::DataManager>, std::shared_ptr<terrama2::core::Service>, std::shared_ptr<terrama2::core::ProcessLogger> >
createView()
{
  auto dataManager = std::make_shared<terrama2::services::view::core::DataManager>();
  auto service = std::make_shared<terrama2::services::view::core::Service>(dataManager);
  auto logger = std::make_shared<terrama2::services::view::core::ViewLogger>();

  service->setLogger(logger);

  return std::make_tuple(dataManager, service, logger);
}
#endif

#ifdef TERRAMA2_ALERT_ENABLED
std::tuple<std::shared_ptr<terrama2::core::DataManager>, std::shared_ptr<terrama2::core::Service>, std::shared_ptr<terrama2::core::ProcessLogger> >
createAlert()
{
  auto dataManager = std::make_shared<terrama2::services::alert::core::DataManager>();
  auto service = std::make_shared<terrama2::services::alert::core::Service>(dataManager);
  auto logger = std::make_shared<terrama2::services::alert::core::AlertLogger>();

  service->setLogger(logger);

  terrama2::services::alert::core::registerFactories();

  return std::make_tuple(dataManager, service, logger);
}
#endif

std::tuple<std::shared_ptr<terrama2::core::DataManager>, std::shared_ptr<terrama2::core::Service>, std::shared_ptr<terrama2::core::ProcessLogger> >
createService(const std::string& serviceType)
{
#ifdef TERRAMA2_COLLECTOR_ENABLED
  if(serviceType == collectorType)
    return createCollector();
#endif

#ifdef TERRAMA2_ANALYSIS_CORE_ENABLED
  if(serviceType == analysisType)
    return createAnalysis();
#endif

#ifdef TERRAMA2_VIEW_ENABLED
  if(serviceType == viewType)
    return createView();
#endif

#ifdef TERRAMA2_ALERT_ENABLED
  if(serviceType == alertType)
    return createAlert();
#endif

  exit(SERVICE_LOAD_ERROR);
}

int main(int argc, char* argv[])
{
  try
  {

    QGuiApplication app(argc, argv);

    std::string appName = boost::filesystem::basename(argv[0]);

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "show help message.")
        ("version,v", "Show TerraMA2 version.")
        ("service,s", po::value<std::string>()->required(), "Service to be started.\nValid options:\n\t- ANALYSIS\n\t- COLLECTOR\n\t- VIEW")
        ("port,p", po::value<int>()->required(), "Port the service will listen.")
    ;
    po::positional_options_description positionalOptions;
        positionalOptions.add("service", 1);
        positionalOptions.add("port", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc)
                      .positional(positionalOptions).run(),
                    vm);

    if (vm.count("help")) {
        std::cout << "usage: "+appName+" [--version] [--help] service port" << "\n";
        std::cout << desc << std::endl;
        return 0;
    }

    if (vm.count("version")) {
        std::cout << "TerraMA2 " << TERRAMA2_VERSION_STRING << std::endl;
        return 0;
    }

    po::notify(vm);

    std::string serviceType(vm["service"].as<std::string>());
    std::transform(serviceType.begin(), serviceType.end(), serviceType.begin(), ::tolower);

    if(!checkServiceType(serviceType))
      return UNKNOWN_SERVICE_TYPE;

    int listeningPort = vm["port"].as<int>();

    terrama2::core::TerraMA2Init terramaRaii(serviceType, listeningPort);
    terrama2::core::registerFactories();


    auto& serviceManager = terrama2::core::ServiceManager::getInstance();
    serviceManager.setServiceType(serviceType);
    serviceManager.setListeningPort(listeningPort);

    // service context
    // this is needed for calling the destructor of the service before finalizing terralib
    {
      TERRAMA2_LOG_INFO() << QObject::tr("Initializing TerraMA2 service...");
      TERRAMA2_LOG_INFO() << QObject::tr("Starting %1 service.").arg(QString::fromStdString(serviceType));

#ifdef TERRAMA2_ANALYSIS_CORE_ENABLED
      // Must initialize the python interpreter before creating any thread.
      terrama2::services::analysis::core::PythonInterpreterInit pythonInterpreterInit;
#endif

      // Changes in the initialization order may cause locale problems
      std::locale::global(std::locale::classic());

      std::shared_ptr<terrama2::core::DataManager> dataManager;
      std::shared_ptr<terrama2::core::Service> service;
      std::shared_ptr<terrama2::core::ProcessLogger> logger;
      std::tie(dataManager, service, logger) = createService(serviceType);
      if(!service.get()
          || !dataManager.get()
          || !logger.get())
        return SERVICE_LOAD_ERROR;

      auto tcpManager = std::make_shared<terrama2::core::TcpManager>(dataManager, logger);
      if(!tcpManager->listen(QHostAddress::Any, serviceManager.listeningPort()))
      {
        TERRAMA2_LOG_ERROR() <<  QObject::tr("Unable to listen to port: %1").arg(serviceManager.listeningPort());

        exit(TCP_SERVER_ERROR);
      }

      QObject::connect(&serviceManager, &terrama2::core::ServiceManager::listeningPortUpdated, tcpManager.get(), &terrama2::core::TcpManager::updateListeningPort);

      QObject::connect(tcpManager.get(), &terrama2::core::TcpManager::startProcess, service.get(), &terrama2::core::Service::addToQueue);
      QObject::connect(&serviceManager, &terrama2::core::ServiceManager::numberOfThreadsUpdated, service.get(), &terrama2::core::Service::updateNumberOfThreads);

      QObject::connect(&serviceManager, &terrama2::core::ServiceManager::logConnectionInfoUpdated, logger.get(), &terrama2::core::ProcessLogger::setConnectionInfo);

      QObject::connect(service.get(), &terrama2::core::Service::processFinishedSignal, tcpManager.get(),
                       &terrama2::core::TcpManager::sendProcessFinishedSlot);

      QObject::connect(service.get(), &terrama2::core::Service::validateProcessSignal, tcpManager.get(),
                       &terrama2::core::TcpManager::sendValidateProcessSlot);

      QObject::connect(tcpManager.get(), &terrama2::core::TcpManager::stopSignal, service.get(), &terrama2::core::Service::stopService);
      QObject::connect(service.get(), &terrama2::core::Service::serviceFinishedSignal, &app, &QCoreApplication::quit);

      app.exec();
    }

    try
    {
      //Service closed by load error
      if(!serviceManager.serviceLoaded())
        return SERVICE_LOAD_ERROR;
    }
    catch(...)
    {
      return TERRAMA2_FINALIZATION_ERROR;
    }
  }
  catch(const boost::program_options::error& e)
  {
    std::cout << "Invalid options.\nSee 'terrama2_service --help'\n" << std::endl;
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(const boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::diagnostic_information(e);
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
//  catch(...)
//  {
//    TERRAMA2_LOG_ERROR() << QObject::tr("\n\nUnknown Exception...\n");
//  }

  return 0;
}
