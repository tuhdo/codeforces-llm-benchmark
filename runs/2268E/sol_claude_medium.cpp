#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll M=998244353;
ll pw(ll a,ll e){ll r=1;a%=M;while(e){if(e&1)r=r*a%M;a=a*a%M;e>>=1;}return r;}
void ntt(vector<ll>&a,bool inv){
    int n=a.size();
    for(int i=1,j=0;i<n;i++){int bit=n>>1;for(;j&bit;bit>>=1)j^=bit;j^=bit;if(i<j)swap(a[i],a[j]);}
    for(int len=2;len<=n;len<<=1){
        ll w=pw(3,(M-1)/len);if(inv)w=pw(w,M-2);
        vector<ll> ws(len/2);ws[0]=1;for(int i=1;i<len/2;i++)ws[i]=ws[i-1]*w%M;
        for(int i=0;i<n;i+=len)for(int j=0;j<len/2;j++){
            ll u=a[i+j],v=a[i+j+len/2]*ws[j]%M;
            a[i+j]=u+v>=M?u+v-M:u+v;a[i+j+len/2]=u-v<0?u-v+M:u-v;}
    }
    if(inv){ll ni=pw(n,M-2);for(auto&x:a)x=x*ni%M;}
}
int main(){
    int MX=400010;
    vector<ll> f(MX),fi(MX);f[0]=1;for(int i=1;i<MX;i++)f[i]=f[i-1]*i%M;
    fi[MX-1]=pw(f[MX-1],M-2);for(int i=MX-1;i>0;i--)fi[i-1]=fi[i]*i%M;
    auto cat=[&](int n){return f[2*n]*fi[n]%M*fi[n+1]%M;};
    int t;scanf("%d",&t);
    while(t--){
        int n;scanf("%d",&n);
        vector<int>P(n+1,0);
        for(int i=1;i<=n;i++){int x;scanf("%d",&x);P[i]=P[i-1]^x;}
        if(n==1){puts("0");continue;}
        int T=P[n];
        int sz=1;while(sz<2*(n+1))sz<<=1;
        vector<ll> G(n+1,0);
        for(int b=0;b<18;b++){
            ll wb=(1LL<<b)%M;
            vector<int> pre(n+2,0);
            for(int i=0;i<=n;i++)pre[i+1]=pre[i]+((P[i]>>b)&1);
            if((T>>b)&1){
                for(int d=1;d<n;d++)G[d]=(G[d]+wb*(n-d+1))%M;
            }else{
                if(pre[n+1]==0)continue;
                vector<ll> A(sz,0),B(sz,0);
                for(int i=0;i<=n;i++){A[i]=(P[i]>>b)&1;B[n-i]=(P[i]>>b)&1;}
                ntt(A,false);ntt(B,false);
                for(int i=0;i<sz;i++)A[i]=A[i]*B[i]%M;
                ntt(A,true);
                for(int d=1;d<n;d++){
                    ll diff=(ll)pre[n-d+1]+pre[n+1]-pre[d]-2*A[n-d];
                    G[d]=(G[d]+wb*2%M*(diff%M))%M;
                }
            }
        }
        ll ans=0;
        for(int d=1;d<n;d++)ans=(ans+cat(d)*cat(n-d)%M*G[d])%M;
        printf("%lld\n",ans);
    }
}
