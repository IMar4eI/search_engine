#pragma once

#include <cstddef>

/**
 * @brief Represents the relevance of a document to a search query.
 */
struct RelativeIndex {
  size_t doc_id; /**< Identifier of the document */
  float rank; /**< Relevance score of the document */

  /**
     * @brief Compares two RelativeIndex objects for equality.
     * @param other Another RelativeIndex to compare with.
     * @return True if both doc_id and rank are equal; otherwise, false.
     */
  bool operator==(const RelativeIndex& other) const {
    return doc_id == other.doc_id && rank == other.rank;
  }
};