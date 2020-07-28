unsigned int r( unsigned int m_z, unsigned int m_w) {
    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 18000 * (m_w & 65535) + (m_w >> 16);
    return (m_z << 16) + m_w;
}

int main() {
    for(int i=0;i<100;i++) {
        for(int j=0;j<100;j++) {
            unsigned int k=r(i,j) % 77;
            char c=' ';
            if(k==0) c='.';
            printf("%c",c);
        }
        printf("\n");
    }
}
