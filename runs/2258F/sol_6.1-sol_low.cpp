#include <bits/stdc++.h>
using namespace std;
using ll = long long;

struct Treap {
    struct Node {
        int l=0, r=0, cnt=0, sz=0;
        ll val=0, sum=0, lazy=0;
        uint32_t priority=0;
    };
    vector<Node> tr;
    mt19937 rng{712367821};
    Treap(int n) { tr.reserve(3*n+5); tr.push_back(Node{}); }
    int size(int x) const { return tr[x].sz; }
    int node(ll val, int cnt) {
        Node x; x.val=val; x.cnt=x.sz=cnt;
        x.sum=val*cnt; x.priority=rng();
        tr.push_back(x); return (int)tr.size()-1;
    }
    void add(int x, ll d) {
        if (!x) return;
        tr[x].val+=d; tr[x].sum+=d*tr[x].sz; tr[x].lazy+=d;
    }
    void push(int x) {
        if (tr[x].lazy) {
            add(tr[x].l,tr[x].lazy); add(tr[x].r,tr[x].lazy);
            tr[x].lazy=0;
        }
    }
    void pull(int x) {
        tr[x].sz=size(tr[x].l)+tr[x].cnt+size(tr[x].r);
        tr[x].sum=tr[tr[x].l].sum+tr[x].val*tr[x].cnt+tr[tr[x].r].sum;
    }
    int merge(int a, int b) {
        if (!a || !b) return a ? a : b;
        if (tr[a].priority>tr[b].priority) {
            push(a); tr[a].r=merge(tr[a].r,b); pull(a); return a;
        }
        push(b); tr[b].l=merge(a,tr[b].l); pull(b); return b;
    }
    // Split by value, with equality going left iff inclusive is true.
    void splitValue(int x, ll key, bool inclusive, int &a, int &b) {
        if (!x) { a=b=0; return; }
        push(x);
        if (tr[x].val<key || (inclusive && tr[x].val==key)) {
            a=x; splitValue(tr[x].r,key,inclusive,tr[x].r,b); pull(a);
        } else {
            b=x; splitValue(tr[x].l,key,inclusive,a,tr[x].l); pull(b);
        }
    }
    int unite(int a, int b) {
        if (!a || !b) return a ? a : b;
        if (tr[a].priority<tr[b].priority) swap(a,b);
        push(a);
        int lo, eq, hi, rest;
        splitValue(b,tr[a].val,false,lo,rest);
        splitValue(rest,tr[a].val,true,eq,hi);
        tr[a].cnt+=size(eq);
        tr[a].l=unite(tr[a].l,lo); tr[a].r=unite(tr[a].r,hi);
        pull(a); return a;
    }
    void splitRank(int x, int k, int &a, int &b) {
        if (!x) { a=b=0; return; }
        push(x);
        int left=size(tr[x].l);
        if (k<left) {
            int tail; splitRank(tr[x].l,k,a,tail);
            tr[x].l=0; pull(x); b=merge(tail,x);
        } else if (k>left+tr[x].cnt) {
            int head; splitRank(tr[x].r,k-left-tr[x].cnt,head,b);
            tr[x].r=0; pull(x); a=merge(x,head);
        } else if (k==left) {
            a=tr[x].l; tr[x].l=0; b=x; pull(b);
        } else if (k==left+tr[x].cnt) {
            b=tr[x].r; tr[x].r=0; a=x; pull(a);
        } else {
            int right=tr[x].r;
            int y=node(tr[x].val,tr[x].cnt-(k-left));
            tr[x].cnt=k-left; tr[x].r=0; pull(x);
            a=x; b=merge(y,right);
        }
    }
    ll negativeSum(int x) {
        if (!x) return 0;
        push(x);
        if (tr[x].val>=0) return negativeSum(tr[x].l);
        return tr[tr[x].l].sum+tr[x].val*tr[x].cnt+negativeSum(tr[x].r);
    }
};

int main() {
    ios::sync_with_stdio(false); cin.tie(nullptr);
    int t; cin>>t;
    while (t--) {
        int n; cin>>n;
        vector<int> a(n), parent(n,-1), order{0}, root(n), low(n);
        for (int &x:a) cin>>x;
        vector<vector<int>> g(n);
        for (int i=1,u,v;i<n;i++) {
            cin>>u>>v; --u; --v; g[u].push_back(v); g[v].push_back(u);
        }
        for (int i=0;i<n;i++) {
            int v=order[i];
            for (int u:g[v]) if (u!=parent[v]) {
                parent[u]=v; order.push_back(u);
            }
        }
        Treap dp(n);
        ll baseline=0;
        for (int i=n-1;i>=0;i--) {
            int v=order[i];
            low[v]=(a[v]==0 ? -1 : a[v]);
            int r=0;
            for (int u:g[v]) if (parent[u]==v) {
                low[v]+=low[u]; r=dp.unite(r,root[u]);
            }
            if (a[v]==0) r=dp.unite(r,dp.node(0,1));
            baseline+=abs(low[v]);
            int z=dp.size(r);
            int minus=min(z,max(0,-low[v]/2));
            int middle=(low[v]<0 && (-low[v])%2 && minus<z) ? 1 : 0;
            int p,q,s;
            dp.splitRank(r,minus,p,q);
            dp.splitRank(q,middle,q,s);
            dp.add(p,-2); dp.add(s,2);
            root[v]=dp.merge(dp.merge(p,q),s);
        }
        cout<<baseline+dp.negativeSum(root[0])<<'\n';
    }
}
