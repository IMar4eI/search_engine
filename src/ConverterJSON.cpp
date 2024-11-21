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
  std::string config_path = "../data/config.json";
  std::ifstream config_file(config_path);

  if (!config_file.is_open()) {
    throw std::runtime_error("Cannot open config file: " + config_path);
  }

  json config_json;
  try {
    config_file >> config_json;
  } catch (const json::parse_error& e) {
    throw std::runtime_error("Error parsing JSON in config file: " + std::string(e.what()));
  }
  config_file.close();

  // Check if the "config" sections exists
  if (config_json.find("config") == config_json.end()) {
    throw std::runtime_error("config file is empty");
  }

  // Check if the "config" section exists
  if (!config_json.contains("config") || !config_json["config"].is_object()) {
    throw std::runtime_error("Config file is missing 'config' section or it is not an object.");
  }

  // Check for "version" in "config"
  if (!config_json["config"].contains("version") || !config_json["config"]["version"].is_string()) {
    throw std::runtime_error("Config file is missing 'version' field in 'config' section or it is not a string.");
  }

  // Check if "files" section exists
  if (!config_json.contains("files") || !config_json["files"].is_array()) {
    throw std::runtime_error("Config file is missing 'files' section or it is not an array.");
  }

  // Reserve space in the vector to avoid reallocations
  documents.reserve(config_json["files"].size());

  // Iterate over each file path in "files"
  for (const auto& file_path_json : config_json["files"]) {
    if (!file_path_json.is_string()) {
      std::cerr << "File path is not a string. Skipping entry." << std::endl;
      continue;
    }
    std::string file_path = file_path_json.get<std::string>();

    std::ifstream file(file_path);
    if (!file.is_open()) {
      std::cerr << "Cannot open file: " << file_path << ". Skipping this file." << std::endl;
      continue;
    }

    // Read the entire content of the file
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    documents.push_back(std::move(content));
    file.close();
  }

  if (documents.empty()) {
    throw std::runtime_error("No documents were read. Please check the file paths in config.json.");
  }

  return documents;
}

/**
 * Reads the "max_responses" value from config.json.
 * @return Maximum number of responses; returns default value 5 if not specified.
 */
int ConverterJSON::GetResponsesLimit() {
  std::string config_path = "../data/config.json";
  std::ifstream config_file(config_path);

  if (!config_file.is_open()) {
    throw std::runtime_error("Cannot open config file: " + config_path);
  }

  json config_json;
  try {
    config_file >> config_json;
  } catch (const json::parse_error& e) {
    throw std::runtime_error("Error parsing JSON in config file: " + std::string(e.what()));
  }
  config_file.close();

  int max_responses = 5; // Default value

  // Check if "max_responses" exists in "config"
  if (config_json.contains("config") && config_json["config"].contains("max_responses")) {
    if (config_json["config"]["max_responses"].is_number_integer()) {
      max_responses = config_json["config"]["max_responses"].get<int>();
    } else {
      std::cerr << "'max_responses' in config file is not an integer. Using default value: 5" << std::endl;
    }
  } else {
    std::cerr << "'max_responses' not found in config file. Using default value: 5" << std::endl;
  }

  return max_responses;
}

/**
 * Reads search requests from requests.json file.
 * @return Vector containing each request as a string.
 */
std::vector<std::string> ConverterJSON::GetRequests() {
  std::vector<std::string> requests;
  std::string requests_path = "../data/requests.json";
  std::ifstream requests_file(requests_path);

  if (!requests_file.is_open()) {
    throw std::runtime_error("Cannot open requests file: " + requests_path);
  }

  json requests_json;
  try {
    requests_file >> requests_json;
  } catch (const json::parse_error& e) {
    throw std::runtime_error("Error parsing JSON in requests file: " + std::string(e.what()));
  }
  requests_file.close();

  // Check if "requests" section exists
  if (!requests_json.contains("requests") || !requests_json["requests"].is_array()) {
    throw std::runtime_error("Requests file is missing 'requests' section or it is not an array.");
  }

  // Extract each request
  for (const auto& request_json : requests_json["requests"]) {
    if (!request_json.is_string()) {
      std::cerr << "Request is not a string. Skipping entry." << std::endl;
      continue;
    }
    requests.push_back(request_json.get<std::string>());
  }

  if (requests.empty()) {
    throw std::runtime_error("No valid requests found in requests.json.");
  }

  return requests;
}

/**
 * Writes the search results to answers.json file.
 * @param answers Vector of vectors containing RelativeIndex objects for each request.
 */
void ConverterJSON::putAnswers(const std::vector<std::vector<RelativeIndex>>& answers) {
  if (answers.empty()) {
        std::cerr << "No answers to write to answers.json." << std::endl;
        return;
    }

    json answers_json;
    int request_id = 1;

    // Iterate over each request's results
    for (const auto& result_for_request : answers) {
        std::ostringstream oss;
        oss << "request" << request_id;
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
    std::string data_dir = "data";
    if (!std::filesystem::exists(data_dir)) {
        std::error_code ec;
        std::filesystem::create_directory(data_dir, ec);
        if (ec) {
            throw std::runtime_error("Failed to create directory 'data': " + ec.message());
        }
    }

    std::string answers_path = "../data/answers.json";
    std::ofstream answers_file(answers_path);
    if (!answers_file.is_open()) {
        throw std::runtime_error("Cannot open answers file for writing: " + answers_path);
    }

    // Write formatted JSON to the file
    try {
        answers_file << std::setw(4) << answers_json;
        answers_file.close();
    } catch (const std::exception& e) {
        throw std::runtime_error("Error writing to answers file: " + std::string(e.what()));
    }

    std::cout << "Answers successfully written to: " << std::filesystem::absolute(answers_path) << std::endl;
}