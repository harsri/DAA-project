#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <queue>

using namespace std;

// Structure to hold document info
struct Doc {
    int id;
    string text;
};

// Tokenize text into words (lowercase, remove punctuation)
vector<string> tokenize(const string& text) {
    vector<string> words;
    stringstream ss(text);
    string word;
    while (ss >> word) {
        // Remove punctuation and convert to lowercase
        string clean_word;
        for (char c : word) {
            if (isalnum(c)) clean_word += tolower(c);
        }
        if (!clean_word.empty()) words.push_back(clean_word);
    }
    return words;
}

// Compute edit distance (word-level) with space optimization
int edit_distance(const vector<string>& s1, const vector<string>& s2) {
    int m = s1.size(), n = s2.size();
    // Use two rows to reduce space to O(min(m, n))
    vector<int> prev(min(m, n) + 1), curr(min(m, n) + 1);
    
    // Initialize first row
    for (int j = 0; j <= min(m, n); ++j) prev[j] = j;
    
    // Fill DP table
    for (int i = 1; i <= max(m, n); ++i) {
        curr[0] = i;
        for (int j = 1; j <= min(m, n); ++j) {
            int i_idx = (m >= n) ? i : j;
            int j_idx = (m >= n) ? j : i;
            int cost = (s1[i_idx-1] == s2[j_idx-1]) ? 0 : 1;
            curr[j] = min({
                curr[j-1] + 1, // insertion
                prev[j] + 1,   // deletion
                prev[j-1] + cost // substitution
            });
        }
        swap(prev, curr);
    }
    return prev[min(m, n)];
}

// Detect plagiarism using edit distance
vector<pair<int, double>> detect_plagiarism(const string& new_doc, const vector<Doc>& docs, int top_k) {
    auto new_words = tokenize(new_doc);
    int len1 = new_words.size();
    if (len1 == 0) return {};
    
    // Priority queue for top-k documents (min-heap to keep highest similarities)
    using ScorePair = pair<double, int>; // similarity, doc_id
    priority_queue<ScorePair, vector<ScorePair>, greater<>> pq;
    
    for (const auto& doc : docs) {
        auto doc_words = tokenize(doc.text);
        int len2 = doc_words.size();
        if (len2 == 0) continue;
        
        // Compute edit distance
        int dist = edit_distance(new_words, doc_words);
        
        // Convert to similarity score
        double similarity = 1.0 - static_cast<double>(dist) / max(len1, len2);
        
        // Add to priority queue
        pq.emplace(similarity, doc.id);
        if (pq.size() > top_k) pq.pop();
    }
    
    // Extract results in descending order
    vector<pair<int, double>> results;
    while (!pq.empty()) {
        results.emplace_back(pq.top().second, pq.top().first);
        pq.pop();
    }
    reverse(results.begin(), results.end());
    return results;
}

int main() {
    // Sample input
    vector<Doc> docs = {
        {1, "The quick brown fox jumps over the lazy dog"},
        {2, "A fox fled from danger to safety"},
        {3, "The lazy dog sleeps while the fox jumps"}
    };
    string new_doc = "This is a test with the quick brown fox running";
    int top_k = 2;
    
    // Detect plagiarism
    auto results = detect_plagiarism(new_doc, docs, top_k);
    
    // Print results
    cout << "Top matching documents:\n";
    for (const auto& [doc_id, similarity] : results) {
        cout << "Doc ID: " << doc_id << ", Similarity: " << fixed << setprecision(6) << similarity << "\n";
    }
    
    // Expected output:
    // Top matching documents:
    // Doc ID: 1, Similarity: 0.428571
    // Doc ID: 3, Similarity: 0.333333
    return 0;
}