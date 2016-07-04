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
  \file terrama2/services/view/core/DataManager.hpp

  \brief Model class for the view configuration.

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_SERVICES_VIEW_CORE_DATAMANAGER_HPP__
#define __TERRAMA2_SERVICES_VIEW_CORE_DATAMANAGER_HPP__

#include "../../../core/data-model/DataManager.hpp"
#include "Typedef.hpp"
#include "Shared.hpp"

namespace terrama2
{
  namespace services
  {
    namespace view
    {
      namespace core
      {
        class DataManager : public terrama2::core::DataManager
        {
          Q_OBJECT

        public:

          DataManager() = default;

          //! Default destructor
          virtual ~DataManager() = default;

          DataManager(const DataManager& other) = delete;
          DataManager(DataManager&& other) = delete;
          DataManager& operator=(const DataManager& other) = delete;
          DataManager& operator=(DataManager&& other) = delete;

          using terrama2::core::DataManager::add;
          using terrama2::core::DataManager::update;

          /*!
            \brief Register a map in the manager.

            At end it will emit MapAdded(ViewPtr) signal.

            \param map The map to be registered into the manager.

            \pre The map must not have a terrama2::core::InvalidId.
            \pre A map with the same name must not be already in the manager.

            \exception terrama2::InvalidArgumentException If it is not possible to add the map.

            \note Thread-safe.
          */
          virtual void add(ViewPtr map);

          /*!
            \brief Update a given map.

            Emits MapUpdated() signal if the map is updated successfully.

            \param dataseries     map to update.
            \param shallowSave If true it will update only the dataseries attributes.

            \pre The map must not have a terrama2::core::InvalidId.
            \pre The map must exist in the DataManager.

            \exception terrama2::InvalidArgumentException If it is not possible to update the map.

            \note Thread-safe.
          */
          virtual void update(ViewPtr map);

          /*!
            \brief Removes the map with the given id.

            Emits MapRemoved() signal if the DataSeries is removed successfully.

            \param id ID of the map to remove.

            \exception terrama2::InvalidArgumentException If it is not possible to remove the map.

            \note Thread-safe.
          */
          virtual void removeView(ViewId viewId);

          /*!
            \brief Retrieves the View with the given ViewId.

            \param id The map ViewId.

            \return DataProviderPtr A smart pointer to the map

            \exception terrama2::InvalidArgumentException If some error occur when trying to find the map.

            \note Thread-safe.
          */
          virtual ViewPtr findView(ViewId id) const;

          //! Verify if the View already exists in the DataManager
          virtual bool hasView(ViewId id) const;

          /*!
            \brief Parsers the QJsonObject for terrama2::core::DataProvider, terrama2::core::DataSeries and View to be added.

            The valid tags are:
              - "dataproviders"
              - "dataseries"
              - "view"
          */
          virtual void addJSon(const QJsonObject& obj) override;

          /*!
            \brief Parsers the QJsonObject for terrama2::core::DataProvider, terrama2::core::DataSeries and View to be removed.

            The valid tags are:
              - "dataproviders"
              - "dataseries"
              - "view"
          */
          virtual void removeJSon(const QJsonObject& obj) override;

        signals:
          //! Signal to notify that a View has been added.
          void viewAdded(ViewPtr);
          //! Signal to notify that a View has been updated.
          void viewUpdated(ViewPtr);
          //! Signal to notify that a View has been removed.
          void viewRemoved(ViewId);

        protected:
          std::map<ViewId, ViewPtr> view_;//!< A map from ViewId to View.

        };
      } // end namespace core
    }   // end namespace view
  }     // end namespace services
} // end namespace terrama2

#endif //__TERRAMA2_SERVICES_VIEW_CORE_DATAMANAGER_HPP__
