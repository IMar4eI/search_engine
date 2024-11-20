#include <iostream>
#include "ConverterJSON.h"
#include "InvertedIndex.h"

int main() {
  try {
    // Instantiate the ConverterJSON class
    ConverterJSON converter;

    // Get and display text documents
    std::cout << "Reading text documents from config.json..." << std::endl;
    auto documents = converter.GetTextDocuments();
    for (const auto& doc : documents) {
      std::cout << doc << std::endl;
    }

    // Get and display the maximum number of responses
    std::cout << "Getting max responses from config.json..." << std::endl;
    int max_responses = converter.GetResponsesLimit();
    std::cout << "Max responses: " << max_responses << std::endl;

    // Get and display search requests
    std::cout << "Reading search requests from requests.json..." << std::endl;
    auto requests = converter.GetRequests();
    std::cout << "Requests loaded: " << std::endl;
    for (const auto& req : requests) {
      std::cout << req << std::endl;
    }

    // Simulate answers for testing (you need to replace this with actual logic)
    std::cout << "Generating sample answers..." << std::endl;
    std::vector<std::vector<RelativeIndex>> answers;

    // Simulate 4 requests with some sample RelativeIndex data
    for (size_t i = 0; i < requests.size(); i++) {
      std::vector<RelativeIndex> answer;
      for (size_t j = 0; j < max_responses; j++) {
        answer.push_back({
                    static_cast<size_t>(i * max_responses + j),
                    static_cast<float>(1.0f / (j + 1))
                });
      }
      answers.push_back(answer);
    }

    // Write answers to answers.json
    std::cout << "Writting answers to answers.json..." << std::endl;
    converter.putAnswers(answers);

    std::cout << "All operations completed successfully!" << std::endl;

  } catch (const std::exception& ex) {
    std::cerr << "An error occurred: " << ex.what() << std::endl;
    return 1;
  }

}
