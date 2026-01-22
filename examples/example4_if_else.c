int main() {
    int x = 15;
    int y = 10;
    int result = 0;

    if (x > y) {
        result = x - y;
    }
    else if (x == y) {
        result = 0;
    }
    else {
        result = y - x;
    }

    if (result > 5) {
        result = result * 2;
    }
    else if (result > 0) {
        result = result + 10;
    }
    else {
        result = 0;
    }

    return result;
}
