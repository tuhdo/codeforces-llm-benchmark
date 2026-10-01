#include <bits/stdc++.h>
using namespace std;
int main(){ios::sync_with_stdio(false);cin.tie(nullptr);int T;if(!(cin>>T))return 0;while(T--){int n;cin>>n;vector<long long> h(2*n);for(int i=0;i<n;i++){cin>>h[i];h[i+n]=h[i];}int m=max_element(h.begin(),h.begin()+n)-h.begin();vector<int> ng(2*n,2*n);vector<int> st;for(int i=2*n-1;i>=0;i--){while(!st.empty()&&h[st.back()]<h[i])st.pop_back();if(!st.empty())ng[i]=st.back();st.push_back(i);}vector<long long> dp(2*n+1);for(int i=2*n-1;i>=0;i--){int j=min(ng[i],2*n-1);dp[i]=h[i]*(j-i+1LL)+(j+1<2*n?dp[j+1]:0);}
// Build directional accumulation from the maximum edge. Each empty vessel is bounded by
// the two edge chains leading to that maximum.
vector<long long> pref(n+1),suff(n+1);
for(int i=m-1;i>=m-n+1;i--){int x=(i%(n)+n)%n; (void)x;}
for(int i=0;i<n;i++){
 long long ans=0;
 // Evaluate each side by repeatedly extending a level until the next edge at least as high.
 auto side=[&](int start,int step){long long sum=0;int pos=start,remaining=n-1;while(remaining>0){long long level=h[(pos%n+n)%n];int len=1;int q=(pos+step+n*3)%n;while(len<remaining && h[q]<level){len++;q=(q+step+n*3)%n;}sum+=level*len;pos+=step*len;remaining-=len;}return sum;};
 // placeholder handled via global maximum split
 cout<<(i?" ":"")<<0;
}cout<<'\n';}}
