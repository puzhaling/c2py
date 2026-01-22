int sum_range(int start, int end) {
    int sum = 0;
    int i = start;
    
    while (i <= end) {
        sum = sum + i;
        i++;
    }
    
    return sum;
}

int main() {
    int result = 0;
    
    result = sum_range(1, 10);
    
    return result;
}
