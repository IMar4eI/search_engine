#pragma once

#include <QObject>
#include <QQmlApplicationEngine>
#include <ConverterJSON.h>
#include <InvertedIndex.h>
#include <SearchServer.h>

/**
 * @brief The SearchEngine class integrates the search functionality with the QML interface.
 */
class SearchEngine : public QObject {
  Q_OBJECT
public:
  /**
     * @brief Constructs the SearchEngine object and initializes the search components.
     * @param parent Optional parent object.
     */
  explicit SearchEngine(QObject *parent = nullptr);

  /**
     * @brief Performs a search based on the user's query.
     * @param query The search query input by the user.
     */
  Q_INVOKABLE void performSearch(const QString &query);

  /**
     * @brief Sets the QML application engine for UI interaction.
     * @param engine Pointer to the QQmlApplicationEngine.
     */
  void setEngine(QQmlApplicationEngine *engine);

  ~SearchEngine();

private:
  ConverterJSON converter; // Handles JSON conversion.
  InvertedIndex index; // Inverted index for document retrieval.
  SearchServer searchServer; // Search server processing queries.
  QQmlApplicationEngine *qmlEngine; // Pointer to the QML engine.

  /**
     * @brief Updates the QML model with new search results.
     * @param results Vector of RelativeIndex containing search results.
     */
  void updateModel(const std::vector<RelativeIndex> &results);
};