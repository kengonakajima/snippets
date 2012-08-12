export uniform int isum( uniform int vin[], uniform int n ) {
    int tot=0;
    for(int i=0;i<n;i++){
        tot += vin[i];
    }
    return reduce_add(tot);    
}

