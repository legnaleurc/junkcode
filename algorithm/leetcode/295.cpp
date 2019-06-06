class MedianFinder {
public:
    /** initialize your data structure here. */
    MedianFinder() {
        this->median = 0.0;
    }

    void addNum(int num) {
        if (this->small.empty() && this->big.empty()) {
            this->median = num;
            this->small.push(num);
            return;
        }

        if (this->small.size() > this->big.size()) {
            if (num > this->median) {
                this->big.push(num);
            } else {
                this->big.push(this->small.top());
                this->small.pop();
                this->small.push(num);
            }
            this->median = (this->small.top() + this->big.top()) / 2.0;
        } else if (this->small.size() < this->big.size()) {
            if (num < this->median) {
                this->small.push(num);
            } else {
                this->small.push(this->big.top());
                this->big.pop();
                this->big.push(num);
            }
            this->median = (this->small.top() + this->big.top()) / 2.0;
        } else {
            if (num > this->median) {
                this->big.push(num);
                this->median = this->big.top();
            } else {
                this->small.push(num);
                this->median = this->small.top();
            }
        }
    }

    double findMedian() {
        return this->median;
    }

private:
    double median;
    priority_queue<double> small;
    priority_queue<double, vector<double>, greater<double>> big;
};

/**
 * Your MedianFinder object will be instantiated and called as such:
 * MedianFinder* obj = new MedianFinder();
 * obj->addNum(num);
 * double param_2 = obj->findMedian();
 */
