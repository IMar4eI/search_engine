#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "SearchEngine.h"
#include "ResultsModel.h"
#include "ConverterJSON.h"
#include <iostream>

using namespace Qt::StringLiterals;

void initializeSearchEngine(SearchEngine& searchEngine) {
  try {
    // Instantiate the ConverterJSON class
    ConverterJSON converter;

    //Get and display text documents
    std::cout << "Reading text documents from config.json..." << std::endl;
    auto documents = converter.GetTextDocuments();
    searchEngine.setDocuments(documents);

    // Get and display the maximum number of responses
    std::cout << "Getting max responses from config.json..." << std::endl;
    int max_responses = converter.GetResponsesLimit();
    searchEngine.setMaxResponses(max_responses);

    // Get and display search requests
    std::cout << "Reading search requests from requests.json..." << std::endl;
    auto requests = converter.GetRequests();
    searchEngine.setRequests(requests);

    std::cout << "All initial data loaded successfully!" << std::endl;
  } catch (const std::exception& ex) {
    std::cerr << "An error occurred during initialization: " << ex.what() << std::endl;
    throw;
  }
}

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;

  SearchEngine search_engine;
  ResultsModel results_model;

  // Initialize search engine with JSON data
  try {
    initializeSearchEngine(search_engine);
  } catch (const std::exception& ex) {
    std::cerr << "Error during initialization: " << ex.what() << std::endl;
    return 1;
  }

  // Set up context properties for QML
  engine.rootContext()->setContextProperty("cppInterface", &search_engine);
  engine.rootContext()->setContextProperty("resultsModel", &results_model);

  // Load QML interface
  const QUrl url(u"qrc:/qml/main.qml"_s);
  QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                   &app, [url](QObject *obj, const QUrl &objUrl) {
                     if (!obj && url == objUrl) QCoreApplication::exit(-1);
                   }, Qt::QueuedConnection);
  engine.load(url);

  return app.exec();
}
