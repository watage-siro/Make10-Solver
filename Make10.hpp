#pragma once

#include <vector>
#include <string>
#include <stack>
#include <algorithm>
#include <numeric>
#include <cstdint>
#include <cmath>

inline constexpr long long APP_LINE = (long long)1e15;

class Make10 {
private:
    enum class tkn : uint8_t {
        v0, v1, v2, v3, v4, v5, v6, v7,
        ADD, SUB, MUL, DIV
    };
    
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

        frac operator+(const frac& other) const{
            if (!this->valid || !other.valid) return frac::invalid();
            frac res((__int128_t)this->num * other.den + (__int128_t)other.num * this->den, (__int128_t)this->den * other.den);
            if(res.num >= APP_LINE || -res.num >= APP_LINE || res.den >= APP_LINE) res.approx();
            return res;
        }

        frac operator-() const{
            if (!this->valid) return frac::invalid();
            return frac(-(this->num), this->den);
        }

        frac operator-(const frac& other) const { return (*this) + (-other); }

        frac operator*(const frac& other) const {
            if (!this->valid || !other.valid) return frac::invalid();
            frac res((__int128_t)this->num * other.num, (__int128_t)this->den * other.den);
            if(res.num >= APP_LINE || -res.num >= APP_LINE || res.den >= APP_LINE) res.approx();
            return res;
        }

        frac operator/(const frac& other) const {
            if (!this->valid || !other.valid || other.num == 0) return frac::invalid();
            return (*this) * frac::inv(other);
        }

        bool operator==(const frac& other) const {
            if (!this->valid || !other.valid) return false;
            return this->num == other.num && this->den == other.den;
        }

