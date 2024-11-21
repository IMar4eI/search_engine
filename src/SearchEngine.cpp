#include "SearchEngine.h"
#include "ResultsModel.h"
#include <QQmlContext>
#include <QAbstractListModel>
#include <QDebug>
#include <iostream>


/**
 * @brief Constructs the SearchEngine and initializes the indexing process.
 * @param parent Optional parent object.
 */
SearchEngine::SearchEngine(QObject *parent)
  : QObject(parent), searchServer(index), qmlEngine(nullptr) {
  try {
    ConverterJSON converter;

    // Retrieve documents and update the inverted index
    auto docs = converter.GetTextDocuments();
    index.UpdateDocumentBase(docs);

    std::cout << "Indexing completed successfully." << std::endl;
  } catch (const std::exception& ex) {
    std::cerr << "Error during initialization: " << ex.what() << std::endl;
  }
}

/**
 * @brief Sets the QQmlApplicationEngine to interact with QML.
 * @param engine Pointer to the QQmlApplicationEngine.
 */
void SearchEngine::setEngine(QQmlApplicationEngine *engine) {
  qmlEngine = engine;
}

/**
 * @brief Sets the text documents for the search engine.
 * @param documents A vector of text documents.
 */
void SearchEngine::setDocuments(const std::vector<std::string>& documents) {
  index.UpdateDocumentBase(documents);
  qDebug() << "Documents have been updated in the index.";
}

/**
 * @brief Sets the maximum number of responses for a query.
 * @param maxResponses The maximum number of responses.
 */
void SearchEngine::setMaxResponses(int maxResponses) {
  qDebug() << "Maximum responses setting is not directly supported in SearchServer.";
}

/**
 * @brief Sets the search requests to be processed.
 * @param requests A vector of search requests.
 */
void SearchEngine::setRequests(const std::vector<std::string>& requests) {
  auto results = searchServer.search(requests);
  qDebug() << "Search results have been updated.";

  // Iterate through results and unpack RelativeIndex
  for (const auto& result : results) {
    for (const auto& index : result) {
      qDebug() << "Document ID: " << index.doc_id
               << "Relevance: " << index.rank;
    }
  }
}


/**
 * @brief Executes a search based on the provided query string.
 * @param query The user's search query.
 */
void SearchEngine::performSearch(const QString &query) {
  if (!qmlEngine) return;

  try {
    // Convert the QString query to std::string
    std::vector<std::string> requests = { query.toStdString() };

    // Perform the search using the search server
    auto search_results = searchServer.search(requests);

    if (!search_results.empty()) {
      // Update the model with the search results
      updateModel(search_results[0]);
    } else {
      // Clear the model if there are no results
      updateModel({});
    }
  } catch (const std::exception& ex) {
    qWarning() << "Error during search: " << ex.what();
  }
}



/**
 * @brief Updates the QML model with new search results.
 * @param results Vector of RelativeIndex containing search results.
 */
void SearchEngine::updateModel(const std::vector<RelativeIndex> &results) {
  auto context = qmlEngine->rootContext();
  auto resultsModel = context->contextProperty("resultsModel").value<ResultsModel *>();

  if (!resultsModel) {
    // Create a new ResultsModel if it doesn't exist
    resultsModel = new ResultsModel(this);
    context->setContextProperty("resultsModel", resultsModel);
  }

  // Update the model with the new results
  resultsModel->setResults(results);
}

SearchEngine::~SearchEngine() {}
