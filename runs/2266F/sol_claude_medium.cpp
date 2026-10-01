#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
int main(){
    int t;scanf("%d",&t);
    while(t--){
        int n;scanf("%d",&n);
        int S=n+70;
        vector<ll> c(S,0);
        ll N=0,mx=0;
        for(int i=0;i<n;i++){
            ll x,y;scanf("%lld %lld",&x,&y);
            N+=y;mx=max(mx,x);
            if(x<S)c[x]=y;
        }
        auto ok=[&](int m){
            if(m<=1)return true;
            ll D=0,used=0;
            for(int v=m-1;v>=2;v--){
                ll r=1+D;
                used+=min(c[v],r);
                D+=max(0LL,r-c[v]);
                if(D>N)return false;
            }
            return 2*(1+D)<=N-used;
        };
        int lo=0,hi=S-1;
        while(lo<hi){
            int mid=(lo+hi+1)/2;
            if(ok(mid))lo=mid;else hi=mid-1;
        }
        printf("%lld\n",max<ll>(mx,lo));
    }
}
