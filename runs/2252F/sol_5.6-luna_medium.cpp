#include <bits/stdc++.h>
using namespace std;

using ll = long long;

struct SegTree {
    int n;
    vector<ll> cnt, sum;
    SegTree() {}
    SegTree(int n_) { init(n_); }
    void init(int n_) { n=n_; cnt.assign(4*n+4,0); sum.assign(4*n+4,0); }
    void add(int p, ll dc, ll ds, int v, int l, int r) {
        if (l==r) { cnt[v]+=dc; sum[v]+=ds; return; }
        int m=(l+r)>>1;
        if (p<=m) add(p,dc,ds,v<<1,l,m); else add(p,dc,ds,v<<1|1,m+1,r);
        cnt[v]=cnt[v<<1]+cnt[v<<1|1];
        sum[v]=sum[v<<1]+sum[v<<1|1];
    }
    void add(int p, ll amount) { if (amount) add(p,amount,amount*p,1,0,n-1); }

    // Sum of the largest k values after adding two point-multisets.
    ll top_with_adds(ll k, int a, ll ca, int b, ll cb, int v, int l, int r) const {
        if (k<=0) return 0;
        auto extra_count = [&](int ql, int qr)->ll {
            ll z=0;
            if (ql<=a && a<=qr) z+=ca;
            if (ql<=b && b<=qr) z+=cb;
            return z;
        };
        auto extra_sum = [&](int ql, int qr)->ll {
            ll z=0;
            if (ql<=a && a<=qr) z+=ca*a;
            if (ql<=b && b<=qr) z+=cb*b;
            return z;
        };
        ll ans=0;
        while (l<r) {
            int m=(l+r)>>1;
            ll rightCnt=cnt[v<<1|1]+extra_count(m+1,r);
            if (rightCnt>=k) {
                v=v<<1|1; l=m+1;
            } else {
                ans += sum[v<<1|1]+extra_sum(m+1,r);
                k-=rightCnt; v<<=1; r=m;
            }
        }
        return ans + k*l;
    }
    ll top_with_adds(ll k, int a, ll ca, int b, ll cb) const {
        if (k<=0) return 0;
        k=min(k,cnt[1]+ca+cb);
        return top_with_adds(k,a,ca,b,cb,1,0,n-1);
    }
    ll top(ll k) const {
        return top_with_adds(k,0,0,0,0);
    }
};

int main(){
    ios::sync_with_stdio(false); cin.tie(nullptr);
    int T; cin>>T;
    while(T--){
        int n; cin>>n;
        vector<int> col(n+1), kneed(n+1);
        vector<vector<int>> by(n+1);
        for(int i=1;i<=n;i++){ cin>>col[i]; by[col[i]].push_back(i); }
        for(int i=1;i<=n;i++) cin>>kneed[i];
        vector<vector<int>> g(n+1);
        for(int i=1,u,v;i<n;i++){cin>>u>>v; g[u].push_back(v); g[v].push_back(u);}

        int LOG=1; while((1<<LOG)<=n) ++LOG;
        vector<vector<int>> up(LOG, vector<int>(n+1));
        vector<int> tin(n+1), tout(n+1), dep(n+1), order; order.reserve(n);
        int timer=0;
        vector<pair<int,int>> st={{1,0}};
        up[0][1]=1;
        while(!st.empty()){
            int v=st.back().first, &it=st.back().second;
            if(it==0){ tin[v]=++timer; order.push_back(v); }
            if(it<(int)g[v].size()){
                int u=g[v][it++]; if(u==up[0][v]) continue;
                up[0][u]=v; dep[u]=dep[v]+1; st.push_back({u,0});
            }else{ tout[v]=timer; st.pop_back(); }
        }
        for(int j=1;j<LOG;j++) for(int v=1;v<=n;v++) up[j][v]=up[j-1][up[j-1][v]];
        auto isanc=[&](int a,int b){return tin[a]<=tin[b]&&tout[b]<=tout[a];};
        auto lca=[&](int a,int b){
            if(isanc(a,b)) return a; if(isanc(b,a)) return b;
            int x=a; for(int j=LOG-1;j>=0;j--) if(!isanc(up[j][x],b)) x=up[j][x];
            return up[0][x];
        };
        auto dist=[&](int a,int b){int z=lca(a,b); return dep[a]+dep[b]-2*dep[z];};
        vector<ll> answer(n+1,-1);

        for(int c=1;c<=n;c++) if(!by[c].empty()){
            int m=by[c].size();
            vector<int> nodes=by[c];
            sort(nodes.begin(),nodes.end(),[&](int a,int b){return tin[a]<tin[b];});
            int sz=nodes.size();
            for(int i=1;i<sz;i++) nodes.push_back(lca(nodes[i-1],nodes[i]));
            sort(nodes.begin(),nodes.end(),[&](int a,int b){return tin[a]<tin[b];});
            nodes.erase(unique(nodes.begin(),nodes.end()),nodes.end());
            int q=nodes.size();
            vector<vector<pair<int,int>>> ch(q);
            vector<int> stackv;
            for(int x=0;x<q;x++){
                while(!stackv.empty() && !isanc(nodes[stackv.back()],nodes[x])) stackv.pop_back();
                if(!stackv.empty()) ch[stackv.back()].push_back({x,dep[nodes[x]]-dep[nodes[stackv.back()]]});
                stackv.push_back(x);
            }
            vector<int> sub(q,0), marked(q,0);
            for(int i=0;i<q;i++) marked[i]= (col[nodes[i]]==c);
            for(int i=q-1;i>=0;i--){ sub[i]+=marked[i]; for(auto [j,len]:ch[i]) sub[i]+=sub[j]; }
            int root=0;
            // The first node in Euler order is the virtual-tree root.
            SegTree seg(m+1);
            ll totalEdges=0;
            for(int i=0;i<q;i++) for(auto [j,len]:ch[i]) { seg.add(sub[j],len); totalEdges+=len; }
            ll D=0;
            for(int v:by[c]) D+=dist(nodes[root],v);
            ll K=max(0LL,kneed[c]-1LL);
            auto evalNode=[&](ll curD)->ll { return curD-seg.top(K); };
            ll best=evalNode(D);

            auto evalSegment=[&](ll baseD,int len,int a)->ll{
                int b=m-a;
                seg.add(a,-(ll)len);
                auto f=[&](int x)->ll{
                    ll top=seg.top_with_adds(min(K,totalEdges),a,len-x,b,x);
                    return baseD+1LL*x*(m-2LL*a)-top;
                };
                ll res;
                if(b<=a){ res=min(f(0),f(len)); }
                else {
                    int lo=0,hi=len;
                    while(lo<hi){ int mid=lo+(hi-lo)/2; if(f(mid)<=f(mid+1)) hi=mid; else lo=mid+1; }
                    res=f(lo);
                }
                seg.add(a,len);
                return res;
            };
            function<void(int,ll)> dfs=[&](int v,ll curD){
                for(auto [u,len]:ch[v]){
                    int a=sub[u];
                    best=min(best,evalSegment(curD,len,a));
                    seg.add(a,-(ll)len); seg.add(m-a,len);
                    ll nd=curD+1LL*len*(m-2LL*a);
                    best=min(best,evalNode(nd));
                    dfs(u,nd);
                    seg.add(m-a,-(ll)len); seg.add(a,len);
                }
            };
            dfs(root,D);
            answer[c]=best;
        }
        for(int i=1;i<=n;i++) cout<<answer[i]<<(i==n?'\n':' ');
    }
}
