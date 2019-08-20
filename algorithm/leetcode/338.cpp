class Solution {
    unordered_map<uint8_t, int> cache;
public:
    vector<int> countBits(int num) {
        vector<int> rv;
        for (int i = 0; i <= num; ++i) {
            rv.push_back(count(i));
        }
        return rv;
    }

    int count(int n) {
        uint8_t * bits = reinterpret_cast<uint8_t *>(&n);
        int sum = 0;
        for (int i = 0; i < sizeof(int); ++i) {
            auto it = cache.find(bits[i]);
            if (it != end(cache)) {
                sum += it->second;
                continue;
            }

            int cc = 0;
            uint8_t c = bits[i];
            while (c) {
                cc += c & 0x01;
                c = c >> 1;
            }
            cache.insert({ bits[i], cc });
            sum += cc;
        }
        return sum;
    }
};
