#include <bits/stdc++.h>
using namespace std;

struct Tree {
    int n, LOG;
    vector<vector<int>> g, up;
    vector<int> tin, depth, it;
    Tree(int n): n(n), g(n), tin(n), depth(n) {
        LOG = 1; while ((1 << LOG) <= n) ++LOG;
        up.assign(LOG, vector<int>(n));
    }
    void addEdge(int a, int b) { g[a].push_back(b); g[b].push_back(a); }
    void build() {
        int timer=0;
        vector<int> par(n,-1), st{0}; par[0]=0;
        while (!st.empty()) {
            int v=st.back();
            if (it.size() < st.size()) { it.push_back(0); tin[v]=timer++; up[0][v]=par[v]; }
            if (it.back() == (int)g[v].size()) { st.pop_back(); it.pop_back(); continue; }
            int u=g[v][it.back()++]; if (u==par[v]) continue;
            par[u]=v; depth[u]=depth[v]+1; st.push_back(u);
        }
        for(int j=1;j<LOG;j++) for(int v=0;v<n;v++) up[j][v]=up[j-1][up[j-1][v]];
    }
    int lift(int v,int d) const { for(int j=0;j<LOG;j++) if(d>>j&1) v=up[j][v]; return v; }
    int lca(int a,int b) const {
        if(depth[a]<depth[b]) swap(a,b); a=lift(a,depth[a]-depth[b]);
        if(a==b) return a;
        for(int j=LOG-1;j>=0;j--) if(up[j][a]!=up[j][b]) a=up[j][a],b=up[j][b];
        return up[0][a];
    }
    int dist(int a,int b) const { int z=lca(a,b); return depth[a]+depth[b]-2*depth[z]; }
};

int main(){
    ios::sync_with_stdio(false); cin.tie(nullptr);
    int T; if(!(cin>>T)) return 0;
    while(T--){
        int n; cin>>n; vector<int> color(n), k(n+1); vector<vector<int>> occ(n+1);
        for(int &x:color) cin>>x;
        for(int i=1;i<=n;i++) cin>>k[i];
        for(int i=0;i<n;i++) occ[color[i]].push_back(i);
        Tree tr(n); for(int i=1,a,b;i<n;i++){cin>>a>>b;tr.addEdge(--a,--b);} tr.build();
        vector<long long> ans(n+1,-1);
        for(int c=1;c<=n;c++) if(!occ[c].empty()){
            auto marks=occ[c]; int m=marks.size();
            vector<int> nodes=marks;
            sort(nodes.begin(),nodes.end(),[&](int a,int b){return tr.tin[a]<tr.tin[b];});
            int z=nodes.size(); for(int i=1;i<z;i++) nodes.push_back(tr.lca(nodes[i-1],nodes[i]));
            sort(nodes.begin(),nodes.end(),[&](int a,int b){return tr.tin[a]<tr.tin[b];});
            nodes.erase(unique(nodes.begin(),nodes.end()),nodes.end());
            int q=nodes.size(); vector<vector<pair<int,int>>> vg(q);
            vector<int> st;
            for(int i=0;i<q;i++){
                while(!st.empty() && tr.lca(nodes[st.back()],nodes[i])!=nodes[st.back()]) st.pop_back();
                if(!st.empty()) { int p=st.back(); vg[p].push_back({i,tr.dist(nodes[p],nodes[i])}); vg[i].push_back({p,tr.dist(nodes[p],nodes[i])}); }
                st.push_back(i);
            }
            unordered_map<int,int> idx; idx.reserve(q*2); for(int i=0;i<q;i++) idx[nodes[i]]=i;
            vector<int> weight(q); for(int v:marks) weight[idx[v]]++;
            int base=0; vector<int> par(q,-1), order{base}; par[base]=base;
            for(int p=0;p<(int)order.size();p++){int v=order[p]; for(auto [u,d]:vg[v]) if(par[u]<0) par[u]=v,order.push_back(u);}
            vector<int> sub=weight;
            for(int i=q-1;i>0;i--) sub[par[order[i]]]+=sub[order[i]];
            int med=base;
            for(int v=0;v<q;v++){
                int mx=m-sub[v];
                for(auto [u,d]:vg[v]) if(par[u]==v) mx=max(mx,sub[u]);
                if(mx*2<=m){med=v;break;}
            }
            vector<tuple<int,int,int>> walk{{med,-1,0}}; vector<pair<int,int>> gains; long long distSum=0;
            while(!walk.empty()){
                auto [v,p,d0]=walk.back(); walk.pop_back();
                distSum += 1LL*weight[v]*d0;
                for(auto [u,len]:vg[v]) if(u!=p){
                    // sub[u] is from the arbitrary base rooting; convert to the side away from med.
                    int side;
                    if(par[u]==v) side=sub[u];
                    else side=m-sub[v];
                    if(side>0) gains.push_back({side,len});
                    walk.push_back({u,v,d0+len});
                }
            }
            sort(gains.begin(),gains.end(),greater<>());
            long long cost=distSum; int need=k[c]-1;
            for(auto [cnt,len]:gains){ long long take=min<long long>(need,len); cost-=take*cnt; need-=take; if(!need) break; }
            ans[c]=cost;
        }
        for(int c=1;c<=n;c++) cout<<ans[c]<<(c==n?'\n':' ');
    }
}
