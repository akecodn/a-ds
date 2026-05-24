#include <algorithm>
#include <chrono>
#include <fstream>
#include <random>
#include <string>
#include <vector>

struct RunRes {
    long long t_ns;
    long long c_cmp;
};

class StringGenerator {
public:
    StringGenerator(unsigned int seed) : rng(seed) {}
    std::vector<std::string> make_random_base(int n) {
        std::vector<std::string> data;
        data.reserve(n);
        for (int i = 0; i < n; ++i) {
            data.push_back(make_str(10, 200));
        }
        return data;
    }

    std::vector<std::string> make_reverse_base(int n) {
        std::vector<std::string> data = make_random_base(n);
        std::sort(data.begin(), data.end());
        std::reverse(data.begin(), data.end());
        return data;
    }

    std::vector<std::string> make_almost_base(int n) {
        std::vector<std::string> data = make_random_base(n);
        std::sort(data.begin(), data.end());

        int swaps = n / 50;
        if (swaps < 1) {
            swaps = 1;
        }
        for (int i = 0; i < swaps; ++i) {
            int x = rnd(0, n - 1);
            int y = rnd(0, n - 1);
            std::swap(data[x], data[y]);
        }
        return data;
    }

    std::vector<std::string> make_prefix_base(int n) {
        std::vector<std::string> data(n);
        const int prefix_len = 20;
        const std::string prefix = make_str(prefix_len, prefix_len);

        for (int i = 0; i < n; ++i) {
            int length = rnd(10, 200);
            int suffix_len = length - prefix_len;
            if (suffix_len < 1) {
                suffix_len = 1;
            }
            data[i] = prefix + make_str(suffix_len, suffix_len);
        }
        return data;
    }

    std::vector<std::string> cut(const std::vector<std::string> &base, int n) {
        return {base.begin(), base.begin() + n};
    }

private:
    std::mt19937 rng;
    const std::string ab =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        "!@#%:;^&*()-.";

    int rnd(int l, int r) {
        std::uniform_int_distribution<int> dist(l, r);
        return dist(rng);
    }

    std::string make_str(int l, int r) {
        int n = rnd(l, r);
        std::string s;
        s.reserve(n);
        for (int i = 0; i < n; i++) {
            int p = rnd(0, (int)ab.size() - 1);
            s += ab[p];
        }
        return s;
    }
};

class StringSortTester {
public:
    StringSortTester() {
        for (int i = 0; i < 256; i++) {
            mp[i] = 0;
        }
        std::string ab =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789"
            "!@#%:;^&*()-.";
        for (int i = 0; i < (int)ab.size(); i++) {
            unsigned char c = ab[i];
            mp[c] = i + 1;
        }
    }

    RunRes run_quick_std(const std::vector<std::string> &src) {
        c_cmp = 0;
        std::vector<std::string> a = src;
        auto st = std::chrono::steady_clock::now();
        if (!a.empty()) {
            q_std(a, 0, (int)a.size() - 1);
        }
        auto en = std::chrono::steady_clock::now();
        return {dur_ns(st, en), c_cmp};
    }

    RunRes run_merge_std(const std::vector<std::string> &src) {
        c_cmp = 0;
        std::vector<std::string> a = src;
        std::vector<std::string> b(a.size());
        auto st = std::chrono::steady_clock::now();
        if (!a.empty()) {
            m_std(a, b, 0, (int)a.size() - 1);
        }
        auto en = std::chrono::steady_clock::now();
        return {dur_ns(st, en), c_cmp};
    }

    RunRes run_merge_lcp(const std::vector<std::string> &src) {
        c_cmp = 0;
        std::vector<std::string> a = src;
        std::vector<std::string> b(a.size());
        std::vector<int> lcp(a.size(), 0);
        std::vector<int> blcp(a.size(), 0);
        auto st = std::chrono::steady_clock::now();
        if (!a.empty()) {
            sort_lcp(a, lcp, b, blcp, 0, (int)a.size() - 1);
        }
        auto en = std::chrono::steady_clock::now();
        return {dur_ns(st, en), c_cmp};
    }

    RunRes run_string_quick3(const std::vector<std::string> &src) {
        c_cmp = 0;
        std::vector<std::string> a = src;
        auto st = std::chrono::steady_clock::now();
        if (!a.empty()) {
            sq3(a, 0, (int)a.size() - 1, 0);
        }
        auto en = std::chrono::steady_clock::now();
        return {dur_ns(st, en), c_cmp};
    }

    RunRes run_msd_plain(const std::vector<std::string> &src) {
        c_cmp = 0;
        std::vector<std::string> a = src;
        std::vector<std::string> b(a.size());
        auto st = std::chrono::steady_clock::now();
        if (!a.empty()) {
            msd_plain(a, b, 0, (int)a.size() - 1, 0);
        }
        auto en = std::chrono::steady_clock::now();
        return {dur_ns(st, en), c_cmp};
    }

