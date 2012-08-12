export uniform float fsum( uniform float vin[], uniform int n ) {
  float tot = 0;
  foreach( index=0 ... n ) {
    tot += sqrt(vin[index]);
  }
  return reduce_add(tot);
}

/*
export void simple(uniform float vin[], uniform float vout[], 
                   uniform int count) {
    foreach (index = 0 ... count) {
        // Load the appropriate input value for this program instance.
        float v = vin[index];

        // Do an arbitrary little computation, but at least make the
        // computation dependent on the value being processed
        if (v < 3.)
            v = v * v;
        else
            v = sqrt(v);

        // And write the result to the output array.
        vout[index] = v;
    }
}
*/