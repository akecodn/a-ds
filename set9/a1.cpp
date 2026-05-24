#include <iostream>
#include <string>
#include <vector>

int lcp_compare(const std::string &a, const std::string &b, int k, int &h) {
    int i = k;
    int n = a.size();
    int m = b.size();
    while (i < n && i < m && a[i] == b[i]) {
        i++;
    }

    h = i;
    if (i == n && i == m) {
        return 0;
    }
    if (i == n) {
        return -1;
    }
    if (i == m) {
        return 1;
    }
    if (a[i] < b[i]) {
        return -1;
    }
    return 1;
}

void merge_lcp(std::vector<std::string> &a, std::vector<int> &lcp, std::vector<std::string> &b, std::vector<int> &blcp, int l, int m, int r) {
    int i = l;
    int j = m + 1;
    int p = l;

    int ki = 0;
    int pj = 0;

    auto push_i = [&]() {
        b[p] = a[i];
        blcp[p] = (p == l ? 0 : ki);
        i++;
        p++;
        if (i <= m) {
            ki = lcp[i];
        }
    };

    auto push_j = [&]() {
        b[p] = a[j];
        blcp[p] = (p == l ? 0 : pj);
        j++;
        p++;
        if (j <= r) {
            pj = lcp[j];
        }
    };

    while (i <= m && j <= r) {
        if (ki < pj) {
            push_j();
            continue;
        }

        if (ki > pj) {
            push_i();
            continue;
        }

        int h = 0;
        int cmp = lcp_compare(a[i], a[j], ki, h);

        if (cmp <= 0) {
            push_i();
            pj = h;
        } else {
            push_j();
            ki = h;
        }
    }

    while (i <= m) {
        push_i();
    }

    while (j <= r) {
        push_j();
    }

    for (int t = l; t <= r; t++) {
        a[t] = b[t];
        lcp[t] = blcp[t];
    }
}

void sort_lcp(std::vector<std::string> &a, std::vector<int> &lcp, std::vector<std::string> &b, std::vector<int> &blcp, int l, int r) {
    int n = r - l + 1;
    if (n <= 1) {
        if (n == 1) {
            lcp[l] = 0;
        }
        return;
    }

    for (int i = l; i <= r; i++) {
        lcp[i] = 0;
    }

    for (int sz = 1; sz < n; sz += sz) {
        int step = sz + sz;
        for (int left = l; left + sz <= r; left += step) {
            int mid = left + sz - 1;
            int right = left + step - 1;
            if (right > r) {
                right = r;
            }
            merge_lcp(a, lcp, b, blcp, left, mid, right);
        }
    }
}

int main() {
    int n;
    std::cin >> n;
    std::cin.ignore();

    std::vector<std::string> a(n);
    for (int i = 0; i < n; i++) {
        std::getline(std::cin, a[i]);
    }

    if (n > 1) {
        std::vector<int> lcp(n, 0);
        std::vector<std::string> b(n);
        std::vector<int> blcp(n, 0);
        sort_lcp(a, lcp, b, blcp, 0, n - 1);
    }

    for (int i = 0; i < n; i++) {
        std::cout << a[i] << '\n';
    }

    return 0;
}
