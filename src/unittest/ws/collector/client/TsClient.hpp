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
  \file terrama2/unittest/ws/client/TsClient.hpp

  \brief Tests for the WebService Client class.

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_UNITTEST_WS_COLLECTOR_CLIENT_TSCLIENT__
#define __TERRAMA2_UNITTEST_WS_COLLECTOR_CLIENT_TSCLIENT__

#include "WebProxyAdapterMock.hpp"

//TerraMA2

#include <terrama2/ws/collector/client/Client.hpp>

// Qt
#include <QtTest>

class Client;

class TsClient: public QObject
{
  Q_OBJECT

  public:
    TsClient();

  private:

    WebProxyAdapterMock mock_;
    terrama2::ws::collector::client::Client wsClient_;


    terrama2::core::DataProvider buildDataProvider();
    DataProvider buildDataProviderStruct();
    terrama2::core::DataSet buildDataSet();
    DataSet buildDataSetStruct();


  private slots:


    //******Test functions********;


    /*!
     * \brief Test to add a normal DataProvider
     */
    void testAddDataProvider();


   /*!
    * \brief Test to add a null DataProvider
    */
   void testAddNullDataProvider();


   /*!
    * \brief
    */
   void testRemoveDataProvider();

   /*!
    * \brief
    */
   void testRemoveDataProviderInvalidId();


   /*!
    * \brief
    */
   void testUpdateDataProvider();


   /*!
    * \brief
    */
   void testFindDataProvider();


   /*!
    * \brief
    */
   void testFindDataProviderInvalidID();


   /*!
    * \brief Test to add a normal DataSet
    */
   void testAddDataSet();

   /*!
    * \brief Test to add a null DataProvider
    */
   void testAddNullDataSet();


   /*!
    * \brief Test to add a DataProvider with an id
    */
   void testAddDataSetWithID();


   /*!
    * \brief Test to add a DataProvider with an id
    */
   void testAddDataSetWithWrongDataProviderID();


   /*!
    * \brief
    */
   void testRemoveDataSet();

   /*!
    * \brief
    */
   void testRemoveDataSetInvalidId();


   /*!
    * \brief
    */
   void testUpdateDataSet();

   /*!
    * \brief
    */
   void testUpdateDataSetInvalidId();


   /*!
    * \brief
    */
   void testFindDataSet();


   /*!
    * \brief
    */
   void testFindDataSetInvalidID();


   //******End of Test functions****



};

#endif // __TERRAMA2_UNITTEST_WS_COLLECTOR_CLIENT_TSCLIENT__

