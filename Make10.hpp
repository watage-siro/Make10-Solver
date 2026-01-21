#pragma once

#include <vector>
#include <string>
#include <stack>
#include <algorithm>
#include <numeric>
#include <unordered_set>
#include <cstdint>
#include <cmath>

class Make10 {
private:
    struct frac {
        long long num;
        long long den;
        bool valid;

        static frac invalid() {
            frac x;
            x.valid = false;
            return x;
        }

        void approx() {
            if (!valid) return;
            if (num == 0) {
                den = 1;
                return;
            }
            if (den == 0) {
                valid = false;
                return;
            }
            long long g = std::gcd(std::llabs(num), den);
            num /= g; den /= g;
        }

        frac(long long n = 0, long long d = 1) : valid(true) {
            if (d == 0) {
                valid = false;
                return;
            }
            if (d < 0) { n = -n; d = -d; }
            if (n == 0) { num = 0; den = 1; return;}
            num = n; den = d;
        }

        static frac inv(const frac& val) {
            if (!val.valid || val.num == 0) return frac::invalid();
            return frac(val.den, val.num);
        }

        friend frac operator+(const frac& a, const frac& b) {
            if (!a.valid || !b.valid) return frac::invalid();
            frac res((__int128_t)a.num * b.den + (__int128_t)b.num * a.den, (__int128_t)a.den * b.den);
            //res.approx();
            return res;
        }

        friend frac operator-(const frac& a) {
            if (!a.valid) return frac::invalid();
            return frac(-(a.num), a.den);
        }

        friend frac operator-(const frac& a, const frac& b) { return a + (-b); }

        friend frac operator*(const frac& a, const frac& b) {
            if (!a.valid || !b.valid) return frac::invalid();
            frac res((__int128_t)a.num * b.num, (__int128_t)a.den * b.den);
            //res.approx();
            return res;
        }

        friend frac operator/(const frac& a, const frac& b) {
            if (!a.valid || !b.valid || b.num == 0) return frac::invalid();
            return a * frac::inv(b);
        }

        friend bool operator==(const frac& a, const frac& b) {
            if (!a.valid || !b.valid) return false;
            return a.num == b.num && a.den == b.den;
        }

        friend bool operator>(const frac& a, const frac& b) {
            if (!a.valid || !b.valid) return false;
            return (__int128_t)a.num * b.den > (__int128_t)b.num * a.den;
        }
    };

    enum class tkn : uint8_t {
        v0, v1, v2, v3, v4, v5, v6, v7,
        ADD, SUB, MUL, DIV
    };

    struct op { tkn name; };

    struct fml {
        uint64_t cul = 0;
        uint8_t len = 0;

        fml() = default;
        fml(tkn v) : cul(static_cast<uint64_t>(v)), len(1) {}
        fml(uint64_t cul_, uint8_t len_) : cul(cul_), len(len_) {}

        static fml merge(const fml& a, const fml& b, const op& o) {
            uint64_t data = a.cul;
            data |= (b.cul << (a.len * 4));
            data |= (uint64_t)o.name << ((a.len + b.len) * 4);
            return fml(data, a.len + b.len + 1);
        }

        static inline bool is_value(tkn t) { return t <= tkn::v7; }

        static std::string tkn2str(tkn t, const std::vector<int>& A) {
            if (is_value(t)) return std::to_string(A[static_cast<size_t>(t)]);
            switch (t) {
                case tkn::ADD: return "+";
                case tkn::SUB: return "-";
                case tkn::MUL: return "*";
                case tkn::DIV: return "/";
                default: return "";
            }
        }

        static inline int get_priority(tkn t) {
            if (is_value(t)) return 100;
            if (t == tkn::MUL || t == tkn::DIV) return 2;
            if (t == tkn::ADD || t == tkn::SUB) return 1;
            return 0;
        }

        std::string build_string(const std::vector<int>& A) const {
            if (cul == 0 && len == 0) return "";
            struct Node {
                std::string s;
                tkn op;
                Node(tkn t, const std::vector<int>& val_A) : op(t) {
                    s = std::to_string(val_A[static_cast<int>(op)]);
                }
                Node(std::string str, tkn t) : s(std::move(str)), op(t) {}
            };

            std::stack<Node> st;
            uint64_t data = cul;
            for (uint8_t i = 0; i < len; ++i) {
                tkn t = static_cast<tkn>(data & 0xF);
                data >>= 4;

                if (is_value(t)) {
                    st.push(Node(t, A));
                } else {
                    if (st.size() < 2) return "";
                    Node b = st.top(); st.pop();
                    Node a = st.top(); st.pop();

                    int ap = get_priority(a.op), bp = get_priority(b.op), tp = get_priority(t);
                    if (ap < tp) a.s = "(" + a.s + ")";
                    if (bp < tp || (bp == tp && (t == tkn::SUB || t == tkn::DIV))) b.s = "(" + b.s + ")";
                    
                    st.push(Node(a.s + tkn2str(t, A) + b.s, t));
                }
            }
            return (st.size() == 1 ? st.top().s : "");
        }
    };

    struct StateKey {
        uint64_t fml_data;
        uint8_t len;
        long long num, den;

        StateKey(const fml& f, const frac& val = frac()) : fml_data(f.cul), len(f.len), num(val.num), den(val.den) {}

        bool operator==(const StateKey& other) const {
            return fml_data == other.fml_data && len == other.len && num == other.num && den == other.den;
        }

