class Solution {
public:
    bool exist(vector<vector<char>>& board, string word) {
        if (word.empty()) {
            return false;
        }
        set<pair<int, int>> cache;
        for (int row = 0; row < board.size(); ++row) {
            for (int col = 0; col < board[row].size(); ++col) {
                if (board[row][col] == word[0]) {
                    auto rv = cache.insert({ row, col });
                    if (walk(board, row, col, word, 1, cache)) {
                        return true;
                    }
                    cache.erase(rv.first);
                }
            }
        }
        return false;
    }
    
    bool walk(const vector<vector<char>>& board, int row, int col,
              const string& word, int pos,
              set<pair<int, int>>& cache) {
        if (pos >= word.size()) {
            return true;
        }
        if (row > 0 && !cache.count({ row - 1, col }) && word[pos] == board[row-1][col]) {
            auto rv = cache.insert({ row - 1, col });
            if (walk(board, row - 1, col, word, pos + 1, cache)) {
                return true;
            }
            cache.erase(rv.first);
        }
        if (row < (board.size() - 1) && !cache.count({ row + 1, col }) && word[pos] == board[row+1][col]) {
            auto rv = cache.insert({ row + 1, col });
            if (walk(board, row + 1, col, word, pos + 1, cache)) {
                return true;
            }
            cache.erase(rv.first);
        }
        if (col > 0 && !cache.count({ row, col - 1 }) && word[pos] == board[row][col-1]) {
            auto rv = cache.insert({ row, col - 1 });
            if (walk(board, row, col - 1, word, pos + 1, cache)) {
                return true;
            }
            cache.erase(rv.first);
        }
        if (col < (board[row].size() - 1) && !cache.count({ row, col + 1 }) && word[pos] == board[row][col+1]) {
            auto rv = cache.insert({ row, col + 1 });
            if (walk(board, row, col + 1, word, pos + 1, cache)) {
                return true;
            }
            cache.erase(rv.first);
        }
        return false;
    }
};
