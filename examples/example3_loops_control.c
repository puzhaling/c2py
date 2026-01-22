int main() {
    int i = 0;
    int j = 0;
    int k = 10;

    for (i = 0; i < 5; i++) {
        j++;
        k--;
    }

    while (i < 10) {
        if (i == 7) {
            break;
        }
        if (i % 2 == 0) {
            i++;
            continue;
        }
        j = j + i;
        i++;
    }

    do {
        k = k + 2;
    } while (k < 20);

    return j;
}
