if grep tako poo; then
  echo found tako
fi

if grep ika poo; then
  echo found ika
fi

if xcodebuild -version -sdk | grep -q -i macosx10.12.sdk; then
  echo 10.12 found
fi
