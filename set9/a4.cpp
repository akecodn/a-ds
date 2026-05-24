#include <iostream>
#include <string>
#include <vector>

int get_sym(const std::string &s, int d) {
    int n = s.size();
    if (d < n) {
        unsigned char c = s[d];
        return c + 1;
    }
    return 0;
}

void sort3(std::vector<std::string> &a, int l, int r, int d) {
    if (l >= r) {
        return;
    }
    int v = get_sym(a[(l + r) / 2], d);
    int lt = l;
    int gt = r;
    int i = l;
    while (i <= gt) {
        int t = get_sym(a[i], d);
        if (t < v) {
            std::swap(a[lt], a[i]);
            lt++;
            i++;
        } else if (t > v) {
            std::swap(a[i], a[gt]);
            gt--;
        } else {
            i++;
        }
    }

    sort3(a, l, lt - 1, d);
    if (v > 0) {
        sort3(a, lt, gt, d + 1);
    }
    sort3(a, gt + 1, r, d);
}

void radix_msd(std::vector<std::string> &a, std::vector<std::string> &b, int l, int r, int d) {
    if (l >= r) {
        return;
    }

    int len = r - l + 1;
    if (len < 74) {
        sort3(a, l, r, d);
        return;
    }

    const int R = 257;
    int cnt[R + 1];
    for (int i = 0; i <= R; i++) {
        cnt[i] = 0;
    }

    for (int i = l; i <= r; i++) {
        int x = get_sym(a[i], d);
        cnt[x + 1]++;
    }

    for (int i = 1; i <= R; i++) {
        cnt[i] += cnt[i - 1];
    }

    int st[R];
    int en[R];
    int pos[R];
    for (int i = 0; i < R; i++) {
        st[i] = l + cnt[i];
        en[i] = l + cnt[i + 1] - 1;
        pos[i] = st[i];
    }

    for (int i = l; i <= r; i++) {
        int x = get_sym(a[i], d);
        b[pos[x]] = a[i];
        pos[x]++;
    }

    for (int i = l; i <= r; i++) {
        a[i] = b[i];
    }

    for (int x = 1; x < R; x++) {
        if (st[x] < en[x]) {
            radix_msd(a, b, st[x], en[x], d + 1);
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
        std::vector<std::string> b(n);
        radix_msd(a, b, 0, n - 1, 0);
    }
    for (int i = 0; i < n; i++) {
        std::cout << a[i] << '\n';
    }

    return 0;
}
