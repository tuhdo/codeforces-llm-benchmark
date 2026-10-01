#include <bits/stdc++.h>
using namespace std;
const int B=18;
static int ch[4000000][2],cnt[4000000];
int tot;
int G;
int M,Y;
int ins(int p,int v){
    int r=++tot;ch[r][0]=ch[p][0];ch[r][1]=ch[p][1];cnt[r]=cnt[p]+1;
    int c=r;
    for(int b=B-1;b>=0;b--){
        int d=v>>b&1;
        int o=ch[c][d];
        int nn=++tot;
        ch[nn][0]=ch[o][0];ch[nn][1]=ch[o][1];cnt[nn]=cnt[o]+1;
        ch[c][d]=nn;c=nn;
    }
    return r;
}
void dfs(int u,int v,int b,int cur){
    if(cnt[u]-cnt[v]<=0)return;
    if(b<0){if(cur>G)G=cur;return;}
    if(cur+(M&((2<<b)-1))<=G)return;
    int yb=Y>>b&1;
    if(M>>b&1){
        int d=yb^1;
        dfs(ch[u][d],ch[v][d],b-1,cur|(1<<b));
        dfs(ch[u][d^1],ch[v][d^1],b-1,cur);
    }else{
        dfs(ch[u][0],ch[v][0],b-1,cur);
        dfs(ch[u][1],ch[v][1],b-1,cur);
    }
}
int main(){
    int t;scanf("%d",&t);
    while(t--){
        int n;scanf("%d",&n);
        vector<int>a(n+2),P(n+1,0),root(n+2,0);
        for(int i=1;i<=n;i++){scanf("%d",&a[i]);P[i]=P[i-1]^a[i];}
        tot=0;ch[0][0]=ch[0][1]=0;cnt[0]=0;
        for(int j=0;j<=n;j++)root[j+1]=ins(root[j],P[j]);
        vector<int>lo(n+1),hi(n+1),st;
        for(int i=1;i<=n;i++){
            while(!st.empty()&&a[st.back()]<a[i])st.pop_back();
            lo[i]=st.empty()?0:st.back();
            st.push_back(i);
        }
        st.clear();
        for(int i=n;i>=1;i--){
            while(!st.empty()&&a[st.back()]<=a[i])st.pop_back();
            hi[i]=st.empty()?n+1:st.back();
            st.push_back(i);
        }
        vector<int>ord(n);iota(ord.begin(),ord.end(),1);
        sort(ord.begin(),ord.end(),[&](int x,int y){return a[x]>a[y];});
        G=0;
        auto query=[&](int xa,int xb,int y,int m){
            if(xa>xb)return;
            M=m;Y=y;
            dfs(root[xb+1],root[xa],B-1,0);
        };
        for(int i:ord){
            int m=a[i];
            if(m<=G)break;
            int l=lo[i],h=hi[i];
            if(i-l<=h-i){
                for(int j=l;j<=i-1;j++)
                    query(j==i-1?i+1:i,h-1,P[j],m);
            }else{
                for(int r=i;r<=h-1;r++)
                    query(l,r==i?i-2:i-1,P[r],m);
            }
        }
        printf("%d\n",G);
    }
}
