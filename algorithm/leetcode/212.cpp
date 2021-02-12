class Solution {
public:
    vector<string> findWords(vector<vector<char>>& board, vector<string>& words) {
        vector<string> rv;
        for (const auto & word : words) {
            bool found = false;
            for (int row = 0; !found && row < board.size(); ++row) {
                for (int col = 0; !found && col < board[row].size(); ++col) {
                    if (board[row][col] == word[0] && searchWord(board, row, col, word, 0)) {
                        rv.push_back(word);
                        found = true;
                    }
                }
            }
        }
        return rv;
    }

    bool searchWord(
        vector<vector<char>>& board,
        int row,
        int col,
        const string& word,
        int i
    ) {
        if (i >= word.size()) {
            return true;
        }
        if (row < 0 || col < 0 || row >= board.size() || col >= board[row].size()) {
            return false;
        }
        if (board[row][col] != word[i]) {
            return false;
        }

        board[row][col] = toupper(board[row][col]);
        bool ok = (false
            || searchWord(board, row - 1, col, word, i + 1)
            || searchWord(board, row + 1, col, word, i + 1)
            || searchWord(board, row, col - 1, word, i + 1)
            || searchWord(board, row, col + 1, word, i + 1)
        );
        board[row][col] = tolower(board[row][col]);
        return ok;
    }
};
