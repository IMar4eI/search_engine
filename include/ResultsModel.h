#pragma once

#include <QAbstractListModel>
#include <vector>
#include "RelativeIndex.h"

/**
 * @brief The ResultsModel class provides a model for search results to be displayed in a QML view.
 */
class ResultsModel : public QAbstractListModel {
  Q_OBJECT
public:
  /**
     * @brief Roles used for data retrieval in the model.
     */
  enum Roles {
    DocIdRole = Qt::UserRole + 1, ///< Role for the document ID.
    RankRole                     ///< Role for the rank (relevance score).
  };

  /**
     * @brief Constructs the ResultsModel object.
     * @param parent Optional parent object.
     */
  ResultsModel(QObject *parent = nullptr);

  /**
     * @brief Updates the model with new search results.
     * @param results A vector of RelativeIndex containing the search results.
     */
  void setResults(const std::vector<RelativeIndex> &results);

  /**
     * @brief Returns the number of rows in the model.
     * @param parent The parent index.
     * @return The number of rows.
     */
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  /**
     * @brief Retrieves data for a given index and role.
     * @param index The index of the item.
     * @param role The data role.
     * @return The data as a QVariant.
     */
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  /**
     * @brief Provides the mapping of roles to names for QML access.
     * @return A hash mapping role integers to role names.
     */

  QHash<int, QByteArray> roleNames() const override;

  ~ResultsModel();
private:
  std::vector<RelativeIndex> m_results; ///< Container for the search results.
};