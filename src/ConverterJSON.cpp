#include "ConverterJSON.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <iomanip>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * Reads text documents specified in the config.json file.
 * @return Vector containing the contents of each document.
 */
std::vector<std::string> ConverterJSON::GetTextDocuments() {
  std::vector<std::string> documents;
  std::ifstream config_file("../data/config.json");

  if (!config_file.is_open()) {
    throw std::runtime_error("config file is missing");
  }

  json config_json;
  config_file >> config_json;
  config_file.close();

  // Check if the "config" sections exists
  if (config_json.find("config") == config_json.end()) {
    throw std::runtime_error("config file is empty");
  }

  // Check for "version" in "config"
  if (config_json["config"].find("version") == config_json["config"].end()) {
    throw std::runtime_error("config.json has incorrect file version");
  }

  // Check if "files" section exists
  if (config_json.find("files") == config_json.end()) {
    throw std::runtime_error("no files to process in config.json");
  }

  // Iterate over each file path in "files"
  for (const auto& file_path : config_json["files"]) {
    std::ifstream file(file_path.get<std::string>());

    if (!file.is_open()) {
      std::cerr << "Cannot open file: " << file_path << std::endl;
      continue;
    }

    // Read the entire content of the file
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    documents.push_back(content);
    file.close();
  }

  return documents;
}

/**
 * Reads the "max_responses" value from config.json.
 * @return Maximum number of responses; returns default value 5 if not specified.
 */
int ConverterJSON::GetResponsesLimit() {
  std::ifstream config_file("../data/config.json");

  if (!config_file.is_open()) {
    throw std::runtime_error("config file is missing");
  }

  json config_json;
  config_file >> config_json;
  config_file.close();

  int max_responses = 5; // Default value

  // Check if "max_responses" exists in "config"
  if (config_json.find("config") != config_json.end()) {
    if (config_json["config"].find("max_responses") != config_json["config"].end()) {
      max_responses = config_json["config"]["max_responses"].get<int>();
    }
  }

  return max_responses;
}

/**
 * Reads search requests from requests.json file.
 * @return Vector containing each request as a string.
 */
std::vector<std::string> ConverterJSON::GetRequests() {
  std::vector<std::string> requests;
  std::ifstream requests_file("../data/requests.json");

  if (!requests_file.is_open()) {
    throw std::runtime_error("requests.json file is missing");
  }

  json requests_json;
  requests_file >> requests_json;
  requests_file.close();

  // Check if "requests" section exists
  if (requests_json.find("requests") == requests_json.end()) {
    throw std::runtime_error("No requests found in requests.json");
  }

  // Extract each request
  for (const auto& request : requests_json["requests"]) {
    requests.push_back(request.get<std::string>());
  }

  return requests;
}

/**
 * Writes the search results to answers.json file.
 * @param answers Vector of vectors containing RelativeIndex objects for each request.
 */
void ConverterJSON::putAnswers(const std::vector<std::vector<RelativeIndex>>& answers) {
  json answers_json;

  int request_id = 1;

  // Iterate over each request's results
  for (const auto& result_for_request : answers) {
    std::ostringstream oss;
    oss << "request" << std::setw(3) << std::setfill('0') << request_id;
    std::string request_key = oss.str();
    ++request_id;

    json result;

    if (result_for_request.empty()) {
      result["result"] = false;
    } else {
      result["result"] = true;

      if (result_for_request.size() > 1) {
        json relevance = json::array();
        for (const auto& rel : result_for_request) {
          relevance.push_back({
            {"docid", rel.doc_id},
            {"rank", rel.rank}
          });
        }
        result["relevance"] = relevance;
      } else {
        const auto& rel = result_for_request.front();
        result["docid"] = rel.doc_id;
        result["rank"] = rel.rank;
      }
    }

    answers_json["answers"][request_key] = result;
  }

  // Create "data" directory if it doesn't exist
  std::filesystem::create_directory("data");

  std::ofstream answers_file("../data/answers.json");
  if (!answers_file.is_open()) {
    throw std::runtime_error("Cannot open answer.json for writting");
  }

  // Write formatted JSON to the file
  answers_file << std::setw(4) << answers_json.dump(4);
  answers_file.close();
}