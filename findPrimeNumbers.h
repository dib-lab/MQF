#include <vector>
#include <stdint.h>
/* This function calculates (ab)%c */
int modulo(uint64_t a,uint64_t b,uint64_t c){
    uint64_t x=1,y=a; // uint64_t is taken to avoid overflow of intermediate results
    while(b > 0){
        if(b%2 == 1){
            x=(x*y)%c;
        }
        y = (y*y)%c; // squaring the base
        b /= 2;
    }
    return x%c;
}
/* this function calculates (a*b)%c taking into account that a*b might overflow */
uint64_t mulmod(uint64_t a,uint64_t b,uint64_t c){
    uint64_t x = 0,y=a%c;
    while(b > 0){
        if(b%2 == 1){
            x = (x+y)%c;
        }
        y = (y*2)%c;
        b /= 2;
    }
    return x%c;
}

/* Miller-Rabin primality test, iteration signifies the accuracy of the test */
bool Miller(uint64_t p,int iteration){
    if(p<2){
        return false;
    }
    if(p!=2 && p%2==0){
        return false;
    }
    uint64_t s=p-1;
    while(s%2==0){
        s/=2;
    }
    for(int i=0;i<iteration;i++){
        uint64_t a=rand()%(p-1)+1,temp=s;
        uint64_t mod=modulo(a,temp,p);
        while(temp!=p-1 && mod!=1 && mod!=p-1){
            mod=mulmod(mod,mod,p);
            temp *= 2;
        }
        if(mod!=p-1 && temp%2==0){
            return false;
        }
    }
    return true;
}
vector<uint64_t> findNPrimesLargerThanK(uint64_t N, uint64_t K)
{
  vector<uint64_t> res;
  uint64_t curr=K;
  while(res.size()<N){
    while(!Miller(++curr,20));
    res.push_back(curr);
  }
  return res;
}