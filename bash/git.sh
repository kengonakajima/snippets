HOGE=$(git status --porcelain)

if [ "$HOGE" = "" ]; then
  echo clean
else
  echo not clean
fi
    