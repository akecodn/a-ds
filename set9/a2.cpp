#include <iostream>
#include <string>
#include <vector>

int ch(const std::string &s, int d) {
    int n = s.size();
    if (d >= n) {
        return 0;
    }
    return (unsigned char)s[d] + 1;
}

void sq(std::vector<std::string> &a, int l, int r, int d) {
    if (l >= r) {
        return;
    }
    int v = ch(a[(l + r) / 2], d);
    int lt = l;
    int gt = r;
    int i = l;
    while (i <= gt) {
        int t = ch(a[i], d);
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

    int left_r = lt - 1;
    int mid_l = lt;
    int mid_r = gt;
    int right_l = gt + 1;
    sq(a, l, left_r, d);
    sq(a, right_l, r, d);
    if (v != 0) {
        sq(a, mid_l, mid_r, d + 1);
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
        sq(a, 0, n - 1, 0);
    }

    for (int i = 0; i < n; i++) {
        std::cout << a[i] << '\n';
    }

    return 0;
}
