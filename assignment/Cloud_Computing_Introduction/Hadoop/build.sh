#! /bin/sh

find src -name '*.java' | xargs javac -sourcepath src -d bin -cp ~/local/opt/hadoop-0.20.203.0/hadoop-core-0.20.203.0.jar -Xlint:deprecation
jar -cvf WordCount.jar -C bin .
