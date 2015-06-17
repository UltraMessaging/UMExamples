#!/bin/sh
# bld.sh - Builds html pages

if which semlit.sh >/dev/null; then :
else :
  echo "bld.sh: ERROR: semlit.sh not found in PATH."
  exit 1
fi

# Find all the examples
find * -name README.txt -print >bld.tmp

# Build all the semiliterate docs
cat bld.tmp | while read F; do :  # find each tool
  T=`dirname $F`
  cd $T
  find * -name "$T.sldoc" -print | while read DOC; do :  # each language
    D=`dirname $DOC`
    echo "bld.sh: Info: $D/$T.sldoc: building semiliterate doc"
    cd $D
    chmod +w *.c *.h *.java *.cs >/dev/null 2>&1
    semlit.sh "$T.sldoc"
    # Don't let example maintainers edit the output files
    chmod -w *.c *.h *.java *.cs >/dev/null 2>&1
    cd ..
  done
  cd ..
done

echo "bld.sh: Info: building html index files"
perl bld.pl bld.tmp

rm bld.tmp
