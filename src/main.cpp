#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"
#include <QCoreApplication>
#include <QDir>
#include <iostream>

/**
 * @brief The entry point of the program.
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @return Program exit code.
 */
int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  try {
    std::cout << "Initializing search engine..." << std::endl;
    std::cout << "Current working directory: " << QDir::currentPath().toStdString() << std::endl;
    ConverterJSON converter;
    auto documents = converter.GetTextDocuments();
    int max_responses = converter.GetResponsesLimit();
    auto requests = converter.GetRequests();

    std::cout << "Updating document base..." << std::endl;
    InvertedIndex idx;
    idx.UpdateDocumentBase(documents);

    std::cout << "Creating search server..." << std::endl;
    SearchServer srv(idx, max_responses);

    std::cout << "Processing search requests..." << std::endl;
    auto answers = srv.search(requests);

    std::cout << "Writing answers to file..." << std::endl;
    converter.putAnswers(answers);

    std::cout << "Search completed successfully. Answers are saved in data/answers.json" << std::endl;

  } catch (const std::exception& ex) {
    std::cerr << "An error occurred during program execution: " << ex.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "An unknown error occurred during program execution." << std::endl;
    return 1;
  }
}