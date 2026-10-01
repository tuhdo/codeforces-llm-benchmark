#include <bits/stdc++.h>
using namespace std;
int main(){
    const int M=200001;
    vector<int> spf(M+1,0);
    for(int i=2;i<=M;i++) if(!spf[i]) for(int j=i;j<=M;j+=i) if(!spf[j]) spf[j]=i;
    int t; scanf("%d",&t);
    while(t--){
        int n,k; scanf("%d %d",&n,&k);
        vector<int> a(n);
        for(auto&x:a) scanf("%d",&x);
        vector<long long> g(n+1,0);
        for(int x=k+1;x<=n;x++){
            long long best=LLONG_MAX;
            int y=x;
            while(y>1){
                int p=spf[y];
                while(y%p==0) y/=p;
                best=min(best,1+(long long)p*g[x/p]);
            }
            g[x]=best;
        }
        long long s=0;
        for(int x:a) s+=g[x];
        printf("%lld\n",s);
    }
}
