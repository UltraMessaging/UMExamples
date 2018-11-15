#!/bin/sh
# bld.sh - Builds html pages

if which semlit.sh >/dev/null; then :
else :
  echo "bld.sh: ERROR: semlit.sh not found in PATH."
  exit 1
fi

if [ -n "$1" ]; then :
  if [ ! -d "$1" ]; then :
    echo "$1 not found" >&2
    exit 1
  fi
  find "$1" -name README.txt -print >bld.tmp
else :
  # Find all the examples
  find * -name README.txt -print >bld.tmp
fi

# Build the semiliterate docs
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
    # Try compile (check for errors)
    if ls *.c >/dev/null 2>&1; then :
      echo "bld.sh: Info: gcc -I$LBM_PLATFORM/include -g -c *.c"
      gcc $* -I$LBM_PLATFORM/include -g -c *.c
      rm -f *.o
    fi
    cd ..
  done
  cd ..
done

echo "bld.sh: Info: building html index files"
find * -name README.txt -print >bld.tmp
perl bld.pl bld.tmp

rm bld.tmp
