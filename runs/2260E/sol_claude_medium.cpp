#include <bits/stdc++.h>
using namespace std;
static char s[300005];
int main(){
    int n,q;
    scanf("%d %d",&n,&q);
    scanf("%s",s+1);
    vector<int> z(n+1,0),d(n+1,0);
    for(int i=1;i<=n;i++){
        z[i]=z[i-1]+(s[i]=='0');
        d[i]=d[i-1]+(i>1&&s[i]!=s[i-1]);
    }
    while(q--){
        int l,r;
        scanf("%d %d",&l,&r);
        long long m=r-l+1,zz=z[r]-z[l-1],o=m-zz;
        long long p=1+d[r]-d[l];
        long long k;
        if(p==1) k=max(1LL,(m+1)/2);
        else{
            long long h=(s[l]==s[r])?(p-1)/2:p/2;
            k=max({h,(zz+1)/2,(o+1)/2,(m-h+2)/3,1LL});
        }
        printf("%lld\n",4*k-m);
    }
}
