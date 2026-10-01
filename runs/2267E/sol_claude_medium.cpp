#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
int main(){
    int t;
    scanf("%d",&t);
    while(t--){
        int n,q;
        scanf("%d %d",&n,&q);
        static char buf[200005];
        scanf("%s",buf);
        vector<int> s(n+2,0);
        ll c1=0;
        for(int i=1;i<=n;i++){ s[i]=buf[i-1]-'0'; c1+=s[i]; }
        ll S=0;
        auto w=[&](int j)->ll{ return (j>=1&&j<n)?(ll)j*(n-j):0; };
        for(int j=1;j<n;j++) if(s[j]!=s[j+1]) S+=w(j);
        auto ans=[&](){ return (S+c1*(n-c1))/2; };
        printf("%lld",ans());
        while(q--){
            int i;
            scanf("%d",&i);
            if(i>1){ if(s[i-1]!=s[i]) S-=w(i-1); else S+=w(i-1); }
            if(i<n){ if(s[i]!=s[i+1]) S-=w(i); else S+=w(i); }
            c1+=s[i]?-1:1;
            s[i]^=1;
            printf(" %lld",ans());
        }
        printf("\n");
    }
}
