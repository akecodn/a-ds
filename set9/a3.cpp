#include <iostream>
#include <string>
#include <vector>

int ch(const std::string &t, int p) {
    int m = t.size();
    if (p >= m) {
        return 0;
    }
    unsigned char c = t[p];
    return c + 1;
}

void msd(std::vector<std::string> &v, std::vector<std::string> &u, int lo, int hi, int p) {
    int len = hi - lo + 1;
    if (len <= 1) {
        return;
    }
    int cnt[257 + 1] = {0};
    for (int i = lo; i <= hi; i++) {
        int z = ch(v[i], p);
        cnt[z + 1]++;
    }

    for (int i = 1; i <= 257; i++) {
        cnt[i] += cnt[i - 1];
    }

    int beg[257];
    int end[257];
    int cur[257];
    for (int i = 0; i < 257; i++) {
        beg[i] = lo + cnt[i];
        end[i] = lo + cnt[i + 1] - 1;
        cur[i] = beg[i];
    }

    for (int i = lo; i <= hi; i++) {
        int z = ch(v[i], p);
        u[cur[z]] = v[i];
        cur[z]++;
    }

    for (int i = lo; i <= hi; i++) {
        v[i] = u[i];
    }

    for (int z = 1; z < 257; z++) {
        if (beg[z] < end[z]) {
            msd(v, u, beg[z], end[z], p + 1);
        }
    }
}

int main() {
    int n;
    std::cin >> n;
    std::cin.ignore();
    std::vector<std::string> v(n);
    for (int i = 0; i < n; i++) {
        std::getline(std::cin, v[i]);
    }

    if (n > 1) {
        std::vector<std::string> u(n);
        msd(v, u, 0, n - 1, 0);
    }
    for (int i = 0; i < n; i++) {
        std::cout << v[i] << '\n';
    }

    return 0;
}
