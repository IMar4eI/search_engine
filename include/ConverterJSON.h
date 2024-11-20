#pragma once

#include <string>
#include <vector>
#include "RelativeIndex.h"

class ConverterJSON {
  public:
    ConverterJSON() = default;

    /**
     * Retrieves the content of files listed in config.json.
     * @return Vector containing the content of each file.
    */
    std::vector<std::string> GetTextDocuments();

    /**
     * Reads the max_responses field from config.json.
     * @return Maximum number of responses.
    */
     int GetResponsesLimit();

    /**
     * Retrieves search requests from requests.json file.
     * @return Vector containing the search requests.
    */
     std::vector<std::string> GetRequests();

    /**
     * Writes the answers to answers.json file.
     * @param answers Vector of vectors containing RelativeIndex objects for each request.
    */
     void putAnswers(const std::vector<std::vector<RelativeIndex>>& answers);
};