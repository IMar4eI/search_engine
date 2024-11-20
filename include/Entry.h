#pragma once

#include <cstddef>

/**
  * @brief Represents the frequency of a word in a specific document.
  */
struct Entry {
  size_t doc_id; // Document ID
  size_t count; // Number of times the word appears in the document

  /**
     * Equality operator to compare two Entry objects.
     * Required for testing scenarios.
     * @param other Another Entry object to compare with.
     * @return True if both doc_id and count are equal; otherwise, false.
     */
  bool operator==(const Entry& other) const {
    return doc_id == other.doc_id && count == other.count;
  }
};