        bool operator>(const frac& other) const {
            if (!this->valid || !other.valid) return false;
            return (__int128_t)this->num * other.den > (__int128_t)other.num * this->den;
        }
    };
    
    struct fml {
        uint64_t cul;
        uint8_t len;
        uint8_t min_idx;
        
        fml() = default;
        fml(tkn v) : cul(static_cast<uint64_t>(v)), len(1), min_idx(static_cast<uint8_t>(v)) {}
        fml(uint64_t cul_, uint8_t len_, uint8_t min_idx_) : cul(cul_), len(len_), min_idx(min_idx_) {}
        
    };
    
    struct op {
        tkn name;
        std::string s;
        int priority;
        bool commutative;
    };
    
    struct sig {
        uint64_t pn = 0;
        uint64_t ar = 0;
        uint8_t len = 0;
        uint64_t last_op_pn = 0;
        bool valid = false;
        
        sig() = default;
        sig(int id) : pn((uint64_t)id), ar(0), len(1), valid(true) {}
        
        static sig invalid() { return sig(); }
        
        tkn root() const {
            return static_cast<tkn>(pn & 0xF);
        }
        
        bool operator>(const sig& other) const {
            if(this->pn != other.pn) return this->pn > other.pn;
            if(this->len != other.len) return this->len > other.len;
            return this->ar > other.ar;
        }
    };
    
    struct node {
        frac v;
        fml f;
        sig s;
        
        node() = default;
        node(const frac& v_, const fml& f_, const sig& s_) : v(v_), f(f_), s(s_) {}
    };
    
    inline frac calc(const frac& a, const frac& b, const op& o) {
        switch (o.name) {
            case tkn::ADD:
                return a + b;
            case tkn::SUB:
                if(b.num == 0) return frac::invalid();
                return a - b;
            case tkn::MUL:
                return a * b; 
            case tkn::DIV:
                if(b.num == b.den) return frac::invalid();
                return a / b;
            default: return frac::invalid();
        }
    }
    
    inline fml merge_fml(const fml& a, const fml& b, const op& o) {
        uint64_t data = a.cul;
        data |= (b.cul << (a.len * 4));
        data |= (uint64_t)o.name << ((a.len + b.len) * 4);
        return fml(data, a.len + b.len + 1, std::min(a.min_idx, b.min_idx));
    }

    inline sig merge_sig(const sig& a, const sig& b, const op& o) {
        if (!a.valid || !b.valid) return sig::invalid();
        
        if (!is_value(b.root())) {
            const op& b_op = tkn2op(b.root());
            if (b_op.priority == o.priority) return sig::invalid();
        }
        
    if (o.commutative) {
        const uint64_t Acomp_pn = (a.root() == o.name) ? a.last_op_pn : a.pn;
        if (Acomp_pn > b.pn) return sig::invalid();
    }

    sig s;
    s.valid = true;
    s.len = 1 + a.len + b.len;
    s.last_op_pn = b.pn;

    s.pn = static_cast<uint64_t>(o.name);
    s.pn |= (a.pn << 4);
    s.pn |= (b.pn << (4 * (1 + a.len)));

    s.ar = 2;
    s.ar |= (a.ar << 4);
    s.ar |= (b.ar << (4 * (1 + a.len)));

    return s;
}

    inline bool merge_node(const node& a, const node& b, const op& o, node& nn) {
        nn.v = calc(a.v, b.v, o);
        if(!nn.v.valid) return false;
        
        nn.f = merge_fml(a.f, b.f, o);
        
        nn.s = merge_sig(a.s, b.s, o);
        if(!nn.s.valid) return false;
        
        return true;
    }
    
    inline bool is_value(const tkn t) const { return t < tkn::ADD; }
    
    inline op tkn2op(const tkn t) const { return ops[static_cast<size_t>(t) - static_cast<size_t>(tkn::ADD)]; }
    
    std::string build_string(const fml& f, const std::vector<int>& A) const {
            if (f.cul == 0 && f.len == 0) return "";
            
            std::stack<std::pair<std::string,int>> st;
            uint64_t data = f.cul;
            for (uint8_t i = 0; i < f.len; ++i) {
                tkn t = static_cast<tkn>(data & 0xF);
                data >>= 4;
                
                if (is_value(t)) st.push({std::to_string(A[static_cast<size_t>(t)]), 100});
                else {
                    if (st.size() < 2) return "";
                    auto [bs, bp] = st.top(); st.pop();
                    auto [as, ap] = st.top(); st.pop();

                    const op& t_op = tkn2op(t);
                    if(ap < t_op.priority)  as = "(" + as + ")";
                    if(t_op.priority > bp || (!t_op.commutative && t_op.priority == bp)) bs = "(" + bs + ")";
                    st.push({as + t_op.s + bs, t_op.priority});
                }
            }
            return (st.size() == 1 ? st.top().first : "");
        }
    
    
    std::vector<int> A;
    bool bench;
    uint64_t call, opc, eval;
    inline static const op ops[4] = {
        {tkn::ADD, "+", 1, true},
        {tkn::SUB, "-", 1, false},
        {tkn::MUL, "*", 2, true},
        {tkn::DIV, "/", 2, false}
    };

    
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
    void solve_ops(const std::vector<char>& rpn, const std::vector<int>& A, const frac& target,
                   std::stack<node>& st,
                   Emit&& emit, const std::vector<int>& ID,
                   int rpn_i = 0, int val_i = 0) {
        
        if (rpn_i == (int)rpn.size()) {
            if(bench) ++eval;
			st.top().v.approx();
            if (st.size() == 1 && st.top().v == target) emit(st.top().f);
            return;
        }
        
        if (rpn[rpn_i] == 'V') {
            st.push(node(frac(A[ID[val_i]]), fml(static_cast<tkn>(ID[val_i])), sig(ID[val_i])));
            
            solve_ops(rpn, A, target, st, emit, ID, rpn_i + 1, val_i + 1);
            
            st.pop();
        } else {
            if(bench) ++call;
            if (st.size() < 2) return;
            node r = st.top(); st.pop();
            node l = st.top(); st.pop();
            
            for (const op& o : ops) {
                node nn;
                if(!merge_node(l, r, o, nn)) continue;
                
                st.push(nn);
                solve_ops(rpn, A, target, st, emit, ID, rpn_i + 1, val_i);
                st.pop();
            }
            st.push(l); st.push(r);
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
            *out++ = build_string(f, A);
            ++cnt;
        };
        
        std::vector<int> ID(A.size());
        std::iota(ID.begin(), ID.end(), 0);
        do {
            for (auto& rpn : RPN) {
                std::stack<node> st;
                
                solve_ops(rpn, A, target, st, emit, ID);
            }
        } while (std::next_permutation(ID.begin(), ID.end()));
        
    return cnt;
}

public:
    explicit Make10(const std::vector<int>& vec) : A(vec) , bench(false) {}

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
		call += eval;
        std::vector<uint64_t> res = {call, opc, eval}; 
        return res;
    }
};