    RunRes run_msd_switch(const std::vector<std::string> &src) {
        c_cmp = 0;
        std::vector<std::string> a = src;
        std::vector<std::string> b(a.size());
        auto st = std::chrono::steady_clock::now();
        if (!a.empty()) {
            msd_sw(a, b, 0, (int)a.size() - 1, 0);
        }
        auto en = std::chrono::steady_clock::now();
        return {dur_ns(st, en), c_cmp};
    }

private:
    long long c_cmp = 0;
    int mp[256];

    long long dur_ns(std::chrono::steady_clock::time_point st, std::chrono::steady_clock::time_point en) {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(en - st).count();
    }

    int cmp_str(const std::string &a, const std::string &b) {
        int i = 0;
        int n = a.size();
        int m = b.size();
        while (i < n && i < m) {
            c_cmp++;
            if (a[i] != b[i]) {
                if (a[i] < b[i]) {
                    return -1;
                }
                return 1;
            }
            i++;
        }
        if (n == m) {
            return 0;
        }
        if (n < m) {
            return -1;
        }
        return 1;
    }

    int lcp_cmp(const std::string &a, const std::string &b, int k, int &h) {
        int i = k;
        int n = a.size();
        int m = b.size();
        while (i < n && i < m) {
            c_cmp++;
            if (a[i] != b[i]) {
                h = i;
                if (a[i] < b[i]) {
                    return -1;
                }
                return 1;
            }
            i++;
        }
        h = i;
        if (i == n && i == m) {
            return 0;
        }
        if (i == n) {
            return -1;
        }
        return 1;
    }

    int sym(const std::string &s, int d) {
        int n = s.size();
        if (d < n) {
            unsigned char c = s[d];
            return mp[c];
        }
        return 0;
    }

    int sym_bucket(const std::string &s, int d) {
        c_cmp++;
        return sym(s, d);
    }

    void q_std(std::vector<std::string> &a, int l, int r) {
        int i = l;
        int j = r;
        std::string p = a[(l + r) / 2];
        while (i <= j) {
            while (cmp_str(a[i], p) < 0) {
                i++;
            }
            while (cmp_str(a[j], p) > 0) {
                j--;
            }
            if (i <= j) {
                std::swap(a[i], a[j]);
                i++;
                j--;
            }
        }
        if (l < j) {
            q_std(a, l, j);
        }
        if (i < r) {
            q_std(a, i, r);
        }
    }

