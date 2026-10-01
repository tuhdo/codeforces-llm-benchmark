#include <bits/stdc++.h>
using namespace std;
int K;
struct KE{int a,b;bool direct;};
vector<KE> ke;
vector<vector<pair<int,int>>> dadj;
bool found;
vector<int> path;
bool connectedWithout(){
    vector<char> rem(ke.size(),0);
    for(int e:path)rem[e]=1;
    vector<int> p(K);iota(p.begin(),p.end(),0);
    function<int(int)> f=[&](int x){return p[x]==x?x:p[x]=f(p[x]);};
    int c=K;
    for(size_t i=0;i<ke.size();i++)if(!rem[i]){int a=f(ke[i].a),b=f(ke[i].b);if(a!=b){p[a]=b;c--;}}
    return c==1;
}
void dfs(int s,int v,int mask){
    if(found)return;
    for(auto [w,e]:dadj[v]){
        if(w==s&&path.size()>=2){
            path.push_back(e);
            if(connectedWithout())found=true;
            path.pop_back();
            if(found)return;
        } else if(w>s&&!(mask>>w&1)){
            path.push_back(e);
            dfs(s,w,mask|1<<w);
            path.pop_back();
            if(found)return;
        }
    }
}
int main(){
    int t;scanf("%d",&t);
    while(t--){
        int n,m;scanf("%d %d",&n,&m);
        vector<vector<pair<int,int>>> adj(n);
        for(int i=0;i<m;i++){int u,v;scanf("%d %d",&u,&v);u--;v--;adj[u].push_back({v,i});adj[v].push_back({u,i});}
        vector<int> deg(n);vector<char> dead(n,0);
        queue<int> q;
        for(int i=0;i<n;i++){deg[i]=adj[i].size();if(deg[i]<=1)q.push(i);}
        while(!q.empty()){
            int x=q.front();q.pop();
            if(dead[x])continue;
            dead[x]=1;
            for(auto [y,e]:adj[x])if(!dead[y]){if(--deg[y]<=1)q.push(y);}
        }
        vector<int> id(n,-1);K=0;
        for(int i=0;i<n;i++)if(!dead[i]&&deg[i]>=3)id[i]=K++;
        if(K==0){puts("NO");continue;}
        ke.clear();dadj.assign(K,{});
        for(int a=0;a<n;a++)if(id[a]>=0){
            for(auto [x,e0]:adj[a]){
                if(dead[x])continue;
                int cur=x,le=e0,len=1;
                while(id[cur]<0){
                    for(auto [y,e]:adj[cur])if(!dead[y]&&e!=le){cur=y;le=e;break;}
                    len++;
                }
                int A=id[a],B=id[cur];
                if(A<B||(A==B&&e0<le)){
                    int idx=ke.size();
                    ke.push_back({A,B,len==1});
                    if(len==1){dadj[A].push_back({B,idx});dadj[B].push_back({A,idx});}
                }
            }
        }
        found=false;path.clear();
        for(int s=0;s<K&&!found;s++)dfs(s,s,1<<s);
        puts(found?"YES":"NO");
    }
}
