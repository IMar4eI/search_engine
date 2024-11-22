#include "ConverterJSON.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>
#include <stdexcept>

/**
 * @brief Reads text documents specified in the config.json file.
 * @return Vector containing the contents of each document.
 */
std::vector<std::string> ConverterJSON::GetTextDocuments() {
    std::vector<std::string> documents;

    QString base_path = QDir::currentPath();
    QString config_path = QDir(base_path).filePath("../data/config.json");
    QFile config_file(config_path);

    if (!config_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("Cannot open config file: " + config_path.toStdString());
    }

    QByteArray config_data = config_file.readAll();
    config_file.close();

    QJsonDocument config_doc = QJsonDocument::fromJson(config_data);
    if (config_doc.isNull()) {
        throw std::runtime_error("Error parsing JSON in config file.");
    }

    QJsonObject config_json = config_doc.object();

    // Check if the "config" section exists
    if (!config_json.contains("config") || !config_json["config"].isObject()) {
        throw std::runtime_error("Config file is missing 'config' section or it is not an object.");
    }

    QJsonObject config_section = config_json["config"].toObject();

    // Check for "version" in "config"
    if (!config_section.contains("version") || !config_section["version"].isString()) {
        throw std::runtime_error("Config file is missing 'version' field in 'config' section or it is not a string.");
    }

    // Check if "files" section exists
    if (!config_json.contains("files") || !config_json["files"].isArray()) {
        throw std::runtime_error("Config file is missing 'files' section or it is not an array.");
    }

    QJsonArray files_array = config_json["files"].toArray();

    // Reserve space in the vector to avoid reallocations
    documents.reserve(files_array.size());

    // Iterate over each file path in "files"
    for (const QJsonValue& file_path_value : files_array) {
        if (!file_path_value.isString()) {
            std::cerr << "File path is not a string. Skipping entry." << std::endl;
            continue;
        }
        QString file_path = file_path_value.toString();
        QString full_file_path = QDir(base_path).filePath(file_path);

        QFile file(full_file_path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            std::cerr << "Cannot open file: " << full_file_path.toStdString() << ". Skipping this file." << std::endl;
            continue;
        }

        QByteArray file_data = file.readAll();
        documents.push_back(file_data.toStdString());
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
    QString base_path = QDir::currentPath();
    QString config_path = QDir(base_path).filePath("../data/config.json");
    QFile config_file(config_path);

    if (!config_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("Cannot open config file: " + config_path.toStdString());
    }

    QByteArray config_data = config_file.readAll();
    config_file.close();

    QJsonDocument config_doc = QJsonDocument::fromJson(config_data);
    if (config_doc.isNull()) {
        throw std::runtime_error("Error parsing JSON in config file.");
    }

    QJsonObject config_json = config_doc.object();

    int max_responses = 5; // Default value

    // Check if "max_responses" exists in "config"
    if (config_json.contains("config") && config_json["config"].isObject()) {
        QJsonObject config_section = config_json["config"].toObject();
        if (config_section.contains("max_responses")) {
            if (config_section["max_responses"].isDouble()) {
                max_responses = config_section["max_responses"].toInt();
            } else {
                std::cerr << "'max_responses' in config file is not an integer. Using default value: 5" << std::endl;
            }
        } else {
            std::cerr << "'max_responses' not found in config file. Using default value: 5" << std::endl;
        }
    } else {
        std::cerr << "'config' section not found or is not an object. Using default value: 5" << std::endl;
    }

    return max_responses;
}

/**
 * @brief Reads search requests from requests.json file.
 * @return Vector containing each request as a string.
 */
std::vector<std::string> ConverterJSON::GetRequests() {
    std::vector<std::string> requests;
    QString base_path = QDir::currentPath();
    QString requests_path = QDir(base_path).filePath("../data/requests.json");
    QFile requests_file(requests_path);

    if (!requests_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("Cannot open requests file: " + requests_path.toStdString());
    }

    QByteArray requests_data = requests_file.readAll();
    requests_file.close();

    QJsonDocument requests_doc = QJsonDocument::fromJson(requests_data);
    if (requests_doc.isNull()) {
        throw std::runtime_error("Error parsing JSON in requests file.");
    }

    QJsonObject requests_json = requests_doc.object();

    // Check if "requests" section exists
    if (!requests_json.contains("requests") || !requests_json["requests"].isArray()) {
        throw std::runtime_error("Requests file is missing 'requests' section or it is not an array.");
    }

    QJsonArray requests_array = requests_json["requests"].toArray();

    // Extract each request
    for (const QJsonValue& request_value : requests_array) {
        if (!request_value.isString()) {
            std::cerr << "Request is not a string. Skipping entry." << std::endl;
            continue;
        }
        QString request = request_value.toString();
        requests.push_back(request.toStdString());
    }

    if (requests.empty()) {
        throw std::runtime_error("No valid requests found in requests.json.");
    }

    return requests;
}

/**
 * @brief Writes the search results to answers.json file.
 * @param answers Vector of vectors containing RelativeIndex objects for each request.
 */
void ConverterJSON::putAnswers(const std::vector<std::vector<RelativeIndex>>& answers) {
    if (answers.empty()) {
        std::cerr << "No answers to write to answers.json." << std::endl;
        return;
    }

    QJsonObject answers_json;
    int request_id = 1;

    // Iterate over each request's results
    for (const auto& result_for_request : answers) {
        QString request_key = QString("request%1").arg(request_id);
        ++request_id;

        QJsonObject result;

        if (result_for_request.empty()) {
            result["result"] = false;
        } else {
            result["result"] = true;

            if (result_for_request.size() > 1) {
                QJsonArray relevance_array;
                for (const auto& rel : result_for_request) {
                    QJsonObject rel_obj;
                    rel_obj["docid"] = static_cast<int>(rel.doc_id);
                    rel_obj["rank"] = rel.rank;
                    relevance_array.append(rel_obj);
                }
                result["relevance"] = relevance_array;
            } else {
                const auto& rel = result_for_request.front();
                result["docid"] = static_cast<int>(rel.doc_id);
                result["rank"] = rel.rank;
            }
        }

        answers_json[request_key] = result;
    }

    QJsonObject root_json;
    root_json["answers"] = answers_json;

    QJsonDocument answers_doc(root_json);

    QString base_path = QDir::currentPath();
    QString data_dir = QDir(base_path).filePath("../data");

    // Create "data" directory if it doesn't exist
    QDir dir(data_dir);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            throw std::runtime_error("Failed to create directory 'data'.");
        }
    }

    QString answers_path = QDir(data_dir).filePath("answers.json");
    QFile answers_file(answers_path);
    if (!answers_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw std::runtime_error("Cannot open answers file for writing: " + answers_path.toStdString());
    }

    QByteArray answers_data = answers_doc.toJson(QJsonDocument::Indented);
    answers_file.write(answers_data);
    answers_file.close();

    std::cout << "Answers successfully written to: " << answers_path.toStdString() << std::endl;
}