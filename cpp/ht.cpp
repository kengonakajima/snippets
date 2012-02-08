

int _httoi(const char *value)
{
          struct CHexMap
          {
                  char chr;
                  int value;
          };
          const int HexMapL = 16;
            CHexMap HexMap[HexMapL] =
                    {
                            {'0', 0}, {'1', 1},
                            {'2', 2}, {'3', 3},
                            {'4', 4}, {'5', 5},
                            {'6', 6}, {'7', 7},
                            {'8', 8}, {'9', 9},
                            {'A', 10}, {'B', 11},
                            {'C', 12}, {'D', 13},
                            {'E', 14}, {'F', 15}
                    };
            char *mstr = _tcsupr(_tcsdup(value));
            char *s = mstr;
            int result = 0;
            if (*s == '0' && *(s + 1) == 'X') s += 2;
            bool firsttime = true;
            while (*s != '\0')
            {
                    bool found = false;
                    for (int i = 0; i < HexMapL; i++)
                    {
                            if (*s == HexMap[i].chr)
                            {
                                    if (!firsttime) result <<= 4;
                                    result |= HexMap[i].value;
                                    found = true;
                                    break;
                            }
                    }
                    if (!found) break;
                    s++;
                    firsttime = false;
            }
            free(mstr);
            return result;
}

int main(int argc, char* argv[])
{
        char *test[4] = {"0xFFFF", "0xabcd", "ffff", "ABCD"};
        for (int i = 0; i < 4; i++)
                _tprintf(_T("Hex String: %s is int: %d\n\r"), test[i], _httoi(test[i]));
        return 0;
}
