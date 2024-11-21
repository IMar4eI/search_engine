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

  /**
     * @brief Sets the text documents for the search engine.
     * @param documents A vector of text documents.
     */
   void setDocuments(const std::vector<std::string>& documents);

  /**
     * @brief Sets the maximum number of responses for a query.
     * @param maxResponses The maximum number of responses.
     */
   void setMaxResponses(int maxResponses);

  /**
     * @brief Sets the search requests to be processed.
     * @param requests A vector of search requests.
     */
   void setRequests(const std::vector<std::string>& requests);

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