    void m_std(std::vector<std::string> &a, std::vector<std::string> &b, int l, int r) {
        if (l >= r) {
            return;
        }
        int m = (l + r) / 2;
        m_std(a, b, l, m);
        m_std(a, b, m + 1, r);

        int i = l;
        int j = m + 1;
        int p = l;
        while (i <= m && j <= r) {
            if (cmp_str(a[i], a[j]) <= 0) {
                b[p] = a[i];
                i++;
            } else {
                b[p] = a[j];
                j++;
            }
            p++;
        }
        while (i <= m) {
            b[p] = a[i];
            i++;
            p++;
        }
        while (j <= r) {
            b[p] = a[j];
            j++;
            p++;
        }
        for (int t = l; t <= r; t++) {
            a[t] = b[t];
        }
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
            int cmp = lcp_cmp(a[i], a[j], ki, h);
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

    void sq3(std::vector<std::string> &a, int l, int r, int d) {
        if (l >= r) {
            return;
        }

        int v = sym(a[(l + r) / 2], d);
        int lt = l;
        int gt = r;
        int i = l;

        while (i <= gt) {
            int t = sym(a[i], d);
            c_cmp++;
            if (t < v) {
                std::swap(a[lt], a[i]);
                lt++;
                i++;
                continue;
            }
            c_cmp++;
            if (t > v) {
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

        sq3(a, l, left_r, d);
        sq3(a, right_l, r, d);
        if (v != 0) {
            sq3(a, mid_l, mid_r, d + 1);
        }
    }

    void msd_plain(std::vector<std::string> &v, std::vector<std::string> &u, int lo, int hi, int p) {
        int len = hi - lo + 1;
        if (len <= 1) {
            return;
        }

        const int k = 76;
        int cnt[k + 1] = {0};

        for (int i = lo; i <= hi; i++) {
            int z = sym_bucket(v[i], p);
            cnt[z + 1]++;
        }

        for (int i = 1; i <= k; i++) {
            cnt[i] += cnt[i - 1];
        }

        int beg[k];
        int end[k];
        int cur[k];
        for (int i = 0; i < k; i++) {
            beg[i] = lo + cnt[i];
            end[i] = lo + cnt[i + 1] - 1;
            cur[i] = beg[i];
        }

        for (int i = lo; i <= hi; i++) {
            int z = sym_bucket(v[i], p);
            u[cur[z]] = v[i];
            cur[z]++;
        }

        for (int i = lo; i <= hi; i++) {
            v[i] = u[i];
        }

        for (int z = 1; z < k; z++) {
            if (beg[z] < end[z]) {
                msd_plain(v, u, beg[z], end[z], p + 1);
            }
        }
    }

    void msd_sw(std::vector<std::string> &v, std::vector<std::string> &u, int lo, int hi, int p) {
        int len = hi - lo + 1;
        if (len <= 1) {
            return;
        }
        if (len < 74) {
            sq3(v, lo, hi, p);
            return;
        }

        const int k = 76;
        int cnt[k + 1] = {0};

        for (int i = lo; i <= hi; i++) {
            int z = sym_bucket(v[i], p);
            cnt[z + 1]++;
        }

        for (int i = 1; i <= k; i++) {
            cnt[i] += cnt[i - 1];
        }

        int beg[k];
        int end[k];
        int cur[k];
        for (int i = 0; i < k; i++) {
            beg[i] = lo + cnt[i];
            end[i] = lo + cnt[i + 1] - 1;
            cur[i] = beg[i];
        }

        for (int i = lo; i <= hi; i++) {
            int z = sym_bucket(v[i], p);
            u[cur[z]] = v[i];
            cur[z]++;
        }

        for (int i = lo; i <= hi; i++) {
            v[i] = u[i];
        }

        for (int z = 1; z < k; z++) {
            if (beg[z] < end[z]) {
                msd_sw(v, u, beg[z], end[z], p + 1);
            }
        }
    }
};

int main(int argc, char **argv) {
    int runs = 7;
    unsigned int seed = 42;
    std::string out_path = "set9/research_results.csv";
    int max_n = 3000;
    int step_n = 100;
    int use_prefix = 0;

    if (argc >= 2) {
        runs = std::stoi(argv[1]);
    }
    if (argc >= 3) {
        seed = (unsigned int)std::stoul(argv[2]);
    }
    if (argc >= 4) {
        out_path = argv[3];
    }
    if (argc >= 5) {
        max_n = std::stoi(argv[4]);
    }
    if (argc >= 6) {
        step_n = std::stoi(argv[5]);
    }
    if (argc >= 7) {
        use_prefix = std::stoi(argv[6]);
    }

    StringGenerator gen(seed);
    StringSortTester tester;

    std::ofstream out(out_path);
    if (!out.is_open()) {
        return 1;
    }

    out << "run,data_type,n,algo,time_ns,char_cmp\n";

    if (max_n < 100) {
        max_n = 100;
    }
    if (max_n > 3000) {
        max_n = 3000;
    }
    if (step_n < 1) {
        step_n = 1;
    }

    for (int run = 1; run <= runs; run++) {
        std::vector<std::string> base_rnd = gen.make_random_base(max_n);
        std::vector<std::string> base_rev = gen.make_reverse_base(max_n);
        std::vector<std::string> base_alm = gen.make_almost_base(max_n);
        std::vector<std::string> base_pref;
        if (use_prefix) {
            base_pref = gen.make_prefix_base(max_n);
        }

        for (int n = 100; n <= max_n; n += step_n) {
            std::vector<std::pair<std::string, std::vector<std::string> > > cases;
            cases.push_back({"random", gen.cut(base_rnd, n)});
            cases.push_back({"reverse", gen.cut(base_rev, n)});
            cases.push_back({"almost", gen.cut(base_alm, n)});
            if (use_prefix) {
                cases.push_back({"prefix", gen.cut(base_pref, n)});
            }

            for (auto &cs : cases) {
                const std::string &tp = cs.first;
                const std::vector<std::string> &arr = cs.second;

                RunRes r1 = tester.run_quick_std(arr);
                out << run << ',' << tp << ',' << n << ",quick_std," << r1.t_ns << ',' << r1.c_cmp << '\n';

                RunRes r2 = tester.run_merge_std(arr);
                out << run << ',' << tp << ',' << n << ",merge_std," << r2.t_ns << ',' << r2.c_cmp << '\n';

                RunRes r3 = tester.run_merge_lcp(arr);
                out << run << ',' << tp << ',' << n << ",merge_lcp," << r3.t_ns << ',' << r3.c_cmp << '\n';

                RunRes r4 = tester.run_string_quick3(arr);
                out << run << ',' << tp << ',' << n << ",string_quick3," << r4.t_ns << ',' << r4.c_cmp << '\n';

                RunRes r5 = tester.run_msd_plain(arr);
                out << run << ',' << tp << ',' << n << ",msd_plain," << r5.t_ns << ',' << r5.c_cmp << '\n';

                RunRes r6 = tester.run_msd_switch(arr);
                out << run << ',' << tp << ',' << n << ",msd_switch," << r6.t_ns << ',' << r6.c_cmp << '\n';
            }
        }

    }

    return 0;
}