        struct Hasher {
            size_t operator()(const StateKey& k) const {
                size_t h = std::hash<uint64_t>{}(k.fml_data);
                h ^= std::hash<int>{}(k.len) + 0x9e3779b9 + (h << 6) + (h >> 2);
                h ^= std::hash<long long>{}(k.num) + 0x9e3779b9 + (h << 6) + (h >> 2);
                h ^= std::hash<long long>{}(k.den) + 0x9e3779b9 + (h << 6) + (h >> 2);
                return h;
            }
        };
    };

    std::vector<int> A;
    std::vector<op> ops;
    bool bench;
    uint64_t call, opc, eval;

    frac calc(const frac& a, const frac& b, const op& o) {
        switch (o.name) {
            case tkn::ADD:
                if(a > b) return frac::invalid();
                else return a + b;
            case tkn::SUB:
                if(b.den == 0) return frac::invalid();
                else return a - b;
            case tkn::MUL:
                if(a > b) return frac::invalid();
                else return a * b; 
            case tkn::DIV:
                if(b.num == b.den) return frac::invalid();
                else return a / b;
            default: return frac::invalid();
        }
    }

    void genRPN(std::vector<std::vector<char>>& RPN, std::vector<char>& cur, int val_cnt = 0, int op_cnt = 0) {
        int n = static_cast<int>(A.size());
        if (val_cnt == n && op_cnt == n - 1) {
            RPN.push_back(cur);
            return;
        }
        if (val_cnt < n) {
            cur.push_back('V');
            genRPN(RPN, cur, val_cnt + 1, op_cnt);
            cur.pop_back();
        }
        if (op_cnt < val_cnt - 1) {
            cur.push_back('O');
            genRPN(RPN, cur, val_cnt, op_cnt + 1);
            cur.pop_back();
        }
    }
    
    template<class Emit>
    void solve_ops(const std::vector<char>& rpn, const std::vector<int>& current_A, const frac& target,
                   std::vector<frac>& val_st, std::vector<fml>& fml_st,
                   std::unordered_set<StateKey, StateKey::Hasher>& seen,
                   Emit&& emit,
                   int rpn_i = 0, int val_i = 0) {
        //if(bench) ++call;
        if (rpn_i == (int)rpn.size()) {
            if(bench) ++eval;
            if(bench) ++call;
            if (val_st.size() == 1 && val_st.back() == target) {
                if (seen.insert(StateKey(fml_st.back())).second) {
                    emit(fml_st.back());
                }
            }
            return;
        }
        
        if (rpn[rpn_i] == 'V') {
            val_st.push_back(frac(current_A[val_i]));
            fml_st.push_back(fml(static_cast<tkn>(val_i)));
            solve_ops(rpn, current_A, target, val_st, fml_st, seen, emit, rpn_i + 1, val_i + 1);
            fml_st.pop_back();
            val_st.pop_back();
        } else {
            if(bench) ++call;
            if (val_st.size() < 2) return;
            frac b = val_st.back(); val_st.pop_back();
            frac a = val_st.back(); val_st.pop_back();
            fml b_fml = fml_st.back(); fml_st.pop_back();
            fml a_fml = fml_st.back(); fml_st.pop_back();
            
            for (const op& o : ops) {
                frac c = calc(a, b, o);
                if (!c.valid) continue;
                if(bench) ++opc;
                fml c_fml = fml::merge(a_fml, b_fml, o);
                val_st.push_back(c); fml_st.push_back(c_fml);
                solve_ops(rpn, current_A, target, val_st, fml_st, seen, emit, rpn_i + 1, val_i);
                fml_st.pop_back();
                val_st.pop_back();
            }
            fml_st.push_back(a_fml); fml_st.push_back(b_fml);
            val_st.push_back(a); val_st.push_back(b);
        }
    }
    
    template<class OutputIt>
    size_t solve(int x, OutputIt out) {
        if (A.empty()) return 0;
        if (A.size() > 8) return 0;
        
        std::vector<std::vector<char>> RPN;
        std::vector<char> cur_rpn;
        genRPN(RPN, cur_rpn);
        
        const frac target(x);
        std::sort(A.begin(), A.end());
    
        size_t cnt = 0;
        auto emit = [&](const fml& f) {
            *out++ = f.build_string(A);
            ++cnt;
        };
        
        do {
            for (auto& rpn : RPN) {
                std::vector<frac> val_st;
                std::vector<fml> fml_st;
                std::unordered_set<StateKey, StateKey::Hasher> seen;
                
                solve_ops(rpn, A, target, val_st, fml_st, seen, emit);
            }
        } while (std::next_permutation(A.begin(), A.end()));
        
    return cnt;
}

public:
    explicit Make10(const std::vector<int>& vec) : A(vec) , bench(false) {
        ops = { {tkn::ADD}, {tkn::SUB}, {tkn::MUL}, {tkn::DIV} };
    }

    std::vector<std::string> solve(int x = 10) {
        std::vector<std::string> res;
        solve(x, std::back_inserter(res));
        
        std::sort(res.begin(), res.end(), [](const std::string& a, const std::string& b) {
            return a.size() != b.size() ? a.size() < b.size() : a < b;
        });
        
        res.erase(std::unique(res.begin(), res.end()), res.end());
        return res;
    }

    std::vector<uint64_t> benchmark(int x = 10) {
        call = 0,opc = 0,eval = 0;
        bench = true;
        solve(x);
        bench = false;
        std::vector<uint64_t> res = {call, opc, eval}; 
        return res;
    }
};
