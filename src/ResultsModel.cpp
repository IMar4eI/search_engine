#include "ResultsModel.h"

/**
  * @brief Constructs the ResultsModel object.
  * @param parent Optional parent object.
  */
ResultsModel::RsultsModel(QObject *parent)
 : QAbstractListModel(parent) {}

/**
  * @brief Updates the model with new search results.
  * Notifies any connected views about the data change.
  * @param results A vector of RelativeIndex containing the search results.
  */
void ResultsModel::setResults(const std::vector<RelativeIndex> &results) {
  beginResetModel();
  m_results = results;
  endResetModel();
}

/**
  * @brief Returns the number of rows in the model.
  * @param parent The parent index (unused).
  * @return The number of results in the model.
  */
int ResultsModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent)
  return static_cast<int>(m_results.size());
}

/**
  * @brief Retrieves data for a given index and role.
  * Provides the data needed by the QML view based on the role requested.
  * @param index The index of the item.
  * @param role The data role (DocIdRole or RankRole).
  * @return The requested data as a QVariant.
  */
QVariant ResultsModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() >= static_cast<int>(m_results.size()))
    return QVariant();

  const auto &item = m_results.at(index.row());

  if (role == DocIdRole) return static_cast<int>(item.doc_id);
  else if (role == RankRole) return item.rank;

  return QVariant();
}

/**
  * @brief Provides the mapping of roles to names for QML access.
  * This allows QML to access properties like "docid" and "rank" directly.
  * @return A hash mapping role integers to role names.
  */
QHash<int, QByteArray> ResultsModel::roleNames() const {
  return {
          { DocIdRole, "docid" },
          { RankRole, "rank" }
  };
}

ResultsModel::~ResultsModel() {}
