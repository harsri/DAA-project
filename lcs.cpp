#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <queue>
#include <iomanip>

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
        string clean_word;
        for (char c : word) {
            if (isalnum(c)) clean_word += tolower(c);
        }
        if (!clean_word.empty()) words.push_back(clean_word);
    }
    return words;
}

// Compute LCS length (word-level) with space optimization
int lcs_length(const vector<string>& s1, const vector<string>& s2) {
    int m = s1.size(), n = s2.size();
    // Use two rows to reduce space to O(min(m, n))
    vector<int> prev(min(m, n) + 1, 0), curr(min(m, n) + 1, 0);
    
    for (int i = 1; i <= max(m, n); ++i) {
        for (int j = 1; j <= min(m, n); ++j) {
            int i_idx = (m >= n) ? i : j;
            int j_idx = (m >= n) ? j : i;
            if (s1[i_idx-1] == s2[j_idx-1]) {
                curr[j] = prev[j-1] + 1;
            } else {
                curr[j] = max(curr[j-1], prev[j]);
            }
        }
        swap(prev, curr);
    }
    return prev[min(m, n)];
}

// Detect plagiarism using LCS
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
        
        // Compute LCS length
        int lcs_len = lcs_length(new_words, doc_words);
        
        // Convert to similarity score
        double similarity = static_cast<double>(lcs_len) / max(len1, len2);
        
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
    string new_doc = "A fox fled from danger to safety";
    int top_k = 2;
    

    auto results = detect_plagiarism(new_doc, docs, top_k);
    
    cout << "Top matching documents:\n";
    for (const auto& [doc_id, similarity] : results) {
        cout << "Doc ID: " << doc_id << ", Similarity: " << fixed << setprecision(6) << similarity << "\n";
    }
    


}